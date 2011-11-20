/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef PROTOCOLS_AWARE_OBJECT
#define PROTOCOLS_AWARE_OBJECT

#include <QtCore/QList>

#include "aware-object.h"

class ProtocolFactory;

class KADUAPI ProtocolsAwareObject : public AwareObject<ProtocolsAwareObject>
{

protected:
	virtual void protocolRegistered(ProtocolFactory *protocolFactory) = 0;
	virtual void protocolUnregistered(ProtocolFactory *protocolFactory) = 0;

public:
	static void notifyProtocolRegistered(ProtocolFactory *protocolFactory);
	static void notifyProtocolUnregistered(ProtocolFactory *protocolFactory);

	void triggerAllProtocolsRegistered();
	void triggerAllProtocolsUnregistered();

};

#endif // PROTOCOLS_AWARE_OBJECT
