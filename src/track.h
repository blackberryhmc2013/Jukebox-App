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

#include "CBHelper.h"

using namespace Cloudbase;

class Track : public Cloudbase::CBSerializable {
public:
	Track(const QString& pid, const QString& owner, const QUrl& file, const QVariantMap& md);
	Track(const QString& tid, bool removed);
	virtual ~Track();

	std::string serialize();

private:
	std::string m_trackID;
	std::string m_playlistID;
	bool m_removed;
	std::string m_ownerAddr;
	std::string m_filePath;
	std::string m_saveAs;
	QVariantMap m_metaData;
};

#endif /* TRACK_H_ */
