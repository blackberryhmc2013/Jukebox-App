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

#ifndef CHAT_H_
#define CHAT_H_

#include <bb/device/WiFiDirect>
#include <bb/device/WiFiDirectResult>
#include <bb/device/WiFiDirectDeviceType>
#include <bb/device/WiFiDirectWpsMethod>
#include <bb/device/WiFiDirectIntent>
#include <bb/device/WiFiDirectConnectionState>
#include <bb/cascades/GroupDataModel>
#include <QMap>

#include "dataserver.h"
#include "dataclient.h"

class WDHandler : public QObject {
	Q_OBJECT

	Q_PROPERTY(bool stat READ stat WRITE setStat NOTIFY statChanged)
	Q_PROPERTY(bb::cascades::DataModel* model READ model NOTIFY modelChanged)

public:
	WDHandler(QObject* parent = 0);
	virtual ~WDHandler();

	void setStat(bool b);
	void addTarget(QString phoneId);

public slots:
	void scan();
	void requestConnect(const QString& hardwareAddress);
	void readyClient();
	void readyServer();
	void conDevs();

private slots:
	void scanCompleted();
	void deviceFound(const QString& name, const QString& hardwareAddress, bb::device::WiFiDirectDeviceType::Type type);
	void deviceLost(const QString& hardwareAddress);
	void sessionStarted(const QString& sessionName);
	void deviceConnected (const QString& name, const QString& hardwareAddress, bb::device::WiFiDirectDeviceType::Type type,
		const QString& ipAddress, bb::device::WiFiDirectIntent::Type intent);
	void deviceDisconnected (const QString& hardwareAddress);

signals:
	void serverReady();
	void clientReady();
	void scanStartResult(bool success, QString error);
	void connectionAttempt(bool success, QString error);
	void modelChanged();
	void statChanged();

private:
	QString resultToText(bb::device::WiFiDirectResult::Type result);

	Server m_server;
	QMap<QString, QTcpSocket*> phoneCons;
	bb::cascades::GroupDataModel* model() const;
	bool stat() const;

	bool m_stat;
	bb::device::WiFiDirect* m_direct;
	Client* m_client;

	bb::cascades::GroupDataModel* m_model;
	QString m_session;
	QString m_ipAddr;
};

#endif /* CHAT_H_ */
