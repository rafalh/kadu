/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <stdio.h>

#include "uuid-storable-object.h"

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Contructs object with StateNew state and null storage point.
 *
 * Constructs object with @link<StorableObject::StateNew state @endlink and null
 * (invalid) @link<StorableObject::storage storage point @endlink.
 */
UuidStorableObject::UuidStorableObject(QObject *parent) :
		StorableObject{parent}
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Creates default storage point for object.
 *
 * Constructs storage point: XML node that is child of storage point of object
 * returned by @link<StorableObject::storageParent storageParent @endlink method.
 * Node name is given by @link<StorableObject::storageNodeName @endlink method.
 * Node contains one attribute: uuid, that contains value returned by
 * @link<uuid> uuid @endlink method.
 *
 * If @link<StorableObject::storageNodeName @endlink returns invalid node name
 * (empty string) or @link<StorableObject::storageParent storageParent @endlink
 * returns object that has invalid storage point, this method returns invalid
 * storage point.
 *
 * If parent is NULL this method will return storage point that is child of
 * root node of XML configuration file.
 */
std::shared_ptr<StoragePoint> UuidStorableObject::createStoragePoint()
{
	if (storageNodeName().isEmpty())
		return {};

	if (!storageParent())
		return {};

	auto parentStoragePoint = storageParent()->storage();
	if (!parentStoragePoint)
		return {};

	QUuid id = uuid();
	if (id.isNull())
		return {};

	QDomElement node = parentStoragePoint->storage()->getUuidNode(parentStoragePoint->point(), storageNodeName(), id.toString());
	return std::make_shared<StoragePoint>(parentStoragePoint->storage(), node);
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Determines if object is worth to be stored.
 * @return true if object should be stored, defaults to true
 * @todo this method is used only in managers, in 0.10 it should be used in every place
 *
 * Returns true if object has valid uuid. Otherwise object will not be stored.
 */
bool UuidStorableObject::shouldStore()
{
	return StorableObject::shouldStore() && !uuid().isNull();
}
