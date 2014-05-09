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

#ifndef NFCSENDER_H_
#define NFCSENDER_H_

#include <bb/system/NfcShareManager>

class NfcSender : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString playlistInfo WRITE setPlaylistInfo NOTIFY playlistInfoChanged)
	Q_PROPERTY(QString dataType READ dataType WRITE setDataType NOTIFY dataTypeChanged)
	Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

public:
	NfcSender(QObject* parent = 0);
	virtual ~NfcSender();

	bool active() const;
	void setActive(bool stat);

	QString playlistInfo() const;
	void setPlaylistInfo(const QString& map);

	QString dataType() const;
	void setDataType(const QString& type);

public slots:
	void contentChanged(const QString& message, const QString& dataType);
    void finished(bb::system::NfcShareSuccess::Type result);
    void error(bb::system::NfcShareError::Type error);
    void onContentChanged();

signals:
	void activeChanged();
	void playlistInfoChanged();
	void dataTypeChanged();

private:
	QString m_plistInfo;
	QString m_dataType;
	bool m_active;
    bb::system::NfcShareManager* m_nfcShareManager;
};

#endif /* NFCSENDER_H_ */
