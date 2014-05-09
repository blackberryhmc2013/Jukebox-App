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

#ifndef NFCRECEIVER_H_
#define NFCRECEIVER_H_

#include <bb/cascades/QListDataModel>
#include <bb/system/InvokeRequest>

class NfcReceiver : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString playlist_id READ playlist_id CONSTANT)
    Q_PROPERTY(QString playlist_name READ playlist_name CONSTANT)
    Q_PROPERTY(QString host_addr READ host_addr CONSTANT)

public:
	NfcReceiver(QObject* parent = 0);
	virtual ~NfcReceiver();

	QString playlist_id() const;
	QString playlist_name() const;
	QString host_addr() const;

public slots:
	void receivedInvokeTarget(const bb::system::InvokeRequest& request);

signals:
	void plistInfoReceived(const QString& id, const QString& name, const QString& host);

private:
	QString m_pID;
	QString m_pName;
	QString m_hostAddr;
};

#endif /* NFCRECEIVER_H_ */
