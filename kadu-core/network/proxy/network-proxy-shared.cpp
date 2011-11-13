/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "network/proxy/network-proxy-manager.h"

#include "network-proxy-shared.h"

NetworkProxyShared * NetworkProxyShared::loadStubFromStorage(const QSharedPointer<StoragePoint> &storagePoint)
{
	NetworkProxyShared *result = loadFromStorage(storagePoint);
	result->loadStub();

	return result;
}

NetworkProxyShared * NetworkProxyShared::loadFromStorage(const QSharedPointer<StoragePoint> &storagePoint)
{
	NetworkProxyShared *result = new NetworkProxyShared();
	result->setStorage(storagePoint);

	return result;
}

NetworkProxyShared::NetworkProxyShared(const QUuid &uuid) :
		Shared(uuid), Port(0)
{
}

NetworkProxyShared::~NetworkProxyShared()
{
	ref.ref();
}

StorableObject * NetworkProxyShared::storageParent()
{
	return NetworkProxyManager::instance();
}

QString NetworkProxyShared::storageNodeName()
{
	return QLatin1String("Proxy");
}

void NetworkProxyShared::load()
{
	if (!isValidStorage())
		return;

	Shared::load();

	Address = loadValue<QString>("Address");
	Port = loadValue<int>("Port", 0);
	User = loadValue<QString>("User");
	Password = loadValue<QString>("Password");
}

void NetworkProxyShared::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	Shared::store();

	storeValue("Address", Address);
	storeValue("Port", Port);
	storeValue("User", User);
	storeValue("Password", Password);
}

void NetworkProxyShared::emitUpdated()
{
	emit updated();
}

QString NetworkProxyShared::displayName()
{
	ensureLoaded();

	if (User.isEmpty())
		return Address + ":" + QString::number(Port);
	else
		return User + "@" + Address + ":" + QString::number(Port);
}