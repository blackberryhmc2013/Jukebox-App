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

#ifndef PLAYLISTAPP_H_
#define PLAYLISTAPP_H_

#include <bb/cascades/QListDataModel>
#include <QTimer>

#include "nfcsender.h"
#include "nfcreceiver.h"
#include "playlist.h"

using namespace bb::cascades;

class PlaylistApp : public bb::cascades::QMapListDataModel {
	Q_OBJECT

	Q_PROPERTY(NfcSender* sender READ sender)
	Q_PROPERTY(NfcReceiver* receiver READ receiver)
	Q_PROPERTY(QString myHardware READ myHardware CONSTANT)
	//Q_PROPERTY(Playlist* playlist READ playlist CONSTANT)
public:
	PlaylistApp();
	virtual ~PlaylistApp();

	NfcSender* sender() const { return m_sender; }
	NfcReceiver* receiver() const { return m_receiver; }
	QString myHardware() const { return m_myHardware; }
	//Playlist* playlist () const { return m_playlist; }
public slots:
	QString createPlaylist(const QString& name);
	void deletePlaylist(const QString& id);
	void leavePlaylist(const QString& id);

signals:
	void currentPlaylistChanged();

private:
    QString m_myHardware;
	NfcSender* m_sender;
	NfcReceiver* m_receiver;
	//Playlist* m_playlist;
};

#endif /* PLAYLISTAPP_H_ */
