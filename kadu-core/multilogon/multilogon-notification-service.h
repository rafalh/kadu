/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "injeqt-type-roles.h"

#include "notification/notification-callback.h"
#include "notification/notification-event.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class MultilogonManager;
struct MultilogonSession;
class NotificationCallbackRepository;
class NotificationEventRepository;
class NotificationService;
struct Notification;

class MultilogonNotificationService : public QObject
{
	Q_OBJECT
	INJEQT_TYPE_ROLE(SERVICE)

public:
	Q_INVOKABLE explicit MultilogonNotificationService(QObject *parent = nullptr);
	virtual ~MultilogonNotificationService();

public slots:
	void notifyMultilogonSessionConnected(const MultilogonSession &session);
	void notifyMultilogonSessionDisonnected(const MultilogonSession &session);

private:
	QPointer<NotificationCallbackRepository> m_notificationCallbackRepository;
	QPointer<NotificationEventRepository> m_notificationEventRepository;
	QPointer<NotificationService> m_notificationService;

	NotificationCallback m_mutlilogonDisconnectCallback;
	NotificationEvent m_multilogonEvent;
	NotificationEvent m_multilogonConnectedEvent;
	NotificationEvent m_multilogonDisconnectedEvent;

	void disconnectSession(const Notification &notification);

private slots:
	INJEQT_SET void setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository);
	INJEQT_SET void setNotificationEventRepository(NotificationEventRepository *notificationEventRepository);
	INJEQT_SET void setNotificationService(NotificationService *notificationService);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

};
