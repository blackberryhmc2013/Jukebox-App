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

Page {
    id: trackPage
    
    //property variant playlist
    property variant tItem
    
    titleBar: TitleBar {
        title: tItem.title
    }
    
    Container {
        topPadding: 20
        bottomPadding: 20
        leftPadding: 20
        rightPadding: 20
        
        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }

        ImageView {
            imageSource: "asset:///blankcover.jpg"
            scalingMethod: ScalingMethod.AspectFit
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            layoutProperties: StackLayoutProperties {
                spaceQuota: 2.0
            }
        }
        
        /*
        ListView {
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1.0
            }
            
            listItemComponents: [
                ListItemComponent {
                    StandardListItem {
                        title: tItem.artist
                        description: "Artist"
                    }
                },
                
                ListItemComponent {
                    StandardListItem {
                        title: tItem.title
                        description: "Title"
                    }
                },
                
                ListItemComponent {
                    StandardListItem {
                        title: tItem.album
                        description: "Album"
                    }
                }
            ]
        }
        */
        Container {   
            layoutProperties: StackLayoutProperties {
                spaceQuota: 3.0
            }

            Divider {
            	
            }
            /*
             Label {
             text: tItem.track_id
             }
             
             Label {
             text: tItem.owner_addr
             }
             
             Label {
             text: tItem.file_path
             }
             */
               
            Label {
                text: "Title: " + tItem.title
            }
                  
            Label {
                text: "Artist: " + tItem.artist
            }
            
            Label {
                text: "Duration: " + tItem.duration
            }
            
            Label {
                text: "Album: " + tItem.album
            }
            
            Label {
                text: "Album Artist: " + tItem.md_title_albumartist
            }
            
            Label {
                text: "Track: " + tItem.track
            }
            
            Label {
                text: "Year: " + tItem.md_title_year
            }
            
            Label {
                text: "Bit Rate: " + tItem.bit_rate
            }
        
            Label {
                text: "Sample Rate: " + tItem.sample_rate
            }
        }        
    }
}
