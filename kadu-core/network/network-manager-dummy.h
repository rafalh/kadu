/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QObject>

#include "exports.h"
#include "network-manager.h"

/**
 * @addtogroup Network
 * @{
 */

/**
 * @class NetworkManagerDummy
 * @author Rafał 'Vogel' Malinowski
 * @author Piotr 'ultr' Dąbrowski
 * @short Class responsible for network online-offline notifications.
 * @see NetworkAwareObject
 *
 * This class is dummy implementation of NetworkManager that always assumes that network is online. This
 * implementation is used on system that do not have valid network-availability information.
 */
class KADUAPI NetworkManagerDummy : public NetworkManager
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit NetworkManagerDummy(QObject *parent = nullptr);
	virtual ~NetworkManagerDummy();

	virtual bool isOnline();

public slots:
	virtual void forceOnline();

};

/**
 * @}
 */
