/* Copyright (c) 2012 Research In Motion Limited.
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

#ifndef SimplePushServer_HPP_
#define SimplePushServer_HPP_

#include <QObject>
#include <qt4/QtNetwork/QNetworkAccessManager>
#include <qt4/QtNetwork/QNetworkRequest>

#include <bb/cascades/TextArea>
#include <bb/cascades/Page>

namespace bb {
namespace cascades {
class Application;
}
}

using namespace bb::cascades;

class SimplePushServer: public QObject {
Q_OBJECT
public:
	SimplePushServer();
	virtual ~SimplePushServer() {
	}


	//Q_INVOKABLE void pushMessageToUser(const QString &userPin,
	//		const int priority, const QString &title, const QString &body);
	Q_INVOKABLE void pushMessageToUserList(const QString &id, const QString &operation, const QString &values, const qulonglong &stamp);
	Q_INVOKABLE void requestSubscribedUserList();

private:
	static const QString BLACKBERRY_PUSH_APPLICATION_ID;
	static const QString BLACKBERRY_PUSH_PASSWORD;
	static const QString BLACKBERRY_PUSH_URL;
	static const QString BLACKBERRY_PUSH_BOUNDARY;

	QNetworkAccessManager networkAccessManager;

	QString readFile(const QString& fileName);
	void pushMessageToSpecifiedUsers(
			const QString &papFormattedAddresses,
			const QString &id, const QString &operation, const QString &values, const qulonglong &stamp);
	QNetworkRequest generateBasicNetworkRequest(const QString & urlSuffix);
	void generatePushMessagePostData(QString& templateFileData,
			const QString &address, const QString &pushData);
	void generateSubscriptionQueryPostData(QString& templateFileData);
	void populateAddresses(QString& addressList);
	void generatePushMessage(QString& pushMessage, const QString &id, const QString &operation, const QString &values, const qulonglong &stamp);
	void papFormatAddress(QString &address);
	void log(const QString& message);

	bool updatingPins;

private slots:
	void pushMessageResponse(QNetworkReply* reply);
	void subscriptionQueryResponse(QNetworkReply* reply);


};

#endif /* SimplePushServer_HPP_ */
