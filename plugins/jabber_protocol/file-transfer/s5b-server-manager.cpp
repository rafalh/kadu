/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"

#include "s5b-server-manager.h"

S5BServerManager *S5BServerManager::Instance = 0;

void S5BServerManager::createInstance()
{
	if (!Instance)
		Instance = new S5BServerManager();
}

void S5BServerManager::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

S5BServerManager::S5BServerManager() // :
		// Server(new S5BServer(this)), Port(-1)
{
	createDefaultConfiguration();
	configurationUpdated();
}

S5BServerManager::~S5BServerManager()
{
}

void S5BServerManager::createDefaultConfiguration()
{
	Application::instance()->configuration()->deprecatedApi()->addVariable("XMPP", "DataTransferPort", 8010);
	Application::instance()->configuration()->deprecatedApi()->addVariable("XMPP", "DataTransferExternalAddress", "");
}

void S5BServerManager::configurationUpdated()
{
	int port = Application::instance()->configuration()->deprecatedApi()->readNumEntry("XMPP", "DataTransferPort", 8010);
	QString externalAddress = Application::instance()->configuration()->deprecatedApi()->readEntry("XMPP", "DataTransferExternalAddress", "");

	if (externalAddress != ExternalAddress)
	{
		if (!ExternalAddress.isEmpty())
			removeAddress(ExternalAddress);
		ExternalAddress = externalAddress;
		if (!ExternalAddress.isEmpty())
			addAddress(ExternalAddress);
	}

	if (Port != port)
	{
		// if (Server->isActive())
		// 	Server->stop();

		Port = port;
		// Server->start(Port);
	}
}

void S5BServerManager::addAddress(const QString &address)
{
	Addresses.append(address);

	// remove duplicates
	// server()->setHostList(QSet<QString>::fromList(Addresses).toList());
}

void S5BServerManager::removeAddress(const QString &address)
{
	Addresses.removeOne(address);

	// remove duplicates
	// server()->setHostList(QSet<QString>::fromList(Addresses).toList());
}

#include "moc_s5b-server-manager.cpp"
