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
#include <QUuid>

Track::Track(const QString& pid, const QString& owner, const QUrl& file, const QVariantMap& md)
: m_playlistID(pid.toStdString()), m_removed(false),
  m_ownerAddr(owner.toStdString()),  m_filePath(file.toString().toStdString()),
  m_saveAs (file.toString().toStdString()), m_metaData(md)
{
	QUuid tid = QUuid::createUuid();
	m_trackID = tid.toString().toStdString();

	std::string pathStr = m_filePath;
	QString path = QString(pathStr.c_str());
	QStringList parsedPath = path.split("/", QString::SkipEmptyParts);

	if (parsedPath.size() > 0) {
		QString name = *(--parsedPath.end());
		QStringList parsedName = name.split(".", QString::SkipEmptyParts);

		if (parsedName.size() > 0) {
			QString ext = *(--parsedName.end());
			m_saveAs = m_trackID + "." + ext.toStdString();
		}
	}
}

Track::Track(const QString& tid, bool removed)
: m_trackID(tid.toStdString()), m_removed(removed)
{
	// Nothing else to do
}

Track::~Track()
{
	// Nothing to do
}

std::string Track::serialize()
{
	std::string out = "[ {";

	out += "\"track_id\" : \"" + m_trackID + "\", ";
	out += "\"playlist_id\" : \"" + m_playlistID + "\", ";
	out += "\"removed\" : \"";
	out += (m_removed ? "true" : "false");
	out += "\", ";
	out += "\"owner_addr\" : \"" + m_ownerAddr + "\", ";
	out += "\"file_path\" : \"" + m_filePath + "\", ";
	out += "\"save_as\" : \"" + m_saveAs;

	if (m_metaData.size() == 0) {
		out += "\" } ]";
	} else {
		out += "\", ";

		for (int i = 0; i < m_metaData.size(); ++i) {
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

			if (i == m_metaData.size()-1) {
				out += "\"" + key.toStdString() + "\" : \"" + val.toStdString() + "\" } ]";
			} else {
				out += "\"" + key.toStdString() + "\" : \"" + val.toStdString() + "\", ";
			}
		}
	}

	return out;
}

