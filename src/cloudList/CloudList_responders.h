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
#ifndef CLOUDLIST_RESPONDERS_H_
#define CLOUDLIST_RESPONDERS_H_

#include <QMap>

/* RESPONDERS */
template <>  // seems wrong to make this inline but it makes it work for now
inline void CloudList<QString>::InsertResponder::parseResponse(Cloudbase::CBHelperResponseInfo resp) {
	bb::data::JsonDataAccess jda;
	QVariant data = jda.loadFromBuffer(QString(resp.outputString.c_str()));

	ele_ -> dbId_ = data.toMap()["data"].toMap()["message"].toMap()["cb_id"].toString();
	pushServer_ -> pushMessageToUserList(ele_->dbId_, "add", ele_->data_, ele_->stamp_.toMSecsSinceEpoch());
	delete this;	// is this a good idea...?
}

template <typename T>
void CloudList<T>::InsertResponder::parseResponse(Cloudbase::CBHelperResponseInfo resp) {
	bb::data::JsonDataAccess jda;
	qDebug() << "RESP: " << resp.outputString.c_str();

	QVariant data = jda.loadFromBuffer(QString(resp.outputString.c_str()));
	ele_ -> dbId_ = data.toMap()["data"].toMap()["message"].toMap()["cb_id"].toString();
	pushServer_ -> pushMessageToUserList(ele_->dbId_, "add" , ele_->data_.toString(), ele_->stamp_.toMSecsSinceEpoch());
	delete this;	// is this a good idea...?
}

template <typename T>
void CloudList<T>::UpdateResponder::parseResponse(Cloudbase::CBHelperResponseInfo resp) {
	bb::data::JsonDataAccess jda;
	QVariant data = jda.loadFromBuffer(QString(resp.outputString.c_str()));
	qDebug() << "updated: " << data;
	delete this;	// is this a good idea...?
}

template <typename T>
void CloudList<T>::PullResponder::parseResponse(Cloudbase::CBHelperResponseInfo resp) {
	typename Container::iterator it = parent->elements.begin();
	for(; it < parent->elements.end(); ++it) {
		it -> deleted_ = true;
	}

	qDebug() << "pull response ---- ";
	//qDebug() << resp.outputString.c_str();
	bb::data::JsonDataAccess jda;
	QVariant data = jda.loadFromBuffer(QString(resp.outputString.c_str()));
	//ele_.dbId_ = data.toMap()["data"].toMap()["message"].toMap()["cb_id"].toString().toStdString();
	qDebug() << "----";
	foreach ( QVariant q, data.toMap()["data"].toMap()["message"].toList() ) {
		qDebug() << q.toMap()["cb_id"] << " -- " <<  q.toMap()["data"];

		// TODO: MAKE THIS DETECTION AUTOMATIC
		QMap<QString,QVariant> map = q.toMap()["data"].toMap();
		QString out = "{";
		QMap<QString, QVariant>::iterator i;
		for (i = map.begin(); i != map.end(); ++i) {
			if(i != map.begin())
				out += ", ";
			out += "\"" + i.key();
			out += "\" : \"";
			out += i.value().toString() + "\"";
		}
		out += "}";

		parent->updateInternal( q.toMap()["cb_id"].toString(), /* q.toMap()["data"].toString()*/ out, q.toMap()["stamp"].toULongLong(), q.toMap()["deleted"].toBool());
	}

	// is this a good idea...?
	delete this;
}

#endif /* CLOUDLIST_RESPONDERS_H_ */
