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

#ifndef TRACK_H_
#define TRACK_H_

#include <bb/multimedia/MediaPlayer>
#include <bb/multimedia/MetaData>
#include <QUrl>

class Track {
	//Q_OBJECT
	//Q_PROPERTY(QVariantMap metaData READ metaData WRITE setMetaData NOTIFY metaDataChanged)
public:
	Track(const QString& pid, const QString& owner, const QUrl& file, const QVariantMap& md);
	//Track(const QString& pid, const QString& owner, const QUrl& file,
	//		const QString& title, const QString& artist, const QString& album);
	Track(const QString& id);
	void cbSetup();
	virtual ~Track();

	void insert();
	void remove();
	void fetchData();
	void uploadData();

	QString toString() const;

	QString m_ownerAddr;
	QString m_ownerPhoneName;
	QVariantMap m_metaData;

private:
	std::string m_collection;

	QString m_trackID;
	QString m_playlistID;
	QString m_filePath;

};

#endif /* TRACK_H_ */
