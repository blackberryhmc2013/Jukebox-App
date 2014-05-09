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

#include "wdserver.h"
#include <QFile>
#include <QByteArray>
#include <QDir>

WDServer::WDServer(QObject* parent)
: QObject(parent), m_server(new QTcpServer(this))
{
	connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

WDServer::~WDServer() {
	// TODO Auto-generated destructor stub
}

void WDServer::listen()
{
	qDebug() << "Starting to listen...";
	m_server->listen(QHostAddress::Any, 4554); // TODO port
	qDebug() << "Listening...";
}

void WDServer::newConnection()
{
	qDebug() << "Received connection from the client.";
	m_socket = m_server->nextPendingConnection();
	connect(m_socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(receiveTrack()));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
}

void WDServer::sendRequest(const QString& id, const QString& path)
{
	if (m_socket->state() == QTcpSocket::ConnectedState) {
	    QByteArray buf;
	    QDataStream out(&buf, QIODevice::WriteOnly);
	    out.setVersion(QDataStream::Qt_4_2);
	    //out.setVersion( out.version() ); // set to the current Qt version
	    //out << (quint16) 0;
	    out << (quint32) 0;
	    out << id;
	    out << path;
	    out.device()->seek(0);
	    out << (quint32)(buf.size() - sizeof(quint32));
	    //out << (quint16)(block.size() - sizeof(quint16));

		m_socket->write(buf);
		m_socket->flush();


		//m_socket->write(id.toLatin1()); // TODO
		//m_socket->write(path.toLatin1());
		//m_socket->flush();
	}
}

void WDServer::receiveTrack()
{
	qDebug() << "Received File!";
	QString filename = QDir::currentPath() + "/shared/downloads/myImage.jpg"; // TODO
	QFile file(filename);

	if (!file.open(QIODevice::Append)) {
		qDebug() << "Server Error: File cannot be opened";
		return;
	} else {
		qDebug() << "File created: " + filename;

		QDataStream in(m_socket);
		in.setVersion(QDataStream::Qt_4_2);
		quint32 blockSize = 0;

		if (blockSize == 0) {
			if (m_socket->bytesAvailable() < (int)sizeof(quint32)) {
				return ;
			}

			in >> blockSize;
		}

		if (m_socket->bytesAvailable() < blockSize) {
			return;
		}

		QString id;
		QByteArray bytes;
		in >> id;
		in >> bytes;

		//QByteArray read = m_socket->read(m_socket->bytesAvailable());
		//qDebug() << "Read: " + read.size();
		file.write(bytes);
		file.close(); // TODO
		emit trackReceived(id, filename);
	}
}

void WDServer::onDisconnected()
{
	emit disconnected();
	qDebug() << "Server: Socket connection closed.";
	QObject::disconnect(m_socket, SIGNAL(disconnected()));
	QObject::disconnect(m_socket, SIGNAL(readyRead()));
	QObject::disconnect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
	m_socket->deleteLater();
}

void WDServer::disconnect()
{
	if (m_socket->state() == QTcpSocket::ConnectedState) {
		m_socket->disconnectFromHost();
	}

	m_server->close();
}

void WDServer::socketError(QAbstractSocket::SocketError err)
{
	qDebug() << "Socket Error! " << err;
}

