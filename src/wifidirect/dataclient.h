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

#ifndef CLIENT_H_
#define CLIENT_H_

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>

class Client : public QObject {
	Q_OBJECT
public:
	Client(QObject* parent = 0);
	virtual ~Client();

	void connectToHost(const QString& hostAddr);
	void disconnect();

public slots:
	void writeRequest(const QString& req);

private slots:
	void connected();
	void disconnected();
	void readResponse();
	void socketError(QAbstractSocket::SocketError theError);

signals:
	void responseRead(const QString& resp);

private:
	QTcpSocket* m_socket;
};

#endif /* CLIENT_H_ */
