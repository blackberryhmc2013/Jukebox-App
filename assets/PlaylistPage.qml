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
import bb.multimedia 1.0
import bb.cascades.multimedia 1.0
import bb.system 1.2

Page {
    titleBar: TitleBar {
        title: _playlist.name
    }
    
    Container {
        verticalAlignment: VerticalAlignment.Fill
        horizontalAlignment: HorizontalAlignment.Fill
        layout: DockLayout {
        
        }
       
        Playlist {
            id: playlist
        }
        
        NowPlaying {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Bottom
            visible: _playlist.stat
            leftPadding: 20
            rightPadding: 20
            topPadding: 20
            bottomPadding: 10
        }
    }
    
    paneProperties: NavigationPaneProperties {
        backButton: ActionItem {
            title: "Back"
            onTriggered: {
                _playlist.leave();
                navigationPane.pop();
            }
        }
    }
    
    actions: [
        ActionItem {
            title: "New Track"
            imageSource: "asset:///buttons/ic_add.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                filePicker.open();
            }
        },
        
        ActionItem {
            title: "Scan"
            imageSource: "asset:///buttons/ic_search.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                _playlist.scan();
            }
        
        }, 
        
        ActionItem {
            title: "Refresh"
            imageSource: "asset:///buttons/ic_reload.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                _playlist.refresh();
            }
        },
        
        ActionItem {
            title: "Disconnect"
            imageSource: "asset:///buttons/ic_clear.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                _playlist.disconnect();
            }
        }
    ]
    
    attachedObjects: [
        FilePicker {
            id: filePicker
            title: qsTr("Select Music")
            type: FileType.Music
            mode: FilePickerMode.Picker
            
            onFileSelected: {
                uploader.setSourceUrl(selectedFiles[0]);
                uploader.prepare();
            }
        },
        
        MediaPlayer {
            id: uploader
            onMetaDataChanged: {
                if (metaData.bit_rate || metaData.sample_rate) {
                    _playlist.addTrack(sourceUrl, metaData);
                }
            }
        }        
    ]
    
    onCreationCompleted: {
        _playlist.scan();
    }
}
