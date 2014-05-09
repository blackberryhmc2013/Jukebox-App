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

#include "wdclient.h"
#include <QFile>
#include <QDir>


WDClient::WDClient(QObject* parent)
: QObject(parent), m_socket(new QTcpSocket(this))
{
	// TODO Auto-generated constructor stub
	connect(m_socket, SIGNAL(connected()), this, SLOT(connected()));
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(receiveRequest()));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
	connect(this, SIGNAL(requestReceived(QString, QString)), this, SLOT(sendTrack(QString, QString)));
}

WDClient::~WDClient() {
	// TODO Auto-generated destructor stub
}

void WDClient::connectToHost(const QString& serverAddr)
{
	qDebug() << "Connecting to: " << serverAddr;
	QHostAddress hostAddr = QHostAddress(serverAddr);
	m_socket->connectToHost(hostAddr, 4554); // TODO port
}

void WDClient::receiveRequest()
{
	QDataStream in(m_socket);
	in.setVersion(QDataStream::Qt_4_2);
	quint32 blockSize = 0;
	//in.setVersion( in.version() ); // set to the current Qt version instead

	if (blockSize == 0) {
		if (m_socket->bytesAvailable() < (int)sizeof(quint32)) {
			return;
		}

		in >> blockSize;
	}

	if (m_socket->bytesAvailable() < blockSize) {
		return;
	}

	QString id;
	QString path;
	in >> id;
	in >> path;

	emit requestReceived(id, path);

/*
	while (m_socket->canReadLine())
	{
		QByteArray requestByte = m_socket->readLine();
		QString request = QString(requestByte);
		qDebug() << "Received request: " << request;
		emit requestReceived("", request); // TODO
	}*/
}

void WDClient::sendTrack(const QString& id, const QString& path)
{
	if (m_socket->state() == QTcpSocket::ConnectedState) {
		QString filename = QDir::currentPath() + "/app/native/assets/images/receipt.jpg";
		QFile file(filename);

		if (!file.open(QIODevice::ReadOnly)) {
			qDebug() << "Client Error: File cannot be opened!";
			return;
		}

		QByteArray bytes = file.readAll();
		file.close();

	    QByteArray buf;
	    QDataStream out(&buf, QIODevice::WriteOnly);
	    out.setVersion(QDataStream::Qt_4_2);
	    //out.setVersion( out.version() ); // set to the current Qt version
	    //out << (quint16) 0;
	    out << (quint32) 0;
	    out << id;
	    out << bytes;
	    out.device()->seek(0);
	    out << (quint32)(buf.size() - sizeof(quint32));
	    //out << (quint16)(block.size() - sizeof(quint16));

		m_socket->write(buf);
		m_socket->flush();
	}

}

void WDClient::connected()
{
	qDebug() << "Connected to server.";
	connect(m_socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

void WDClient::onDisconnected()
{
	emit disconnected();
	qDebug() << "Client: Socket connection closed.";
	QObject::disconnect(m_socket, SIGNAL(disconnected()));
	QObject::disconnect(m_socket, SIGNAL(readyRead()));
	QObject::disconnect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
	m_socket->deleteLater();
}

void WDClient::disconnect()
{
	if (m_socket->state() == QTcpSocket::ConnectedState) {
		m_socket->disconnectFromHost();
	}
}

void WDClient::socketError(QAbstractSocket::SocketError err)
{
	qDebug() << "Socket Error! " << err;
	onDisconnected();
}

