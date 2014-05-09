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

#ifndef WDCLIENT_H_
#define WDCLIENT_H_

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <cstdio>

class WDClient : public QObject {
	Q_OBJECT
public:
	WDClient(QObject* parent = 0);
	virtual ~WDClient();

	void connectToHost(const QString& serverAddr);

public slots:
	void connected();
	void onDisconnected();
	void disconnect();
	void receiveRequest();
	void sendTrack(const QString& id, const QString& file);
	void socketError(QAbstractSocket::SocketError err);

signals:
	void disconnected();
	void requestReceived(const QString& id, const QString& request);

private:
	QTcpSocket* m_socket;
};

#endif /* WDCLIENT_H_ */
