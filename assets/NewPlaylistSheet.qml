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

Sheet {
    id: addSheet
    
    Page {
        titleBar: TitleBar {
            title: "Create Playlist"
            dismissAction: ActionItem {
                title: qsTr("Cancel") + Retranslate.onLanguageChanged
                onTriggered: {
                    addSheet.close();
                }
            }
            
            acceptAction: ActionItem {
                title: qsTr("Submit") + Retranslate.onLanguageChanged
                onTriggered: {
                    var id = _playlistApp.createPlaylist(plistName.text);
                    var page = trackListPageDef.createObject();
                    page.playlist_id = id;
                    page.playlist_name = plistName.text;
                    page.host_addr = _playlistApp.myHardware;
                    navigationPane.push(page);
                    addSheet.close();
                }
            }
        }
        
        Container {
            Label {
                text: "Name of Playlist: "
            }
            
            TextField {
                id: plistName
                enabled: false
                hintText: qsTr("Enter name.") + Retranslate.onLanguageChanged
                text: "Jukebox"
            }
        }
    }
    
    attachedObjects: [
        ComponentDefinition {
            id: trackListPageDef
            source: "TrackListPage.qml"
        }
    ]
}