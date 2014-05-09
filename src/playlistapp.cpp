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

#include "playlistapp.h"
#include <bb/device/HardwareInfo>


PlaylistApp::PlaylistApp()
: m_receiver(new NfcReceiver(this))
{
	bb::device::HardwareInfo ni;
	m_myHardware = ni.pin();
}

PlaylistApp::~PlaylistApp()
{
	// TODO Auto-generated destructor stub

}

QString PlaylistApp::createPlaylist(const QString& name)
{
	QString id = "jukebox_test"; // TODO
	//m_playlist = new Playlist(id, name, m_myHardware);
	QString pinfo = "[ {\"host_dev_name\" : \"" + m_myHardware + "\", \"playlist_id\" : \"" + id + "\", \"playlist_name\" : \"" + name + "\"} ]";
	return id;
}

void PlaylistApp::deletePlaylist(const QString& id)
{
	//delete m_playlist;
	//m_playlist = NULL;
	//m_sender->setActive(false);
	//m_sender->setPlaylistInfo("");
}

void PlaylistApp::leavePlaylist(const QString& id)
{
	//delete m_playlist;
	//m_playlist = NULL;
}
