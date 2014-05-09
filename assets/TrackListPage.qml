/* Copyright (c) 2013-2014 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import bb.cascades 1.2
import bb.cascades.pickers 1.0
import com.jukebox.playlist 1.0
import bb.multimedia 1.0
import bb.cascades.multimedia 1.0

Page {
    titleBar: TitleBar {
        title: playlist.pName
    }
    
    property string playlist_id
    property string playlist_name
    property string host_addr
    
    Container {
        TrackList {
            id: trackList
            attachedObjects: [
                Playlist {
                    id: playlist
                    pID: playlist_id
                    pName: playlist_name
                    hostAddr: host_addr
                }
            ]
        }
        
        NowPlaying {
            enabled: (_playlistApp.myHardware == playlist.hostAddr)
        }
    }
    
    paneProperties: NavigationPaneProperties {
        backButton: ActionItem {
            title: "Back"
            onTriggered: {
                //playlistApp.delete
                _playlistApp.deletePlaylist(playlist_id);
                
                if (_playlistApp.myHardware == playlist.hostAddr) {
                    playlist.remove();
                } else {
                    playlist.leave();
                }
                
                navigationPane.pop();
            }
        }
    }
    
    actions: [
        ActionItem {
            title: "Add Track"
            onTriggered: {
                filePicker.open();
            }
            ActionBar.placement: ActionBarPlacement.OnBar
        },
        ActionItem {
                    title: "Add Member"
                    onTriggered: {
                        // NFC tap to add new phone to group
                        playlist.addMember();
                    }
                    ActionBar.placement: ActionBarPlacement.OnBar
            imageSource: "asset:///buttons/tap.png"
        }
    ]
    
    attachedObjects: [
        FilePicker {
            id: filePicker
            title: qsTr("Select Music")
            type: FileType.Music
            mode: FilePickerMode.Picker
            //directories: "/accounts/1000/shared/downloads/"
            
            onFileSelected: {
                uploader.setSourceUrl(selectedFiles[0]);
                uploader.prepare();
            }
        },
        
        MediaPlayer {
            id: uploader
            onMetaDataChanged: {
                if (metaData.bit_rate || metaData.sample_rate) {
                    playlist.addTrack(sourceUrl, metaData);
                }
            }
        }		
	]
}
