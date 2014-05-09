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
	Client(const QString& saveDir, QObject* parent = 0);
	virtual ~Client();

	void connectToHost(const QString& hostAddr, quint16 port);
	void disconnect();

	QString track_id;
	QString file_path;
	QString save_as;

public slots:
	void writeRequest(const QString& id, const QString& path, const QString& save);

private slots:
	void connected();
	void disconnected();
	void readMessage();

signals:
	void error(QAbstractSocket::SocketError err);
	void trackReceived();

private:
	QTcpSocket* m_socket;

	quint64 m_size;
	QByteArray m_buffer;

	QString m_dataFolder;
};

#endif /* CLIENT_H_ */
