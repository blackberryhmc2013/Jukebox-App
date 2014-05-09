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

#ifndef SERVER_H_
#define SERVER_H_

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

class Server : public QObject {
	Q_OBJECT
public:
	Server(QObject* parent = 0);
	virtual ~Server();

	void listen(quint16 port);
	void disconnect();

private slots:
	void newConnection();
	void disconnected();
	void receiveRequest();
	void sendTrack(const QString& id, const QString& path, const QString& save);

signals:
	void error(QAbstractSocket::SocketError);
	void requestReceived(const QString& id, const QString& path, const QString& save);

private:
	void sendReady();

	QTcpServer* m_server;
	QTcpSocket* m_socket;

};

#endif /* SERVER_H_ */
