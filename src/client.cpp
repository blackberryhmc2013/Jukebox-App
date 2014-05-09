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

#include "client.h"
#include <QtNetwork/QHostAddress>
#include <QDir>
#include <QFile>

Client::Client(const QString& saveDir, QObject* parent)
: QObject(parent), m_socket(NULL), m_size(0), m_dataFolder(saveDir)
{
	//
}

Client::~Client()
{
	disconnect();

	if (m_socket != NULL) {
		m_socket->deleteLater();
	}
}

void Client::connectToHost(const QString& hostAddr, quint16 port)
{
	m_socket = new QTcpSocket(this);
	connect(m_socket, SIGNAL(connected()), this, SLOT(connected()));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SIGNAL(error(QAbstractSocket::SocketError)));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(disconnected()));


	qDebug() << "Client: Connecting to: " << hostAddr;
	QHostAddress host = QHostAddress(hostAddr);
	m_socket->connectToHost(host, port);
}

void Client::writeRequest(const QString& id, const QString& path, const QString& save)
{
	if (m_socket->state() == QTcpSocket::ConnectedState) {
		qDebug() << "Client: Sending Request";
		QByteArray buf;
		QDataStream out(&buf, QIODevice::WriteOnly);
		out.setVersion(QDataStream::Qt_4_8);

		out << (quint64) 0;
		out << id;
		out << path;
		out << save;
		out.device()->seek(0);
		out << (quint64)(buf.size() - sizeof(quint64));

		m_socket->write(buf);
		m_socket->flush();
	}
}

void Client::readMessage()
{
	if (m_socket->state() == QAbstractSocket::ConnectedState) {
		qDebug() << "Client: Reading Message";

		if (m_size == 0) {
			QDataStream in(m_socket);
			in.setVersion(QDataStream::Qt_4_8);
			quint64 blockSize = 0;

			if (m_socket->bytesAvailable() < (int)sizeof(quint64)) {
				return ;
			}

			in >> blockSize;

			if (m_socket->bytesAvailable() < blockSize) {
				m_size = blockSize;
				m_buffer = QByteArray();
				m_buffer.append(m_socket->read(m_socket->bytesAvailable()));
			} else {
				QString type;
				in >> type;
				qDebug() << "Client: Received Message Type: " << type;

				if (type == "Ready") {
					writeRequest(track_id, file_path, save_as);
				} else if (type == "Track") {
					qDebug() << "Client: Receiving Track";
					QString id;
					QString save;
					QByteArray bytes;
					in >> id;
					in >> save;
					in >> bytes;

					QFile file(m_dataFolder + save);

					if (!file.open(QIODevice::WriteOnly)) {
						qDebug() << "Client: File cannot be written!";
						return;
					} else {
						file.write(bytes);
						file.close();
						qDebug() << "Client: Received Track: (" << id << ", " << save << ")";
						emit trackReceived();
					}
				} else if (type == "Error") {
					qDebug() << "Client: Error Message Received";
				} else {
					qDebug() << "Client: Unknown Message Received";
				}
			}
		} else {
			m_buffer.append(m_socket->read(m_socket->bytesAvailable()));

			if (m_buffer.size() == m_size) {
				QDataStream in(&m_buffer, QIODevice::ReadOnly);
				QString type;
				in >> type;
				qDebug() << "Client: Received Message Type: " << type;

				if (type == "Ready") {
					writeRequest(track_id, file_path, save_as);
				} else if (type == "Track") {
					qDebug() << "Client: Receiving Track";
					QString id;
					QString save;
					QByteArray bytes;
					in >> id;
					in >> save;
					in >> bytes;

					QFile file(m_dataFolder + save);

					if (!file.open(QIODevice::WriteOnly)) {
						qDebug() << "Client: File cannot be written!";
						return;
					} else {
						file.write(bytes);
						file.close();
						qDebug() << "Client: Received Track: (" << id << ", " << save << ")";
						emit trackReceived();
					}
				} else if (type == "Error") {
					qDebug() << "Client: Error Message Received";
				} else {
					qDebug() << "Client: Unknown Message Received";
				}

				m_buffer.clear();
				m_size = 0;
			}
		}
	}
}

void Client::connected()
{
	qDebug() << "Client: Connected to host.";
	connect(m_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(readMessage()));
}

void Client::disconnected()
{
	qDebug() << "Client: Socket connection closed.";
	QObject::disconnect(m_socket, SIGNAL(disconnected()));
	QObject::disconnect(m_socket, SIGNAL(readyRead()));
	QObject::disconnect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)));
	m_socket->deleteLater();
}

void Client::disconnect()
{
	if (m_socket != NULL) {
		if (m_socket->state() == QTcpSocket::ConnectedState) {
			qDebug() << "Client: Disconnecting socket from host..";
			m_socket->disconnectFromHost();
		}
	}
}
