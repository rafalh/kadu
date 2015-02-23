/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef JABBER_SUBSCRIPTION_SERVICE_H
#define JABBER_SUBSCRIPTION_SERVICE_H

#include <QtCore/QObject>

#include "protocols/services/subscription-service.h"

class JabberProtocol;

class JabberSubscriptionService : public SubscriptionService
{
	Q_OBJECT

	JabberProtocol *Protocol;

private slots:
	// void subscription(const Jid &jid, const QString &type, const QString &nick);

public:
	explicit JabberSubscriptionService(JabberProtocol *protocol);

	virtual void resendSubscription(const Contact &contact);
	virtual void removeSubscription(const Contact &contact);
	virtual void requestSubscription(const Contact &contact);

	void sendSubsription(const Contact &contact, const QString &subscription);

public slots:
	virtual void authorizeContact(Contact contact, bool authorized);

};

#endif // JABBER_SUBSCRIPTION_SERVICE_H
