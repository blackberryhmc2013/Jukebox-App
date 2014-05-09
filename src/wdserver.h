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

#ifndef WDSERVER_H_
#define WDSERVER_H_

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <cstdio>


class WDServer : public QObject {
	Q_OBJECT
public:
	WDServer(QObject* parent = 0);
	virtual ~WDServer();

public slots:
	void newConnection();
	void listen();
	void sendRequest(const QString& id, const QString& path);
	void receiveTrack();
	void onDisconnected();
	void disconnect();
	void socketError(QAbstractSocket::SocketError err);

signals:
	void disconnected();
	void trackReceived(const QString& id, const QString& path);

private:
	QTcpServer* m_server;
	QTcpSocket* m_socket;
};

#endif /* WDSERVER_H_ */
