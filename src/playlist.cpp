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

#include "playlist.h"
#include <bb/device/HardwareInfo>
#include <btapi/btdevice.h>

Playlist::Playlist()
: m_collection("jukebox_db"),
  m_IPAddr("127.0.0.1"), //TODO
  m_sessionName(""),
  tracks_(m_collection)
{
	// Get the device name from the bluetooth api... the correct hardwareinfo
	// call only returns "RIM-Blackberry Device"
	char buffer[128];
	bool ok = (bt_ldev_get_friendly_name(buffer, 128) == 0);
	m_devName = QString::fromLatin1(buffer);

    connect(&tracks_, SIGNAL(newObject(int)), this, SLOT(onItemAdded(int)));
    connect(&tracks_, SIGNAL(deletedObject(int)), this, SLOT(onItemDeleted(int)));

    if(false) {
    	m_wdhandler = new WDHandler();
    	m_wdhandler->addTarget("SEO-Z30");
    	m_wdhandler->scan();
    } else {
    	qDebug() << " I AM NOT THE HOST ";
    	m_wdlistener = new WDListener();
    	m_wdlistener->scan();
    }
}

Playlist::~Playlist() {
	// TODO delete from CloudList
}

void Playlist::onItemAdded(int index) {
	qDebug() << " ON ITEM ADDED ___";
	append(tracks_[index].m_metaData);
}

void Playlist::onItemDeleted(int index) {
	//
}

void Playlist::leave()
{
	int currentSize = size();

	for (int i = 0; i < currentSize; ++i) {
		for (int i = tracks_.size(); i >= 0; --i) {
			if(tracks_.at(i).m_ownerPhoneName == m_devName) {
				tracks_.remove(i);
			}
		}
	}
}

void Playlist::remove()
{
	int currentSize = size();

	for (int i = 0; i < currentSize; ++i) {
		QVariant item = value(i);
		QVariantMap itemMap = item.toMap();
		QString id = itemMap["track_id"].toString();
		// TODO
		// not sure what this does
		//deleteTrack(id);
	}
}

void Playlist::addMember()
{
	// TODO connect via wifi direct?
}

void Playlist::addTrack(const QUrl& path, const QVariantMap& md)
{
	tracks_.add(Track(m_pID, m_devName, path, md));
}

QVariant Playlist::pop()
{
	if (size() != 0) {
		QVariant item = tracks_.at(0).m_metaData;
		tracks_.remove(0);
		removeAt(0);
		return item;
	}

	QVariantMap null;
	null["uri"] = "";

	return QVariant(null);
}

void Playlist::setPID(const QString& id)
{
	if (m_pID != id) {
		m_pID = id;
		emit pIDChanged();
	}
}

void Playlist::setPName(const QString& name)
{
	if (m_pName != name) {
		m_pName = name;
		emit pNameChanged();
	}
}

void Playlist::setHostAddr(const QString& addr)
{
	if (m_hostAddr != addr) {
		m_hostAddr = addr;
		emit hostAddrChanged();
	}
}

void Playlist::loadTrack(int num)
{
	if (size() > 0) {
		if (num < size()) {
			if (m_hostAddr == m_devName) {
				QVariant item = value(num);
				QVariantMap itemMap = item.toMap();
				QString trackID = itemMap["track_id"].toString();
				QString trackOwner = itemMap["owner_addr"].toString();
				QString trackFilepath = itemMap["file_path"].toString();
				// TODO connect to host with wD?
			} else {
				// TODO connect to host with wD?
			}
		} else {
			qDebug() << "Track index out of bound";
		}
	} else {
		qDebug() << "Playlist is empty!";
	}
}

void Playlist::onTrackLoaded(const QString& id, const QString& path)
{
	qDebug() << "Track Loaded: (" << id << ", " << path << ")";
}
