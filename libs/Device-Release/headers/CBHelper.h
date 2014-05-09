/* Copyright (c) 2013 Cloudbase.io Limited
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
#include <QObject>
#include <bb/cascades/WebNavigationRequest>
#include <QtLocationSubset/QGeoPositionInfo>
#include <bps/locale.h>
#include <bb/device/HardwareInfo>

#include "yajl/YAJLDom.h"

#include "CBHttpConnection.h"
#include "DataCommands/CBHelperDataCommandList.h"
#include "DataCommands/CBHelperSearchCondition.h"
#include "CBHelperResponder.h"
#include "CBPayPal.h"

namespace Cloudbase {

#ifndef CBHELPER_H_
#define CBHELPER_H_

/*! \mainpage cloudbase.io BlackBerry 10 Native SDK Helper Class Reference
 *
 * \section intro_sec Introduction
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 2, as published by
 * the Free Software Foundation.<br/><br/>
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.<br/><br/>

 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to the Free
 * Software Foundation, 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.<br/><br/>
 *
 * \section install_sec Getting Started
 *
 * The cloudbase.io BlackBerry 10 helper class compiles to a .so dynamic library. The project
 * needs to be part of your workspace and the CBHelper directory in your path for
 * additional includes.<br/><br/>
 * This full reference is a companion to <a href="/documentation/blackberry-10/get-started" target="_blank">
 * the tutorial on the cloudbase.io website<a/>
 */
#include <QObject>

class CBHelper : public QObject, CBHelperResponder {
	Q_OBJECT
public:
	bool debugMode;
	bool isHttps;
	QtMobilitySubset::QGeoPositionInfo* currentLocation; /// The current location information
	std::string sessionId;
	/**
	 * The cloudbase.io API domain. By default set to api.cloudbase.io - should be changed if running on a private
	 * cloud instance
	 */
	std::string domain;

	bool isBesApp;

	/**
	* Initializes the new cloudbase.io CBHelper object for the given application
	* @param appCode the application code as registered on cloudbase.io (for example test-application)
	* @param appUniq the unique application code generated by cloudbase.io (df2edf1f2444c3dccc570224915609d5)
	*/
	CBHelper(std::string appCode, std::string appUniq);

	/**
	 * Sets the password of the current application. This should be the md5 of the password as specified
	 * when creating the application on cloudbase.io
	 * @param pwd The md5 string representation of the application password on cloudbase.io
	 */
	void setPassword(std::string pwd);

	void logDebug(std::string line, std::string category) { writeLog(line, category, "DEBUG"); };
	void logInfo(std::string line, std::string category) { writeLog(line, category, "INFO"); };
	void logWarn(std::string line, std::string category) { writeLog(line, category, "WARNING"); };
	void logError(std::string line, std::string category) { writeLog(line, category, "ERROR"); };
	void logFatal(std::string line, std::string category) { writeLog(line, category, "FATAL"); };
	void logEvent(std::string line, std::string category) { writeLog(line, category, "EVENT"); };
	/**
	 * Writes a line to the cloudbase.io application log. This is used for debug purposes as well as to
	 * collect analytics about the application (see log EVENT in the documentation).
	 * A number of shortcut methods are also available: logDebug, logInfo, logWarn, logError, logFatal
	 * and logEvent
	 * @param line The line of text to be written to the log
	 * @param category The category of the line - this is a custom text field that can be used to keep
	 * the log output for different sections of the app segregated
	 * @param level The severity of the line. Possible values are (DEBUG/INFO/WARNING/ERROR/FATAL/EVENT)
	 */
	void writeLog(std::string line, std::string category, std::string level);

	/**
	 * Log the navigation to a new screen
	 * @param viewName An NSstd::string representing the unique name for the loaded UIViewController
	 */
	void logNavigation(std::string screenName);

	/**
	 * Inserts the given object in a cloudbase collection. If the collection does not exist it is automatically created.
	 * Similarly if the data structure of the given object is different from documents already present in the collection
	 * the structure is automatically altered to accommodate the new object.
	 * The data object implementing the CBSerializable interface should always return a JSon array. This mean it can contain
	 * multiple objects and they will be inserted as multiple rows if needed. Otherwise simply wrap your object withing square
	 * brackets [ ]
	 * Once the call to the APIs is completed the responder is called
	 * @param collectionName The name of the collection (table) in your cloudbase database.
	 * @param data The objec to be inserted in the collection
	 * @param reponder The CBHelperResponder object to handle the response from the cloudbase.io servers
	 */
	void insertDocument(std::string collectionName, CBSerializable* data, CBHelperResponder* responder = NULL);
	/**
	 * Inserts the given object in a cloudbase collection. If the collection does not exist it is automatically created.
	 * Similarly if the data structure of the given object is different from documents already present in the collection
	 * the structure is automatically altered to accommodate the new object.
	 * The data object implementing the CBSerializable interface should always return a JSon array. This mean it can contain
	 * multiple objects and they will be inserted as multiple rows if needed. Otherwise simply wrap your object withing square
	 * brackets [ ]
	 * Once the call to the APIs is completed the requestCompleted method is triggered in the delegate.
	 * @param collectionName The name of the collection (table) in your cloudbase database.
	 * @param data The objec to be inserted in the collection
	 * @param atts A Vector of CBHelperAttachment objects representing the files to be attached to the request
	 * @param reponder The CBHelperResponder object to handle the response from the cloudbase.io servers
	 */
	void insertDocument(std::string collectionName, CBSerializable* data, std::vector<CBHelperAttachment> atts = NULL, CBHelperResponder* responder = NULL);

	/**
	 * Updates an document in the cloud database. It replaces the existing document with the one sent.
	 * Once the call to the APIs is completed the responder is called
	 * @param collectionName The name of the collection (table) in your cloudbase database.
	 * @param data The object to be updated in the collection
	 * @param conditions A set of search conditions to identify which objects to update
	 * @param reponder The CBHelperResponder object to handle the response from the cloudbase.io servers
	 */
	void updateDocument(std::string collectionName, CBSerializable* data, CBHelperSearchCondition* conditions, CBHelperResponder* responder = NULL);

	/***
	 * Runs a search over a collection with the given criteria. The documents matching the search criteria are then
	 * returned in the CBHelperResponseInfo object passed to the responder.
	 * @param collection The name of the collection to run the search over
	 * @param conditions A set of search conditions
	 * @param responder The CBHelperResponder object to handle the response from the cloudbase.io servers
	 */
	void searchDocument(std::string collectionName, CBHelperSearchCondition* conditions, CBHelperResponder* responder = NULL);
	/**
	 * Runs a search over a collection and applies the given list of aggregation commands to the output.
	 * @param collectionName The name of the collection to run the search over
	 * @param commands A List of CBDataAggregationCommand objects
	 * @param responder a block of code to be executed once the request is completed
	 */
	void searchDocumentAggregate(std::string collectionName, CBHelperDataCommandList* commands, CBHelperResponder* responder = NULL);
	/**
	 * Downloads a file attached to a document in the CloudBase database using the file_id generated once the file is uploaded.
	 * More information on file attachments is available here http://cloudbase.io/documentation/rest-apis#CloudBase
	 * @param fileId The cloudbase.io id generated for the file
	 * @param responder A responder implementing the fileDownloaded(CBHelperAttachment* fileData) method.
	 */
	void downloadFile(std::string fileId, CBHelperResponder* responder = NULL);
	/**
	 * Subscribes the devices with the current token received from the manufacturer to a notification channel. All devices are
	 * autmatically subscribed to the channel <strong>all</strong>.
	 * @param channel The name of the channel the device is subscribing to - if the channel doesn't exist it is
	 * automatically created
	 * @param responder The CBHelperResponder object to handle the response from the cloudbase.io servers
	 */
	void subscribeDeviceWithToken(std::string channel, CBHelperResponder* responder = NULL);
	/**
	 * Unsubscribes a device from a notification channel.
	 * @param token The token returned by the createChannelCompleted SIGNAR of the bb::network::PushService object
	 * @param channel The name of the channel the device is unsubscribing from
	 * @param fromAll Whether it should also be removed from the "all" channel
	 * @param responder The CBHelperResponder object to handle the response from the cloudbase.io servers
	 */
	void unsubscribeDeviceWithToken(std::string token, std::string channel, bool fromAll, CBHelperResponder* responder = NULL);
	/**
	 * Sends a push notification to a specific channel. Push notifications from client devices need to be enabled
	 * in the application security settings in your cloudbase.io control panel.
	 * @param channel The name of the channel to notify
	 * @param notification The content of the notification
	 * @param responder The CBHelperResponder object to handle the response from the cloudbase.io servers
	 */
	void sendNotification(std::string channel, std::string notification, CBHelperResponder* responder = NULL);
	/**
	 * Sends an email to a recipient using a template previously created on cloudbase.io
	 * @param templateCode The code assigned to the template
	 * @param recipient The email address of the recipient
	 * @param subject The subject of the email notification
	 * @param vars A Map of variables to populate the template. The serialized String of this object should
	 * look something like this: { "first_name" : "cloud", "last_name" : "base", "title" : ".io" }
	 */
	void sendNotificationEmail(std::string templateCode, std::string recipient, std::string subject, CBSerializable* vars = NULL);

	/**
	 * Executes a CloudFunction declared on cloudbase.io
	 * @param functionCode The code assigned to the function
	 * @param params If necessary the parameters for the function - these will be received by your code on the
	 * cloudbase.io server as additional $_POST parameters.
	 * @param responder The CBHelperResponder object to handle the response from the cloudbase.io servers which
	 * includes the output from your CloudFunction
	 */
	void executeCloudFunction(std::string functionCode, std::map<std::string, std::string> params, CBHelperResponder* responder = NULL);
	/**
	 * Executes a cloudbase.io Applet
	 * @param appletCode The code for the applet
	 * @param params Additional parameters for the Applet
	 * @param responder The CBHelperResponder object to handle the response from the cloudbase.io servers which
	 * contains the output from the Applet
	 */
	void executeApplet(std::string appletCode, std::map<std::string, std::string> params, CBHelperResponder* responder = NULL);

	/**
	 * Executes a cloudbase.io Shared Api
	 * @param apiCode The unique identifier for the Shared Api
	 * @param password The password for the Shared Api if required
	 * @param params Additional parameters for the Shared Api
	 * @param responder The CBHelperResponder object to handle the response from the cloudbase.io servers which
	 * contains the output from the Shared Api
	 */
	void executeSharedApi(std::string apiCode, std::string password, std::map<std::string, std::string> params, CBHelperResponder* responder = NULL);

	/**
	 * Initiates a transaction with PayPal by sending the payment details and retrieving a token
	 * and an express checkout url. The url returned should be then opened in a browser window.
	 * @param purchaseDetails a populated CBPayPalBill object
	 * @param isLiveEnvironment whether we are using the production or sandbox paypal environments
	 * @param responder a responder to handle the returned PayPal token and submission url
	 */
	void preparePayPalPurchase(CBPayPalBill purchaseDetails, bool isLiveEnvironment, CBHelperResponder* responder = NULL);
	/**
	 * Once the PayPal purchase is completed this method updates the record in the cloudbase.io database.
	 * The responder can then proceed to close the payment window using the output of the call
	 * @param url The url returned by PayPal once the payment is completed
	 * @param responder The responder to complete the payment in the application
	 */
	void completePayPalPurchase(std::string url, CBHelperResponder* responder = NULL);

	/**
	 * If client authentication is enabled in the cloudbase.io settings then this function sets the username
	 * and password of the current user in the application. The user will be authenticated against the collection
	 * specified in the cloudbase.io security settings for the application
	 * @param username The username of the user logged onto the application
	 * @param password The password of the user - if the password are stored in encrypted format during the registration
	 *  then this string should be encrypted in exactly the same way.
	 */
	void setAuthentication(std::string username, std::string password);


	void parseResponse(CBHelperResponseInfo resp);

protected:
	std::string appCode;
	std::string appUniq;
	std::string password;

	std::string defaultLogCategory;

	std::string country;
	std::string language;

	std::string deviceUniqueIdentifier;
	std::string deviceModel;
	std::string deviceVersion;
	std::string notificationNetwork;
	//std::string deviceLanguage;

	std::string authUsername;
	std::string authPassword;

	void registerDevice();
	CBHttpConnection* createConnection(std::string function);
private:
	bool deviceRegistered;
	//CBRegisterDeviceResponder* registerResponder;

	std::string devicePin;

	void baseInit();
	std::string generateURL();
public Q_SLOTS:
	/**
	 * This slot is used internally to monitor the status of a request to the cloudbase.io APIs
	 */
	void handleRequestCompleted(CBHelperResponseInfo resp, CBHelperResponder* responder);

Q_SIGNALS:
	/**
	 * triggered whenever a new http request to the cloudbase.io APIs is started
	 * The parameter contains the function type being called:
	 *  - log
	 *  - data
	 *  - notification
	 *  - email
	 *  - cloudfunction
	 *  - applet
	 *  -paypal
	 */
	void requestStarted(std::string function);
	/**
	 * triggered once a request is completed with the http status code.
	 * Anything but 200 is an error http://en.wikipedia.org/wiki/List_of_HTTP_status_codes
	 */
	void requestCompleted(int httpStatus);
};

class CBRegisterDevice : public CBSerializable {
public:
	CBRegisterDevice(std::string deviceType, std::string deviceName, std::string deviceModel, std::string language, std::string country) :
		deviceType_(deviceType), deviceName_(deviceName), deviceModel_(deviceModel), language_(language), country_(country) {}

	std::string serialize() {
		std::string out = "{";
		out += "\"device_type\" : \"" + deviceType_ + "\", ";
		out += "\"device_name\" : \"" + deviceName_ + "\", ";
		out += "\"device_model\" : \"" + deviceModel_ + "\", ";
		out += "\"language\" : \"" + language_ + "\", ";
		out += "\"country\" : \"" + country_ + "\" } ";

		return out;
	}
private:
	std::string deviceType_;
	std::string deviceName_;
	std::string deviceModel_;
	std::string language_;
	std::string country_;
};

class CBPushNotification : public CBSerializable {
public:

	CBPushNotification(std::string action, std::string token, std::string channel, std::string network) :
		action_(action), deviceToken(token), deviceChannel(channel), deviceNetwork(network), fromAll(false) {};

	std::string serialize() {
		std::string out = "{";
		out += "\"action\" : \"" + action_ + "\", ";
		out += "\"device_key\" : \"" + deviceToken + "\", ";
		out += "\"device_network\" : \"" + deviceNetwork + "\", ";
		out += "\"channel\" : \"" + deviceChannel + "\" ";

		if (fromAll) {
			//std::string isAll = (fromAll?"true":"false")l
			out += ", \"from_all\" : \"true\" ";
		}

		out += " } ";
		return out;
	};

	void setFromAll(bool newValue) { this->fromAll = newValue; };
private:
	std::string action_;
	std::string deviceToken;
	std::string deviceChannel;
	std::string deviceNetwork;
	bool fromAll;
};

class CBLogLine : public CBSerializable {
public:
	CBLogLine(std::string category, std::string level, std::string logLine, std::string deviceName, std::string deviceModel) :
		category_(category), level_(level), logLine_(logLine), deviceName_(deviceName), deviceModel_(deviceModel) {};

	std::string serialize() {
		std::string out = "{";
		out += "\"category\" : \"" + category_ + "\", ";
		out += "\"level\" : \"" + level_ + "\", ";
		out += "\"log_line\" : \"" + logLine_ + "\", ";
		out += "\"device_name\" : \"" + deviceName_ + "\", ";
		out += "\"device_model\" : \"" + deviceModel_ + "\" } ";

		return out;
	}
private:
	std::string category_;
	std::string level_;
	std::string logLine_;
	std::string deviceName_;
	std::string deviceModel_;
};

class CBPushNotificationMessage : public CBSerializable {
public:
	CBPushNotificationMessage(std::string channel, std::string notification) :
		channel_(channel), notification_(notification) {};

	std::string serialize() {
		std::string out = "{";
		out += "\"channel\" : \"" + channel_ + "\", ";
		out += "\"cert_type\" : \"production\", ";
		out += "\"alert\" : \"" + notification_ + "\", ";
		out += "\"badge\" : \"\", ";
		out += "\"sound\" : \"\" } ";

		return out;
	}
private:
	std::string channel_;
	std::string notification_;
};

class CBNotificationEmail : public CBSerializable {
public:
	CBNotificationEmail(std::string templateCode, std::string recipient, std::string subject, CBSerializable* vars) :
		template_(templateCode), recipient_(recipient), subject_(subject), vars_(vars) {};

	std::string serialize() {
		std::string out = "{";
		out += "\"template_code\" : \"" + template_ + "\", ";
		out += "\"recipient\" : \"" + recipient_ + "\", ";
		out += "\"subject\" : \"" + subject_ + "\", ";
		out += "\"variables\" : \"" + vars_->serialize() + "\" } ";

		return out;
	}
private:
	std::string template_;
	std::string recipient_;
	std::string subject_;
	CBSerializable* vars_;
};

class CBNavigation : public CBSerializable {
public:
	CBNavigation(std::string sessionId, std::string screenName) :
		sessionId_(sessionId), screenName_(screenName) {};

	std::string serialize() {
		std::string out = "{";
		out += "\"session_id\" : \"" + sessionId_ + "\", ";
		out += "\"screen_name\" : \"" + screenName_ + "\" } ";

		return out;
	}
private:
	std::string sessionId_;
	std::string screenName_;
};

class CBPayPalPurchase : public CBSerializable {
public:
	CBPayPalPurchase(CBPayPalBill bill, bool live, std::string type) :
		bill_(bill), liveEnv_(live), currency_(bill.currency), type_(type),
		completedF_(bill.paymentCompletedFunction), cancelledF_(bill.paymentCancelledFunction),
		completedUrl_(bill.paymentCompletedUrl), cancelledUrl_(bill.paymentCancelledUrl) {};

	std::string serialize() {
		std::string out = "{";
		out += "\"environment\" : \"";
		out += (liveEnv_?"live":"sandbox");
		out += "\", ";
		out += "\"currency\" : \"" + currency_ + "\", ";
		out += "\"type\" : \"" + type_ + "\", ";
		out += "\"completed_cloudfunction\" : \"" + completedF_ + "\", ";
		out += "\"cancelled_cloudfunction\" : \"" + cancelledF_ + "\", ";
		out += "\"payment_completed_url\" : \"" + completedUrl_ + "\", ";
		out += "\"payment_cancelled_url\" : \"" + cancelledUrl_ + "\", ";

		out += "\"purchase_details\" : " + bill_.serialize();
		out += " } ";

		return out;
	}
private:
	CBPayPalBill bill_;
	bool liveEnv_;
	std::string currency_;
	std::string type_;
	std::string completedF_;
	std::string cancelledF_;
	std::string completedUrl_;
	std::string cancelledUrl_;
};

#endif /* CBHELPER_H_ */

}
