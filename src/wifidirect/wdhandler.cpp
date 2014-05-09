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

#include "wdhandler.h"
#include <QThread>

WDHandler::WDHandler(QObject* parent)
: QObject(parent),
  m_direct(new bb::device::WiFiDirect(this)),
  m_model(new bb::cascades::GroupDataModel(QStringList() << "Name" << "HardwareAddress", this)),
  m_ipAddr("127.0.0.1"),
  m_session(""), m_stat(false)
{
	connect(m_direct, SIGNAL(scanCompleted()), this, SLOT(scanCompleted()));
	connect(m_direct, SIGNAL(deviceFound(const QString, const QString, bb::device::WiFiDirectDeviceType::Type)),
			this, SLOT(deviceFound(const QString, const QString, bb::device::WiFiDirectDeviceType::Type)));
	connect(m_direct, SIGNAL(deviceLost(const QString)), this, SLOT(deviceLost(const QString)));
	connect(m_direct, SIGNAL(sessionStarted(const QString)), this, SLOT(sessionStarted(const QString)));
	connect(m_direct, SIGNAL(deviceConnected(const QString, const QString, bb::device::WiFiDirectDeviceType::Type, const QString, bb::device::WiFiDirectIntent::Type)),
			this, SLOT(deviceConnected(const QString, const QString, bb::device::WiFiDirectDeviceType::Type, const QString, bb::device::WiFiDirectIntent::Type)));
	connect(m_direct, SIGNAL(deviceDisconnected(const QString)), this, SLOT(deviceDisconnected(const QString)));
}

WDHandler::~WDHandler()
{
	if (m_session.size() > 0) {
		qDebug() << "Stopping session.";
		m_direct->requestStopSession(m_session);
		m_direct->deleteLater();
	}
}


void WDHandler::scan()
{
	qDebug() << "Started scanning...";

	if (m_direct->connectionState() == bb::device::WiFiDirectConnectionState::Connected) {
		qDebug() << "Scan Start Result: " << false << " + WiFi Direct or Miracast in use.";
		return;
	}

	if (m_session.size() > 0) {
		qDebug() << "Stopping session.";
		m_direct->requestStopSession(m_session);
		m_session = "";
	}

	m_model->clear();
	emit modelChanged();

	bb::device::WiFiDirectResult::Type result = m_direct->requestScan();
	bool success = true;
	QString resultError;

	if (result != bb::device::WiFiDirectResult::Success) {
		success = false;
		resultError = resultToText(result);
	}

	qDebug() << "Scan Start Result: " << success << " + " << resultError;
}

void WDHandler::requestConnect(const QString& hardwareAddress)
{
	qDebug() << "Connecting to: " << hardwareAddress;
	bool success = true;
	QString resultError;

	bb::device::WiFiDirectResult::Type result = m_direct->requestConnect(hardwareAddress,
			bb::device::WiFiDirectIntent::GroupOwner,
			bb::device::WiFiDirectWpsMethod::PushButtonConfiguration, "");

	if (result != bb::device::WiFiDirectResult::Success) {
		success = false;
		resultError = resultToText(result);
	}
	qDebug() << "starting session: " << m_direct->requestStartSession();

	qDebug() << "Connection Attempt: " << success << " + " << resultError;
}

void WDHandler::readyClient()
{
	m_direct->requestStartSession();
	m_client = new Client(this);
	m_client->connectToHost(m_ipAddr);
	emit clientReady();
}

void WDHandler::readyServer()
{
	m_direct->requestStartSession();
	emit serverReady();
}

void WDHandler::scanCompleted()
{
	qDebug() << "Scan completed!";
}

void WDHandler::deviceFound(const QString& name, const QString& hardwareAddress, bb::device::WiFiDirectDeviceType::Type type)
{
	qDebug() << "Device found: " << name << " + " << hardwareAddress << " type " << type;

	if (bb::device::WiFiDirectDeviceType::Telephone == type) {
		foreach(QString id, phoneCons.keys()) {
			if (name.compare(id) == 0) {
				qDebug() << "found a phone we want to connect to!";
				requestConnect(hardwareAddress);
			}
		}
	}
}

void WDHandler::deviceLost (const QString& hardwareAddress)
{
	qDebug() << "Device lost: " << hardwareAddress;
}

void WDHandler::sessionStarted(const QString& sessionName)
{
	qDebug() << "Session started. Name: " << sessionName;
	m_session = sessionName;
}

void WDHandler::deviceConnected (const QString& name, const QString& hardwareAddress, bb::device::WiFiDirectDeviceType::Type type,
		const QString& ipAddress, bb::device::WiFiDirectIntent::Type intent)
{
	qDebug() << "Device Connected: " << name << " + " << hardwareAddress << " type " << type;
	m_ipAddr = ipAddress;
}

void WDHandler::deviceDisconnected(const QString& hardwareAddress)
{
	qDebug() << "Device Disconnected: " << hardwareAddress;
}

QString WDHandler::resultToText(bb::device::WiFiDirectResult::Type result)
{
	QString resultError = "Unknown error has occurred.";

	switch (result)
		{
		case bb::device::WiFiDirectResult::Success:
			break;

		case bb::device::WiFiDirectResult::InternalError:
			resultError = "Error: An unrecoverable internal error occurred.";
			break;

		case bb::device::WiFiDirectResult::Busy:
			resultError =
					"Error: Cannot scan as system is busy and being used by other WiFi direct service (eg.Miracast).";
			break;

		case bb::device::WiFiDirectResult::DeviceNotFound:
			resultError = "Error: Device with specified hardware address was not found.";
			break;

		case bb::device::WiFiDirectResult::NoNetworkConnection:
			resultError = "Error: Device was not connected to a network.";
			break;

		case bb::device::WiFiDirectResult::NoRequestToCancel:
			resultError = "Error: No active connection attempt available to cancel.";
			break;

		case bb::device::WiFiDirectResult::NotSupported:
			resultError = "Error: WiFiDirect is not supported on this device.";
			break;

		case bb::device::WiFiDirectResult::InvalidSession:
			resultError = "Error: The supplied session was not found.";
			break;
		}

	return resultError;
}

void WDHandler::addTarget(QString phoneId)
{
	phoneCons[phoneId] = NULL;
}


bb::cascades::GroupDataModel* WDHandler::model() const
{
    return m_model;
}

bool WDHandler::stat() const
{
	return m_stat;
}

void WDHandler::setStat(bool b)
{
	if (m_stat != b) {
		m_stat = b;
		emit statChanged();
	}
}

void WDHandler::conDevs()
{
	qDebug() << "Visible: " << m_direct->visibleDevices();
	qDebug() << "Connected: " << m_direct->connectedDevices();
}

