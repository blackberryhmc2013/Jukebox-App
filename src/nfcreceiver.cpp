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

#include "nfcreceiver.h"
#include <bb/system/InvokeManager>
#include <QtNfcSubset/QNdefMessage>
#include <QtNfcSubset/QNdefRecord>
#include <QRegExp>

NfcReceiver::NfcReceiver(QObject* parent)
: QObject(parent), m_isJukebox(false)
{
	bb::system::InvokeManager* invokeManager = new bb::system::InvokeManager(this);
	bool ok = connect(invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
			this, SLOT(receivedInvokeTarget(const bb::system::InvokeRequest&)));
	Q_ASSERT(ok);
	Q_UNUSED(ok);
}

NfcReceiver::~NfcReceiver()
{
	// Nothing to do
}

void  NfcReceiver::receivedInvokeTarget(const bb::system::InvokeRequest& request)
{
	QByteArray data = request.data();
	qDebug() << "NfcReceiver: Received invoke request: " << data;

    QString reg = "\\[ \\{\"is_jukebox\" : \"(.*)\", \"playlist_id\" : \"(.*)\", \"playlist_name\" : \"(.*)\"\\} \\]";
    QRegExp rx(reg);
    int pos = rx.indexIn(tr("%1").arg(data.data()));
    QStringList list = rx.capturedTexts();

    m_isJukebox = (list[1] == "true" ? true : false);
    m_pID = list[2];
    m_pName = list[3];

    qDebug() << m_isJukebox;
    qDebug() << m_pID;
    qDebug() << m_pName;

	emit plistInfoReceived(m_pID, m_pName, m_isJukebox);
}
