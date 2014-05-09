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

#ifndef PLAYLIST_H_
#define PLAYLIST_H_

#include <bb/device/WiFiDirect>
#include <bb/cascades/QListDataModel>

#include "CBHelper.h"
#include "server.h"
#include "client.h"
#include "nfcsender.h"
#include "nfcreceiver.h"

typedef bb::cascades::QListDataModel<QVariantMap> QMapListModel;

class Playlist : public QObject, Cloudbase::CBHelperResponder {
	Q_OBJECT

	Q_PROPERTY(QString myAddr READ myAddr CONSTANT)
	Q_PROPERTY(QString dataFolder READ dataFolder CONSTANT)

	Q_PROPERTY(QString playlistID READ playlistID WRITE setPlaylistID NOTIFY playlistIDChanged)
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(bool stat READ stat WRITE setStat NOTIFY statChanged)

	Q_PROPERTY(bb::cascades::DataModel* model READ model NOTIFY modelChanged)
	Q_PROPERTY(NfcReceiver* receiver READ receiver)

public:
	Playlist(QObject* parent = 0);
	virtual ~Playlist();

	void setPlaylistID(const QString& id);
	void setName(const QString& name);
	void setStat(bool b);

	void parseResponse(Cloudbase::CBHelperResponseInfo resp);

public slots:
	void create(const QString& name);
	void refresh();
	void leave();
	void addTrack(const QUrl& path, const QVariantMap& md);
	void deleteTrack(const QString& item);
	QVariant pop();

	void loadFirstTrack();
	void loadTrack(const QVariant& item);
	void scan();
	void requestConnect(const QString& hardwareAddress);
	void readyClient();
	void readyServer();
	void disconnect();

private slots:
	void onPlaylistInfoChanged();
	void onPlistInfoReceived(const QString& id, const QString name, bool stat);
	void onTrackReceived();

	void scanCompleted();
	void deviceFound(const QString& name, const QString& hardwareAddress, bb::device::WiFiDirectDeviceType::Type type);
	void deviceLost(const QString& hardwareAddress);
	void sessionStarted(const QString& sessionName);
	void sessionStopped();
	void deviceConnected (const QString& name, const QString& hardwareAddress, bb::device::WiFiDirectDeviceType::Type type,
		const QString& ipAddress, bb::device::WiFiDirectIntent::Type intent);
	void deviceDisconnected (const QString& hardwareAddress);
	void serverClientError(QAbstractSocket::SocketError err);

signals:
	void playlistIDChanged();
	void nameChanged();
	void statChanged();

	void modelChanged();
	void serverReady();
	void clientReady();

	void socketError(const QString& err);
	void wifiDirectError(const QString& err);

	void newPlaylist();

private:
	QString myAddr() const;
	QString dataFolder() const;
	QString playlistID() const;
	QString name() const;
	bool stat() const;
	bb::cascades::DataModel* model() const;
	Server* server() const;
	Client* client() const;
	NfcReceiver* receiver() const;

	int getmacaddr(char* ifname, char* macaddrstr);
	void deleteTracks(Cloudbase::CBHelperSearchCondition* cond);
	void wifiDirectResult(bb::device::WiFiDirectResult::Type result);
	void showErrorToast(const QString& err);

	QString m_myAddr;
	QString m_playlistID;
	QString m_name;
	bool m_stat;

	bb::device::WiFiDirect* m_direct;
	QMapListModel* m_model;
	Client* m_client;
	Server* m_server;
	QString m_session;
	QString m_ipAddr;
	quint16 m_port;

	NfcSender* m_sender;
	NfcReceiver* m_receiver;

	Cloudbase::CBHelper* m_helper;
	std::string m_collection;

	QVariant m_loadingTrack;
	QString m_dataFolder;
};

#endif /* PLAYLIST_H_ */
