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

#include "server.h"
#include <QDir>
#include <QFile>

Server::Server(QObject* parent)
: QObject(parent), m_server(NULL), m_socket(NULL)
{
	//
}

Server::~Server()
{
	disconnect();

	if (m_socket != NULL) {
		delete m_socket;
		m_socket = NULL;
	}

	if (m_server != NULL) {
		m_server->deleteLater();
	}
}

void Server::listen(quint16 port)
{
	m_server = new QTcpServer(this);
	connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
	//connect(m_server, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SIGNAL(error(QAbstractSocket::SocketError)));
	connect(this, SIGNAL(requestReceived(const QString, const QString, const QString)),
			this, SLOT(sendTrack(const QString, const QString, const QString)));

	qDebug() << "Server: Starting to listen...";
	bool result = m_server->listen(QHostAddress::Any, port);

	if (result) {
		qDebug() << "Server: Listening...";
	} else {
		qDebug() << "Server: Unable to listen";
	}
}

void Server::newConnection()
{
	qDebug() << "Server: Received connection from the client.";
	m_socket = m_server->nextPendingConnection();

	connect(m_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(receiveRequest()));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(error(QAbstractSocket::SocketError)));
	sendReady();
}

void Server::sendReady()
{
	if (m_socket->state() == QTcpSocket::ConnectedState) {
		QByteArray buf;
		QDataStream out(&buf, QIODevice::WriteOnly);
		out.setVersion(QDataStream::Qt_4_8);

		QString ready = "Ready";
		out << (quint64) 0;
		out << ready;
		out.device()->seek(0);
		out << (quint64)(buf.size() - sizeof(quint64));
		qDebug() << buf;
		qDebug() << buf.size();

		m_socket->write(buf);
		m_socket->flush();
	}
}

void Server::receiveRequest()
{
	qDebug() << "Server: Reading Request";
	QDataStream in(m_socket);
	in.setVersion(QDataStream::Qt_4_8);
	quint64 blockSize = 0;

	if (blockSize == 0) {
		if (m_socket->bytesAvailable() < (int)sizeof(quint64)) {
			return;
		}

		in >> blockSize;
	}

	if (m_socket->bytesAvailable() < blockSize) {
		return;
	}

	QString id;
	QString path;
	QString save;
	in >> id;
	in >> path;
	in >> save;

	qDebug() << "Server: Received Request: (" << id << ", " << path << ", " << save << ")";
	emit requestReceived(id, path, save);
}

void Server::sendTrack(const QString& id, const QString& path, const QString& save)
{
	if (m_socket->state() == QTcpSocket::ConnectedState) {
		QByteArray buf;
		QDataStream out(&buf, QIODevice::WriteOnly);
		out.setVersion(QDataStream::Qt_4_8);
		out << (quint64) 0;

		QFile file(path);

		if (!file.open(QIODevice::ReadOnly)) {
			qDebug() << "Server: File cannot be opened!";
			QString err = "Error";
			out << err;
		} else {
			QString track = "Track";
			out << track;
			QByteArray bytes = file.readAll();

			out << id;
			out << save;
			out << bytes;
			file.close();
		}

		out.device()->seek(0);
		out << (quint64)(buf.size() - sizeof(quint64));
		m_socket->write(buf);
		m_socket->flush();
	}
}

void Server::disconnected()
{
	qDebug() << "Server: Socket connection closed.";
	QObject::disconnect(m_socket, SIGNAL(disconnected()));
	QObject::disconnect(m_socket, SIGNAL(readyRead()), this, SLOT(receiveRequest()));
	QObject::disconnect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(error(QAbstractSocket::SocketError)));
	m_socket->deleteLater();
}


void Server::disconnect()
{
	if (m_socket != NULL) {
		if (m_socket->state() == QTcpSocket::ConnectedState) {
			qDebug() << "Server: Disconnecting socket from host..";
			m_socket->disconnectFromHost();
		}
	}

	if (m_server != NULL) {
		qDebug() << "Server: Closing the server..";
		m_server->close();
	}
}
