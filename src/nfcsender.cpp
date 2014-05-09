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

#include "nfcsender.h"
#include <QDebug>

NfcSender::NfcSender(QObject* parent)
: QObject(parent), m_active(false),
  m_nfcShareManager(new bb::system::NfcShareManager()), m_dataType("jukebox/plistinfo")
{
	// TODO Auto-generated constructor stub
    QObject::connect(m_nfcShareManager, SIGNAL(finished(bb::system::NfcShareSuccess::Type)),
    				this, SLOT(finished(bb::system::NfcShareSuccess::Type)));
    QObject::connect(m_nfcShareManager, SIGNAL(error(bb::system::NfcShareError::Type)),
    				this, SLOT(error(bb::system::NfcShareError::Type)));

    QObject::connect(this, SIGNAL(playlistInfoChanged()), this, SLOT(onContentChanged()));
    QObject::connect(this, SIGNAL(dataTypeChanged()), this, SLOT(onContentChanged()));

    m_nfcShareManager->setShareMode(bb::system::NfcShareMode::Disabled);
}

NfcSender::~NfcSender() {
	// TODO Auto-generated destructor stub
}

void NfcSender::contentChanged(const QString& message, const QString& dataType) {
	qDebug() << "XXXX shareContentChanged message : " << message;
	qDebug() << "XXXX shareContentChanged dataType: " << dataType;

}

void NfcSender::setActive(bool stat)
{
	if (m_active != stat) {
		m_active = stat;

		if (m_active) {
			m_nfcShareManager->setShareMode(bb::system::NfcShareMode::Data);
		} else {
			m_nfcShareManager->setShareMode(bb::system::NfcShareMode::Disabled);
		}

		emit activeChanged();
	}
}

bool NfcSender::active() const
{
	return m_active;
}

void NfcSender::setPlaylistInfo(const QString& map)
{
	if (m_plistInfo != map) {
		m_plistInfo = map;
		bb::system::NfcShareDataContent request;
		QByteArray data(m_plistInfo.toUtf8());
		QUrl url;
		request.setUri(url);
		request.setMimeType(m_dataType);
		request.setData(data);
		bb::system::NfcShareSetContentError::Type rc = m_nfcShareManager->setShareContent(request);
		qDebug() << "XXXX shareContentChanged rc =" << rc;
		emit playlistInfoChanged();
	}
}

QString NfcSender::playlistInfo() const
{
	return m_plistInfo;
}

void NfcSender::setDataType(const QString& type)
{
	if (m_dataType != type) {
		m_dataType = type;
		bb::system::NfcShareDataContent request;
		QByteArray data(m_plistInfo.toUtf8());
		QUrl url;
		request.setUri(url);
		request.setMimeType(m_dataType);
		request.setData(data);
		bb::system::NfcShareSetContentError::Type rc = m_nfcShareManager->setShareContent(request);
		qDebug() << "XXXX shareContentChanged rc =" << rc;
		emit dataTypeChanged();
	}
}

QString NfcSender::dataType() const
{
	return m_dataType;
}

void NfcSender::finished(bb::system::NfcShareSuccess::Type result)
{
    qDebug() << "XXXX finished result =" << result;
}

void NfcSender::error(bb::system::NfcShareError::Type error)
{
    qDebug() << "XXXX error error =" << error;
}

void NfcSender::onContentChanged()
{
	bb::system::NfcShareDataContent request;
	QByteArray data(m_plistInfo.toUtf8());
	QUrl url;
	request.setUri(url);
	request.setMimeType(m_dataType);
	request.setData(data);
	bb::system::NfcShareSetContentError::Type rc = m_nfcShareManager->setShareContent(request);
	qDebug() << "XXXX shareContentChanged rc =" << rc;
}
