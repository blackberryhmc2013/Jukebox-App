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
import bb.multimedia 1.0


Container {
    id: nowPlaying
    
    layout: StackLayout {
        orientation: LayoutOrientation.TopToBottom
    }
    
    Container {
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
        }
        
        ImageView {
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1.0
            }
            verticalAlignment: VerticalAlignment.Center
            imageSource: "asset:///blankcover.jpg"
            scalingMethod: ScalingMethod.AspectFit
        }
        
        Container {        
            verticalAlignment: VerticalAlignment.Bottom
                
            layout: StackLayout {
                orientation: LayoutOrientation.TopToBottom
            }
            
            layoutProperties: StackLayoutProperties {
                spaceQuota: 2.0
            }
            
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.TopToBottom
                }
                
                Label {
                    text: musicPlayer.metaData.artist
                }
                
                Label {
                    text: musicPlayer.metaData.title
                }
                
                Label {
                    text: musicPlayer.metaData.album
                }
            }
            
            Container {
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    
                    Label {
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 1.0
                        }
                        property int hours: Math.floor(musicPlayer.position/1000/60/60)
                        property int minutes: Math.floor(musicPlayer.position/1000/60%60)
                        property int seconds: Math.floor(musicPlayer.position/1000%60)
                        
                        text: qsTr("%1:%2:%3").arg(hours < 10 ? "0" + hours : "" + hours)
                        .arg(minutes < 10 ? "0" + minutes : "" + minutes)
                        .arg(seconds < 10 ? "0" + seconds : "" + seconds)
                        textStyle {
                            base: SystemDefaults.TextStyles.SmallText
                            color: Color.Gray
                        }
                    }
                    
                    ProgressIndicator {
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 3.0
                        }
                        verticalAlignment: VerticalAlignment.Center
                        fromValue: 0.0
                        toValue: 1.0
                        value: musicPlayer.position/musicPlayer.duration
                    }
                    
                    Label {
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 1.0
                        }
                        property int hours: Math.floor(musicPlayer.duration/1000/60/60)
                        property int minutes: Math.floor(musicPlayer.duration/1000/60%60)
                        property int seconds: Math.floor(musicPlayer.duration/1000%60)
                        
                        text: qsTr("%1:%2:%3").arg(hours < 10 ? "0" + hours : "" + hours)
                        .arg(minutes < 10 ? "0" + minutes : "" + minutes)
                        .arg(seconds < 10 ? "0" + seconds : "" + seconds)
                        textStyle {
                            base: SystemDefaults.TextStyles.SmallText
                            color: Color.Gray
                        }
                    }
                }
                
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }

                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                    preferredHeight: 75
                    topMargin: 20
                    bottomMargin: 20
                    leftMargin: 100
                    rightMargin: 100

                    ImageButton {
                        id: playbtn

                        defaultImageSource: "asset:///buttons/play.png"
                        pressedImageSource: "asset:///buttons/playi.png"
                        onClicked: {
                            if (musicPlayer.mediaState == MediaState.Paused) {
                                musicPlayer.play();
                            } else if (musicPlayer.mediaState == MediaState.Started) {
                                musicPlayer.pause();
                            } else {
                                musicPlayer.setSourceUrl(playlist.pop().uri);
                                musicPlayer.prepare();
                                musicPlayer.play();
                            }
                        }

                        leftMargin: 100
                        rightMargin: 100
                        preferredWidth: 75
                    }

                    ImageButton {
                        id: nextbtn
                        defaultImageSource: "asset:///buttons/next.png"
                        pressedImageSource: "asset:///buttons/nexti.png"
                        onClicked: {
                            musicPlayer.setSourceUrl(playlist.pop().file_path);
                            musicPlayer.prepare();
                            musicPlayer.play();
                        }

                        leftMargin: 100
                        rightMargin: 100
                        preferredWidth: 75
                    }

                }
            }
        }
    }

    attachedObjects: [
        MediaPlayer {
            id: musicPlayer
            onPlaybackCompleted: {
                playlist.loadTrack(0);
                playlist.loadTrack(1);

                musicPlayer.setSourceUrl(playlist.pop().file_path);
                musicPlayer.prepare();
                musicPlayer.play();
            }

            onMediaStateChanged: {
                if (mediaState == MediaState.Started) {
                    playbtn.defaultImageSource = "asset:///buttons/pause.png";
                    playbtn.pressedImageSource = "asset:///buttons/pausei.png";
                } else {
                    playbtn.defaultImageSource = "asset:///buttons/play.png";
                    playbtn.pressedImageSource = "asset:///buttons/playi.png";
                }
            }
        }
    ]
}
