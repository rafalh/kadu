/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

extern "C" {
#	include <libotr/proto.h>
}

class ChatManager;
class ChatStorage;
class Contact;
class MessageManager;

class OtrAppOpsService;
class OtrOpDataFactory;
class OtrPolicyService;
class OtrTrustLevelService;
class OtrUserStateService;

class OtrSessionService : public QObject
{
	Q_OBJECT

public:
	static void wrapperOtrGoneSecure(void *data, ConnContext *context);
	static void wrapperOtrGoneInsecure(void *data, ConnContext *context);
	static void wrapperOtrStillSecure(void *data, ConnContext *context, int isReply);

	Q_INVOKABLE OtrSessionService();
	virtual ~OtrSessionService();

public slots:
	void startSession(const Contact &contact);
	void endSession(const Contact &contact);

signals:
	void tryingToStartSession(const Contact &contact) const;
	void tryingToRefreshSession(const Contact &contact) const;

	void goneSecure(const Contact &contact) const;
	void goneInsecure(const Contact &contact) const;
	void stillSecure(const Contact &contact) const;

private slots:
	INJEQT_SET void setAppOpsService(OtrAppOpsService *appOpsService);
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setChatStorage(ChatStorage *chatStorage);
	INJEQT_SET void setMessageManager(MessageManager *messageManager);
	INJEQT_SET void setOpDataFactory(OtrOpDataFactory *opDataFactory);
	INJEQT_SET void setPolicyService(OtrPolicyService *policyService);
	INJEQT_SET void setTrustLevelService(OtrTrustLevelService *trustLevelService);
	INJEQT_SET void setUserStateService(OtrUserStateService *userStateService);

private:
	QPointer<ChatManager> m_chatManager;
	QPointer<ChatStorage> m_chatStorage;
	QPointer<MessageManager> CurrentMessageManager;
	QPointer<OtrAppOpsService> AppOpsService;
	QPointer<OtrOpDataFactory> OpDataFactory;
	QPointer<OtrPolicyService> PolicyService;
	QPointer<OtrTrustLevelService> TrustLevelService;
	QPointer<OtrUserStateService> UserStateService;

};
