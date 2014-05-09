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
: QObject(parent)
{
	// TODO Auto-generated constructor stub
	bb::system::InvokeManager *invokeManager = new bb::system::InvokeManager(this);
	bool ok = connect(invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
	                  this, SLOT(receivedInvokeTarget(const bb::system::InvokeRequest&)));
	Q_ASSERT(ok);
	Q_UNUSED(ok);
}

NfcReceiver::~NfcReceiver() {
	// TODO Auto-generated destructor stub
}

QString NfcReceiver::playlist_id() const
{
	return m_pID;
}

QString NfcReceiver::playlist_name() const
{
	return m_pName;
}

QString NfcReceiver::host_addr() const
{
	return m_hostAddr;
}

void  NfcReceiver::receivedInvokeTarget(const bb::system::InvokeRequest& request)
{
	if (request.action().compare("bb.action.OPEN") != 0) {
		return;
	}

	QByteArray data = request.data();
	qDebug() << "XXXXXXXX Received invoke request: " << data;

    QString reg = "\\[ \\{\"host_addr\" : \"(.*)\", \"playlist_id\" : \"(.*)\", \"playlist_name\" : \"(.*)\"\\} \\]";
    QRegExp rx(reg);
    int pos = rx.indexIn(tr("%1").arg(data.data()));
    QStringList list = rx.capturedTexts();

    m_hostAddr = list[1];
    m_pID = list[2];
    m_pName = list[3];

    qDebug() << "host " << m_hostAddr << " pid " << m_pID << " pname " << m_pName;

    /*
	QtMobilitySubset::QNdefMessage ndefMessage = QtMobilitySubset::QNdefMessage::fromByteArray(data);
	m_msgRecords->clear();

	for (int i = 0; i < ndefMessage.size(); ++i) {
		QtMobilitySubset::QNdefRecord record = ndefMessage.at(i);

		QVariantMap entry;
		entry["tnfType"] = record.typeNameFormat();
		entry["recordType"] = QString::fromLatin1(record.type());
		entry["payload"] = QString::fromLatin1(record.payload());
		entry["hexPayload"] = QString::fromLatin1(record.payload().toHex());

		m_msgRecords->append(entry);
		qDebug() << entry;
	}*/

	emit plistInfoReceived(m_pID, m_pName, m_hostAddr);
}
