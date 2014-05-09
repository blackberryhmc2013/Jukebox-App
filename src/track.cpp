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

#include "track.h"
#include <bb/data/JsonDataAccess>
#include <QMap>
#include <btapi/btdevice.h>

Track::Track(const QString& pid, const QString& devName, const QUrl& file,
		const QVariantMap& md)
: m_playlistID(pid),
  m_ownerPhoneName(devName),
  m_filePath(file.toString()),
  m_metaData(md)
{

}

Track::Track(const QString& strRepr)
{
	bb::data::JsonDataAccess jda;
	QVariant data = jda.loadFromBuffer(strRepr);
	qDebug() << data;
	QMap<QString,QVariant> dataMap = data.toMap();
	m_filePath = dataMap["file_path"].toString();
	m_ownerAddr = dataMap["owner_addr"].toString();
	m_playlistID = dataMap["playlist_id"].toString();

	// Load in all the metadata
	m_metaData.swap(dataMap);
}

Track::~Track()
{
	// TODO Auto-generated destructor stub
}


QString Track::toString() const
{
	QString out = "{";

	out += "\"track_id\" : \"" + m_trackID + "\", ";
	out += "\"playlist_id\" : \"" + m_playlistID + "\", ";
	out += "\"owner_addr\" : \"" + m_ownerAddr + "\", ";
	out += "\"file_path\" : \"" + m_filePath + "\" ";

	for (int i = 0; i < m_metaData.size(); ++i) {
		out += ", ";
		QString key = m_metaData.keys().at(i);
		QString val = m_metaData[key].toString();
		if (key == "duration") {
			int ms = 0;

			for (int i = 0; i < val.size(); ++i) {
				QChar current = val[i];
				int currentToInt = (current.isDigit()? current.digitValue() : 0);
				ms += currentToInt*pow(10, val.size()-1-i);
			}

			int hr = ms/1000/60/60;
			int min = ms/1000/60%60;
			int sec = ms/1000%60;

			QString hrStr = QString::number(hr);
			QString minStr = QString::number(min);
			QString secStr = QString::number(sec);

			val = QString("%1:%2:%3").arg(hr < 10 ? "0" + hrStr : "" + hrStr)
								.arg(min < 10 ? "0" + minStr : "" + minStr)
								.arg(sec < 10 ? "0" + secStr : "" + secStr);
		}
		out += "\"" + key + "\" : \"" + val + "\"";
	}
	out += "}";
	return out;
}
