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

#include "dataclient.h"
#include <QtNetwork/QHostAddress>

Client::Client(QObject *parent)
: QObject(parent), m_socket(new QTcpSocket(this))
{
	connect(m_socket, SIGNAL(connected()), this, SLOT(connected()));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
}

Client::~Client()
{
	disconnect();

	if (m_socket != NULL) {
		m_socket->deleteLater();
	}
}

void Client::connectToHost(const QString& hostAddr)
{
	qDebug() << "Client: Connecting to: " << hostAddr;
	QHostAddress host = QHostAddress(hostAddr);
	m_socket->connectToHost(host, 4554);
}

void Client::writeRequest(const QString& req)
{
	if (m_socket->state() == QTcpSocket::ConnectedState) {
		qDebug() << "Client: Sending Request";
		QByteArray buf;
		QDataStream out(&buf, QIODevice::WriteOnly);
		out.setVersion(QDataStream::Qt_4_8);

		out << (quint64) 0;
		out << req;
		out.device()->seek(0);
		out << (quint64)(buf.size() - sizeof(quint64));

		m_socket->write(buf);
		m_socket->flush();
	}
}

void Client::readResponse()
{
	qDebug() << "Client: Reading Response";

	QDataStream in(m_socket);
	in.setVersion(QDataStream::Qt_4_8);
	quint64 blockSize = 0;

	if (blockSize == 0) {
		if (m_socket->bytesAvailable() < (int)sizeof(quint64)) {
			return ;
		}

		in >> blockSize;
	}

	if (m_socket->bytesAvailable() < blockSize) {
		return;
	}

	QString resp;
	in >> resp;
	qDebug() << "Client: Resp: " << resp;

	emit responseRead(resp);
}

void Client::connected()
{
	qDebug() << "Client: Connected to host.";
	connect(m_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(readResponse()));
}

void Client::disconnected()
{
	qDebug() << "Client: Socket connection closed.";
	QObject::disconnect(m_socket, SIGNAL(disconnected()));
	QObject::disconnect(m_socket, SIGNAL(readyRead()), this, SLOT(readResponse()));
	QObject::disconnect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
	m_socket->deleteLater();
}

void Client::socketError(QAbstractSocket::SocketError theError)
{
	qDebug() << "Client: Socket Error! " << theError;
	disconnected();
}

void Client::disconnect()
{
	if(m_socket->state() == QTcpSocket::ConnectedState) {
		m_socket->disconnectFromHost();
	}
}
