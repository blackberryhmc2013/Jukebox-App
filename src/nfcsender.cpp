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
  m_nfcManager(new bb::system::NfcShareManager())
{
	QObject::connect(m_nfcManager, SIGNAL(finished(bb::system::NfcShareSuccess::Type)),
	    				this, SLOT(finished(bb::system::NfcShareSuccess::Type)));
	QObject::connect(m_nfcManager, SIGNAL(error(bb::system::NfcShareError::Type)),
					this, SLOT(error(bb::system::NfcShareError::Type)));

	m_nfcManager->setShareMode(bb::system::NfcShareMode::Disabled);
}

NfcSender::~NfcSender()
{
	m_nfcManager->deleteLater();
}

void NfcSender::setActive(bool stat)
{
	if (m_active != stat) {
		m_active = stat;

		if (m_active) {
			m_nfcManager->setShareMode(bb::system::NfcShareMode::Data);
		} else {
			m_nfcManager->setShareMode(bb::system::NfcShareMode::Disabled);
		}

		emit activeChanged();
	}
}

void NfcSender::setPlaylistInfo(const QString& map)
{
	if (m_plistInfo != map) {
		m_plistInfo = map;
		bb::system::NfcShareDataContent request;
		QByteArray data(m_plistInfo.toUtf8());
		QUrl url;
		request.setUri(url);
		request.setMimeType("jukebox/playlist");
		request.setData(data);
		bb::system::NfcShareSetContentError::Type rc = m_nfcManager->setShareContent(request);
		emit playlistInfoChanged();
	}
}

void NfcSender::finished(bb::system::NfcShareSuccess::Type result)
{
    qDebug() << "NfcSender: Finished result =" << result;
}

void NfcSender::error(bb::system::NfcShareError::Type error)
{
    qDebug() << "NfcSender: Error =" << error;
}

