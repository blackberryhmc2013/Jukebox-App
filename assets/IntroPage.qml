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
import bb.system 1.2

Page {
    Container {
        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }
        
        Container {
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1.0
            }
            
            layout: DockLayout {
            
            }
            
            topPadding: 200
            bottomPadding: 200
            
            horizontalAlignment: HorizontalAlignment.Center
            
            ImageView {
                id: tapToJoinBtn
                imageSource: "asset:///buttons/tap.png"            
                verticalAlignment: VerticalAlignment.Center
                
            }
        }
        
        Divider {
        
        }
        
        Container {
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1.0
            }
            
            layout: DockLayout {
            
            }
            
            topPadding: 100
            bottomPadding: 100
            scaleX: 0.3
            scaleY: 0.3
            
            horizontalAlignment: HorizontalAlignment.Center
            
            
            ImageButton {
                id: createBtn
                defaultImageSource: "asset:///buttons/create.png"
                pressedImageSource: "asset:///buttons/createp.png"
                verticalAlignment: VerticalAlignment.Center
                
                onClicked: {
                    newPlaylist.show();
                }
            }
        }
    }
    
    attachedObjects: [
        SystemPrompt {
            id: newPlaylist
            title: "New Playlist"
            body: "Enter a name."
            confirmButton.enabled: true
            customButton.enabled: false
            inputField.inputMode: SystemUiInputMode.Default
            returnKeyAction: SystemUiReturnKeyAction.Submit
            dismissAutomatically: true
            emoticonsEnabled: true
            includeRememberMe: false
            rememberMeChecked: false
            onFinished: {
                if (result == SystemUiResult.ConfirmButtonSelection) {
                    _playlist.create(inputFieldTextEntry());
                    var page = playlistPageDef.createObject();
                    navigationPane.push(page);
                }
            }
        },
        
        ComponentDefinition {
                    id: playlistPageDef
                    source: "asset:///PlaylistPage.qml"
        }
    ]
}