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

#include <bb/device/WiFiDirectResult>
#include <bb/device/WiFiDirectDeviceType>
#include <bb/device/WiFiDirectWpsMethod>
#include <bb/device/WiFiDirectIntent>
#include <bb/device/WiFiDirectConnectionState>
#include <QTimer>
#include <bb/system/SystemToast>

#include <string.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <net/if_dl.h>

#include "playlist.h"
#include "track.h"

int listmacaddrs() {
	struct ifaddrs *ifap, *ifaptr;
	unsigned char *ptr;

	if (getifaddrs(&ifap) == 0) {
		for (ifaptr = ifap; ifaptr != NULL; ifaptr = (ifaptr)->ifa_next) {
			if (((ifaptr)->ifa_addr)->sa_family == AF_LINK) {
				ptr = (unsigned char *) LLADDR((struct sockaddr_dl *)(ifaptr)->ifa_addr);
				printf("%s: %02x:%02x:%02x:%02x:%02x:%02x\n",
						(ifaptr)->ifa_name, *ptr, *(ptr + 1), *(ptr + 2),
						*(ptr + 3), *(ptr + 4), *(ptr + 5));
			}
		}

		freeifaddrs(ifap);
		return 1;
	} else {
		return 0;
	}
}

Playlist::Playlist(QObject* parent)
: QObject(parent), m_stat(false),
  m_direct(new bb::device::WiFiDirect(this)), m_server(NULL), m_client(NULL),
  m_ipAddr("127.0.0.1"), m_session(""), m_port(4355),
  m_sender(new NfcSender(this)), m_receiver(new NfcReceiver(this)),
  m_collection("jukebox_db"), m_model(NULL), m_dataFolder(QDir::currentPath() + "/data/")
{
	QString appCode = "APP_CODE";
	QString appSecret = "APP_SECRET";
	QString appPwd = "APP_PASS";

	m_helper = new Cloudbase::CBHelper(appCode.toStdString(), appSecret.toStdString());
	QString password = QString(QCryptographicHash::hash(appPwd.toUtf8(), QCryptographicHash::Md5).toHex());
	m_helper->setPassword(password.toStdString());

	QSettings settings;
	settings.setValue("appCode", appCode);
	settings.setValue("appSecret", appSecret);
	settings.setValue("appPwd", password);

	if ( !settings.value("appCode").isNull() ) {
		// TODO: set value in fields here.
	}

	connect(m_direct, SIGNAL(scanCompleted()), this, SLOT(scanCompleted()));
	connect(m_direct, SIGNAL(deviceFound(const QString, const QString, bb::device::WiFiDirectDeviceType::Type)),
			this, SLOT(deviceFound(const QString, const QString, bb::device::WiFiDirectDeviceType::Type)));
	connect(m_direct, SIGNAL(deviceLost(const QString)), this, SLOT(deviceLost(const QString)));
	connect(m_direct, SIGNAL(sessionStarted(const QString)), this, SLOT(sessionStarted(const QString)));
	connect(m_direct, SIGNAL(sessionStopped()), this, SLOT(sessionStopped()));
	connect(m_direct, SIGNAL(deviceConnected(const QString, const QString, bb::device::WiFiDirectDeviceType::Type, const QString, bb::device::WiFiDirectIntent::Type)),
			this, SLOT(deviceConnected(const QString, const QString, bb::device::WiFiDirectDeviceType::Type, const QString, bb::device::WiFiDirectIntent::Type)));
	connect(m_direct, SIGNAL(deviceDisconnected(const QString)), this, SLOT(deviceDisconnected(const QString)));

	connect(this, SIGNAL(statChanged()), this, SLOT(onPlaylistInfoChanged()));
	connect(this, SIGNAL(playlistIDChanged()), this, SLOT(onPlaylistInfoChanged()));
	connect(this, SIGNAL(nameChanged()), this, SLOT(onPlaylistInfoChanged()));
	connect(m_receiver, SIGNAL(plistInfoReceived(const QString, const QString, bool)),
			this, SLOT(onPlistInfoReceived(const QString, const QString, bool)));

	char macaddrstr[17];

	if (getmacaddr("bcm0", macaddrstr)) {
		printf("bcm0: %s\n", macaddrstr);
		char bit = macaddrstr[1];

		if (bit < 54) {
			if (bit % 4 == 0 || bit % 4 == 1) {
				macaddrstr[1] += 2;
			}
		} else if (bit < 58) {
			if (bit % 4 == 0 || bit % 4 == 1) {
				macaddrstr[1] += 41;
			}
		} else if (bit > 98) {
			if (bit % 4 == 3 || bit % 4 == 0) {
				macaddrstr[1] += 2;
			}
		}
	} else {
		printf("bcm0: not found\n");
	}

	fflush(stdout);
	m_myAddr = QString::fromUtf8(macaddrstr);
}

Playlist::~Playlist()
{
	leave();

	if (m_client != NULL) {
		QObject::disconnect(m_client, SIGNAL(trackReceived()));
		QObject::disconnect(m_client, SIGNAL(error(QAbstractSocket::SocketError)));
		m_client->deleteLater();
	}

	if (m_server != NULL) {
		QObject::disconnect(m_server, SIGNAL(error(QAbstractSocket::SocketError)));
		m_server->deleteLater();
	}

	QObject::disconnect(m_direct, SIGNAL(scanCompleted()));
	QObject::disconnect(m_direct, SIGNAL(deviceFound(const QString, const QString, bb::device::WiFiDirectDeviceType::Type)));
	QObject::disconnect(m_direct, SIGNAL(deviceLost(const QString)));
	QObject::disconnect(m_direct, SIGNAL(sessionStarted(const QString)));
	QObject::disconnect(m_direct, SIGNAL(sessionStopped()));
	QObject::disconnect(m_direct, SIGNAL(deviceConnected(const QString, const QString, bb::device::WiFiDirectDeviceType::Type, const QString, bb::device::WiFiDirectIntent::Type)));
	QObject::disconnect(m_direct, SIGNAL(deviceDisconnected(const QString)));

	m_direct->deleteLater();
	m_sender->deleteLater();
	m_receiver->deleteLater();
	m_helper->deleteLater();

	// TODO
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Playlist::create(const QString& name)
{
	QUuid uid = QUuid::createUuid();
	setPlaylistID(uid.toString());
	setName(name);
	setStat(true);
	m_model = new QMapListModel();
}

void Playlist::leave()
{
	// Disconnect WiFi Direct
	disconnect();

	// Clean up cloud
	Cloudbase::CBHelperSearchCondition* cond = new CBHelperSearchCondition();
	cond->addCondition(new CBHelperSearchCondition("removed", "false", CBOperatorEqual));
	cond->addCondition(new CBHelperSearchCondition("playlist_id", m_playlistID.toStdString(), CBOperatorEqual));

	if (m_stat) {
		setStat(false);
		QDir data(m_dataFolder);
		QStringList deletees = data.entryList(QDir::Files);

		for (int i = 0; i < deletees.size(); ++i) {
			deleteTrack(m_dataFolder + deletees[i]);
		}
	} else {
		cond->addCondition(new CBHelperSearchCondition("owner_addr", m_myAddr.toStdString(), CBOperatorEqual));
	}

	deleteTracks(cond);
	setPlaylistID("");
	setName("");

	if (m_model != NULL) {
		m_model->clear();
		delete m_model;
		m_model = NULL;
	}
}

void Playlist::refresh()
{
	if (m_model != NULL) {
		Cloudbase::CBHelperSearchCondition* cond = new CBHelperSearchCondition();
		cond->addCondition(new CBHelperSearchCondition("removed", "false", CBOperatorEqual));
		cond->addCondition(new CBHelperSearchCondition("playlist_id", m_playlistID.toStdString(), CBOperatorEqual));
		m_helper->searchDocument(m_collection, cond, this);
	}
}

void Playlist::onPlaylistInfoChanged()
{
	m_sender->setActive(m_stat);

	if (m_stat) {
		QString pinfo = "[ {\"is_jukebox\" : \"false\", \"playlist_id\" : \"" + m_playlistID + "\", \"playlist_name\" : \"" + m_name + "\"} ]";
		m_sender->setPlaylistInfo(pinfo);
	}
}

void Playlist::onPlistInfoReceived(const QString& id, const QString name, bool stat)
{
	leave();
	setPlaylistID(id);
	setName(name);
	setStat(stat);
	m_model = new QMapListModel();
	emit newPlaylist();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Playlist::addTrack(const QUrl& path, const QVariantMap& md)
{
	Track* addee = new Track(m_playlistID, m_myAddr, path, md);
	m_helper->insertDocument(m_collection, dynamic_cast<Cloudbase::CBSerializable*>(addee), NULL);
}

void Playlist::deleteTrack(const QString& item)
{
	qDebug() << "Deleting item at: " << item;
	QFile::remove(item);
}

void Playlist::deleteTracks(Cloudbase::CBHelperSearchCondition* cond)
{
	Track* deletee = new Track("removed", true);
	m_helper->updateDocument(m_collection, dynamic_cast<Cloudbase::CBSerializable*>(deletee), cond);
}

QVariant Playlist::pop()
{
	if (m_model->size() == 0) {
		QVariantMap empty;
		empty["save_as"] = QVariant("");
		return QVariant(empty);
	} else {
		QVariant first = m_model->value(0);
		m_model->removeAt(0);
		QVariantMap firstMap = first.toMap();
		Cloudbase::CBHelperSearchCondition* cond = new CBHelperSearchCondition("track_id", firstMap["track_id"].toString().toStdString(), CBOperatorEqual);
		deleteTracks(cond);
		//loadFirstTrack();
		return first;
	}
}

void Playlist::parseResponse(Cloudbase::CBHelperResponseInfo resp)
{
	YAJLDom::Value* parsedMsg = resp.parsedMessage;
	m_model->clear();

	for (int i = 0; i < parsedMsg->getNumChildValues(); i++) {
		YAJLDom::Value* current = parsedMsg->getValueByIndex(i);
		std::string tid = current->getValueForKey("track_id")->toString();
		std::string owner = current->getValueForKey("owner_addr")->toString();
		std::string path = current->getValueForKey("file_path")->toString();
		std::string save_as = current->getValueForKey("save_as")->toString();

		std::string title = current->getValueForKey("title")->toString();
		std::string artist = current->getValueForKey("artist")->toString();
		std::string album = current->getValueForKey("album")->toString();
		std::string duration = current->getValueForKey("duration")->toString();
		std::string bit_rate = current->getValueForKey("bit_rate")->toString();
		std::string md_title_albumartist = current->getValueForKey("md_title_albumartist")->toString();
		std::string md_title_year = current->getValueForKey("md_title_year")->toString();
		std::string track = current->getValueForKey("track")->toString();
		std::string sample_rate = current->getValueForKey("sample_rate")->toString();

		QVariantMap currentItem;
		currentItem["track_id"] = QVariant(tid.c_str());
		currentItem["owner_addr"] = QVariant(owner.c_str());
		currentItem["file_path"] = QVariant(QUrl(QString(path.c_str())));
		currentItem["save_as"] = QVariant(QString(save_as.c_str()));
		currentItem["title"] = QVariant(title.c_str());
		currentItem["artist"] = QVariant(artist.c_str());
		currentItem["album"] = QVariant(album.c_str());
		currentItem["duration"] = QVariant(duration.c_str());
		currentItem["bit_rate"] = QVariant(bit_rate.c_str());
		currentItem["md_title_albumartist"] = QVariant(md_title_albumartist.c_str());
		currentItem["md_title_year"] = QVariant(md_title_year.c_str());
		currentItem["track"] = QVariant(track.c_str());
		currentItem["sample_rate"] = QVariant(sample_rate.c_str());
		currentItem["loaded"] = QVariant(QFile::exists(m_dataFolder + QString::fromStdString(save_as)));

		m_model->append(currentItem);
	}

	//loadFirstTrack();

	emit modelChanged();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Playlist::loadFirstTrack()
{
	if (m_model->size() > 0) {
		QVariant item = m_model->value(0);
		QVariantMap map = item.toMap();

		if (!map["loaded"].toBool()) {
			m_loadingTrack = item;
			QString owner = map["owner_addr"].toString();
			QString save = map["save_as"].toString();

			if (owner == m_myAddr) {
				if (QFile::copy(map["file_path"].toString(), m_dataFolder + map["save_as"].toString())) {
					//
				}
			} else { // TODO set the item's loaded to true if file received
				disconnect();

				if (m_direct->isScanning()) {
					QStringList visDevs = m_direct->visibleDevices();
					int i = visDevs.indexOf(owner);

					if (i >= 0) {
						requestConnect(visDevs[i]);
					}
				} else {
					scan();
				}
			}
		}
	}
}

void Playlist::loadTrack(const QVariant& item)
{
	QVariantMap map = item.toMap();

	// TODO MAYBE IN DEVICE FOUND?
	if (!map["loaded"].toBool()) {
		m_loadingTrack = item;
		QString owner = map["owner_addr"].toString();
		QString save = map["save_as"].toString();

		if (owner == m_myAddr) {
			if (QFile::copy(map["file_path"].toString(), m_dataFolder + map["save_as"].toString())) {
				// set the item's loaded to true
			}
		} else { // TODO set the item's loaded to true if file received
			disconnect();

			if (m_direct->isScanning()) {
				QStringList visDevs = m_direct->visibleDevices();
				int i = visDevs.indexOf(owner);

				if (i >= 0) {
					requestConnect(visDevs[i]);
				}
			} else {
				scan();
			}
		}
	}
}

void Playlist::onTrackReceived()
{
	showErrorToast("Track Received!");
	scan();
}

void Playlist::disconnect()
{
	if (m_direct->connectionState() == bb::device::WiFiDirectConnectionState::Connected) {
		if (m_session.size() == 0) {
			qDebug() << "WiFi Direct or Miracast in use";
		} else {
			qDebug() << "Disconnecting..";

			if (m_stat) {
				if (m_client != NULL) {
					qDebug() << "Disconnecting Client..";
					m_client->disconnect();
				}
			} else {
				if (m_server != NULL) {
					qDebug() << "Disconnecting Server..";
					m_server->disconnect();
				}
			}

			qDebug() << "Stopping session...";
			m_direct->requestStopSession(m_session);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Playlist::scan()
{
	disconnect();

	qDebug() << "Started scanning...";
	bb::device::WiFiDirectResult::Type result = m_direct->requestScan();

	if (result != bb::device::WiFiDirectResult::Success) {
		wifiDirectResult(result);
	}
}

void Playlist::scanCompleted()
{
	qDebug() << "Scan completed!";

	if (m_direct->connectionState() == bb::device::WiFiDirectConnectionState::Disconnected) {
		scan();
	}
}

void Playlist::requestConnect(const QString& hardwareAddress)
{
	qDebug() << "Connecting to: " << hardwareAddress;
	bb::device::WiFiDirectResult::Type result = m_direct->requestConnect(hardwareAddress,
			bb::device::WiFiDirectIntent::GroupOwner,
			bb::device::WiFiDirectWpsMethod::PushButtonConfiguration, "");

	if (result != bb::device::WiFiDirectResult::Success) {
		wifiDirectResult(result);
	}
}

void Playlist::deviceConnected (const QString& name, const QString& hardwareAddress,
		bb::device::WiFiDirectDeviceType::Type type,
		const QString& ipAddress, bb::device::WiFiDirectIntent::Type intent)
{
	qDebug() << "Device Connected: (" + name + ", " + hardwareAddress + ", " << type << ", " + ipAddress + ", "  << intent << ")";
	m_ipAddr = ipAddress;

	if (m_stat) {
		readyClient();
	} else {
		readyServer();
	}
}

void Playlist::readyClient()
{
	m_direct->requestStartSession();
	m_client = new Client(m_dataFolder, this);
	connect(m_client, SIGNAL(trackReceived()), this, SLOT(onTrackReceived()));
	connect(m_client, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(serverClientError(QAbstractSocket::SocketError)));
	QVariantMap map = m_loadingTrack.toMap();
	m_client->track_id = map["track_id"].toString();
	m_client->file_path = map["file_path"].toString();
	m_client->save_as = map["save_as"].toString();
	m_client->connectToHost(m_ipAddr, m_port);
	emit clientReady();
}

void Playlist::readyServer()
{
	m_direct->requestStartSession();
	m_server = new Server(this);
	connect(m_server, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(serverClientError(QAbstractSocket::SocketError)));
	m_server->listen(m_port);
	emit serverReady();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Playlist::deviceFound(const QString& name, const QString& hardwareAddress, bb::device::WiFiDirectDeviceType::Type type)
{
	if (type == bb::device::WiFiDirectDeviceType::Telephone) {
		showErrorToast("Device Found: " + name);
	}

	qDebug() << "Device Found: (" + name + ", " + hardwareAddress + ", " + type + ")";
}

void Playlist::deviceLost (const QString& hardwareAddress)
{
	//showErrorToast("Device Lost: " + hardwareAddress);
	qDebug() << "Device Lost: " + hardwareAddress;
}

void Playlist::deviceDisconnected(const QString& hardwareAddress)
{
	qDebug() << "Device Disconnected: " << hardwareAddress;
}

void Playlist::sessionStarted(const QString& sessionName)
{
	qDebug() << "Session started: " << sessionName;
	m_session = sessionName;
}

void Playlist::sessionStopped()
{
	qDebug() << "Session stopped.";
	m_session = "";
	QTimer::singleShot(120000, this, SLOT(scan())); // TODO even after 2 mins, still connected.
}

void Playlist::showErrorToast(const QString& err)
{
	bb::system::SystemToast* toast = new bb::system::SystemToast(this);
	toast->setPosition(bb::system::SystemUiPosition::MiddleCenter);
	toast->setModality(bb::system::SystemUiModality::Application);
	toast->setBody(err);

	bb::system::SystemUiButton* toastButton = toast->button();
	toastButton->setLabel("Ok");
	toastButton->setEnabled(true);

	toast->show();
	connect(toast, SIGNAL(finished(bb::system::SystemUiResult::Type)), toast, SLOT(deleteLater()));
}

void Playlist::serverClientError(QAbstractSocket::SocketError err)
{
	QString resultError = "Unknown error has occurred.";

	switch (err) {
		case QAbstractSocket::ConnectionRefusedError:
			resultError = "The connection was refused by the peer (or timed out).";
			break;
		case QAbstractSocket::RemoteHostClosedError:
			resultError = "The remote host closed the connection.";
			break;
		case QAbstractSocket::HostNotFoundError:
			resultError = "The host address was not found.";
			break;
		case QAbstractSocket::SocketAccessError:
			resultError = "The socket operation failed because the application lacked the required privileges.";
			break;
		case QAbstractSocket::SocketResourceError:
			resultError = "The local system ran out of resources (e.g., too many sockets).";
			break;
		case QAbstractSocket::SocketTimeoutError:
			resultError = "The socket operation timed out.";
			break;
		case QAbstractSocket::DatagramTooLargeError:
			resultError = "The datagram was larger than the operating system's limit.";
			break;
		case QAbstractSocket::NetworkError:
			resultError = "An error occurred with the network (e.g., the network cable was accidentally plugged out).";
			break;
		case QAbstractSocket::AddressInUseError:
			resultError = "The address specified to bind is already in use and was set to be exclusive.";
			break;
		case QAbstractSocket::SocketAddressNotAvailableError:
			resultError = "The address specified to QUdpSocket::bind() does not belong to the host.";
			break;
		case QAbstractSocket::UnsupportedSocketOperationError:
			resultError = "The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support).";
			break;
		case QAbstractSocket::ProxyAuthenticationRequiredError:
			resultError = "The socket is using a proxy, and the proxy requires authentication.";
			break;
		case QAbstractSocket::SslHandshakeFailedError:
			resultError = "The SSL/TLS handshake failed, so the connection was closed.";
			break;
		case QAbstractSocket::UnfinishedSocketOperationError:
			resultError = "The last operation attempted has not finished yet (still in progress in the background).";
			break;
		case QAbstractSocket::ProxyConnectionRefusedError:
			resultError = "Could not contact the proxy server because the connection to that server was denied.";
			break;
		case QAbstractSocket::ProxyConnectionClosedError:
			resultError = "The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established).";
			break;
		case QAbstractSocket::ProxyConnectionTimeoutError:
			resultError = "The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase.";
			break;
		case QAbstractSocket::ProxyNotFoundError:
			resultError = "The proxy address set with setProxy() (or the application proxy) was not found.";
			break;
		case QAbstractSocket::ProxyProtocolError:
			resultError = "The connection negotiation with the proxy server because the response from the proxy server could not be understood.";
			break;
		case QAbstractSocket::UnknownSocketError:
			resultError = "An unidentified error occurred.";
			break;
	}

	showErrorToast(resultError);
}

void Playlist::wifiDirectResult(bb::device::WiFiDirectResult::Type result)
{
	QString resultError = "Unknown error has occurred.";

	switch (result) {
		case bb::device::WiFiDirectResult::Success:
			break;

		case bb::device::WiFiDirectResult::InternalError:
			resultError = "An unrecoverable internal error occurred.";
			break;

		case bb::device::WiFiDirectResult::Busy:
			resultError =
					"Cannot scan as system is busy and being used by other WiFi direct service (eg.Miracast).";
			break;

		case bb::device::WiFiDirectResult::DeviceNotFound:
			resultError = "Device with specified hardware address was not found.";
			break;

		case bb::device::WiFiDirectResult::NoNetworkConnection:
			resultError = "Device was not connected to a network.";
			break;

		case bb::device::WiFiDirectResult::NoRequestToCancel:
			resultError = "No active connection attempt available to cancel.";
			break;

		case bb::device::WiFiDirectResult::NotSupported:
			resultError = "WiFiDirect is not supported on this device.";
			break;

		case bb::device::WiFiDirectResult::InvalidSession:
			resultError = "The supplied session was not found.";
			break;
		}

	showErrorToast(resultError);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Playlist::getmacaddr(char *ifname, char *macaddrstr) {
	struct ifaddrs *ifap, *ifaptr;
	unsigned char *ptr;

	if (getifaddrs(&ifap) == 0) {
		for (ifaptr = ifap; ifaptr != NULL; ifaptr = (ifaptr)->ifa_next) {
			if (!strcmp((ifaptr)->ifa_name, ifname)
					&& (((ifaptr)->ifa_addr)->sa_family == AF_LINK)) {
				ptr = (unsigned char *) LLADDR((struct sockaddr_dl *)(ifaptr)->ifa_addr);
				sprintf(macaddrstr, "%02x:%02x:%02x:%02x:%02x:%02x", *ptr,
						*(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4),
						*(ptr + 5));
				break;
			}
		}

		freeifaddrs(ifap);
		return ifaptr != NULL;
	} else {
		return 0;
	}
}

QString Playlist::myAddr() const
{
	return m_myAddr;
}

QString Playlist::dataFolder() const
{
	return m_dataFolder;
}

QString Playlist::playlistID() const
{
	return m_playlistID;
}

QString Playlist::name() const
{
	return m_name;
}

bool Playlist::stat() const
{
	return m_stat;
}

bb::cascades::DataModel* Playlist::model() const
{
    return m_model;
}

Server* Playlist::server() const
{
	return m_server;
}

Client* Playlist::client() const
{
	return m_client;
}

NfcReceiver* Playlist::receiver() const
{
	return m_receiver;
}

void Playlist::setPlaylistID(const QString& id)
{
	if (m_playlistID != id) {
		m_playlistID = id;
		emit playlistIDChanged();
	}
}

void Playlist::setName(const QString& name)
{
	if (m_name != name) {
		m_name = name;
		emit nameChanged();
	}
}

void Playlist::setStat(bool b)
{
	if (m_stat != b) {
		m_stat = b;
		emit statChanged();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


