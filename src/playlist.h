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

#include <bb/cascades/QListDataModel>
#include <bb/device/WiFiDirect>
#include <bb/device/WiFiDirectResult>
#include <bb/device/WiFiDirectDeviceType>
#include <bb/device/WiFiDirectWpsMethod>
#include <bb/device/WiFiDirectIntent>
#include <bb/device/WiFiDirectConnectionState>
#include <cloudList/CloudList.h>

#include "nfcsender.h"
#include "track.h"
#include "wifidirect/wdhandler.h"
#include "wifidirect/WDListener.h"

using namespace bb::cascades;

class Playlist : public bb::cascades::QListDataModel<QVariantMap> {
	Q_OBJECT

	Q_PROPERTY(QString pID READ pID WRITE setPID NOTIFY pIDChanged)
	Q_PROPERTY(QString pName READ pName WRITE setPName NOTIFY pNameChanged)
	Q_PROPERTY(QString hostAddr READ hostAddr WRITE setHostAddr NOTIFY hostAddrChanged)
public:
	Playlist();
	Playlist(const QString& id);
	Playlist(const QString& id, const QString& name, const QString& host);
	void cbSetup();
	~Playlist();

	QString resultToText(bb::device::WiFiDirectResult::Type result);

	QString pID() const { return m_pID; }
	QString pName() const { return m_pName; }
	QString hostAddr() const { return m_hostAddr; }

	void setPID(const QString& id);
	void setPName(const QString & name);
	void setHostAddr(const QString & addr);

public slots:
	void remove();
	void leave();
	void addTrack(const QUrl& path, const QVariantMap& md);
	QVariant pop();
	void loadTrack(int num);
	void onTrackLoaded(const QString& id, const QString& path);
	void addMember();

signals:
	void pIDChanged();
	void pNameChanged();
	void hostAddrChanged();

private:
	QString m_devName;

	QString m_collection;
	QString m_hostAddr;
	QString m_pID;
	QString m_pName;

	QList<QVariantMap> m_deviceList;
	QString m_sessionName;
	QString m_IPAddr;

	CloudList<Track> tracks_;
	WDHandler* m_wdhandler;
	WDListener* m_wdlistener;
private slots:
	void onItemAdded(int index);
	void onItemDeleted(int index);
};

#endif /* PLAYLIST_H_ */
