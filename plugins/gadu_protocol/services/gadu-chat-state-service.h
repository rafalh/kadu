/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "message/message.h"

#include "protocols/services/chat-state-service.h"

#include <injeqt/injeqt.h>

class ContactManager;
class GaduConnection;

class GaduChatStateService : public ChatStateService
{
	Q_OBJECT

	QPointer<ContactManager> m_contactManager;
	QPointer<GaduConnection> Connection;

	bool SendTypingNotifications;

private slots:
	INJEQT_SET void setContactManager(ContactManager *contactManager);

public:
	explicit GaduChatStateService(Account account, QObject *parent = nullptr);
	virtual ~GaduChatStateService();

	void setConnection(GaduConnection *connection);

	virtual void sendState(const Contact &contact, ChatState state) override;

	void setSendTypingNotifications(bool sendTypingNotifications);

public slots:
	void handleEventTypingNotify(struct gg_event *e);

	void messageReceived(const Message & message);

};
