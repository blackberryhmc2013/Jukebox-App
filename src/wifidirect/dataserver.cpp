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

#include "dataserver.h"

Server::Server(QObject* parent)
: QObject(parent), m_server(new QTcpServer(this))
{
	connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
	connect(this, SIGNAL(requestRead(const QString)), this, SLOT(writeResponse(const QString)));
}

Server::~Server()
{
	disconnect();

	if (m_server != NULL) {
		m_server->deleteLater();
	}
}

void Server::listen()
{
	qDebug() << "Starting to listen...";
	m_server->listen(QHostAddress::Any, 14554);
	qDebug() << "Listening...";
}

void Server::newConnection()
{
	qDebug() << "Received connection from operator.";
	m_sockets.append(m_server->nextPendingConnection());

	//connect(m_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	//connect(m_socket, SIGNAL(readyRead()), this, SLOT(readRequest()));
	//connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
}

void Server::readRequest()
{
//	qDebug() << "Server: Reading Request";
//	QDataStream in(m_socket);
//	in.setVersion(QDataStream::Qt_4_8);
//	quint64 blockSize = 0;
//
//	if (blockSize == 0) {
//		if (m_socket->bytesAvailable() < (int)sizeof(quint64)) {
//			return;
//		}
//
//		in >> blockSize;
//	}
//
//	if (m_socket->bytesAvailable() < blockSize) {
//		return;
//	}
//
//	QString req;
//	in >> req;
//	qDebug() << "Server: Request: " << req;
//
//	emit requestRead(req);
}

void Server::writeResponse(const QString& resp)
{
//	if (m_socket->state() == QTcpSocket::ConnectedState) {
//		QByteArray buf;
//		QDataStream out(&buf, QIODevice::WriteOnly);
//		out.setVersion(QDataStream::Qt_4_8);
//
//		out << (quint64) 0;
//		out << resp;
//		out.device()->seek(0);
//		out << (quint64)(buf.size() - sizeof(quint64));
//		qDebug() << buf;
//		qDebug() << buf.size();
//
//		m_socket->write(buf);
//		m_socket->flush();
//	}
}

void Server::disconnected()
{
	qDebug() << "Server:  Socket connection closed.";
//	QObject::disconnect(m_socket, SIGNAL(disconnected()));
//	QObject::disconnect(m_socket, SIGNAL(readyRead()), this, SLOT(readRequest()));
//	QObject::disconnect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
//	m_socket->deleteLater();
}


void Server::disconnect()
{
//	if (m_socket->state() == QTcpSocket::ConnectedState) {
//		m_socket->disconnectFromHost();
//	}
//
//	m_server->close();
}

void Server::socketError(QAbstractSocket::SocketError theError)
{
	qDebug() << "Socket Error! " << theError;
}
