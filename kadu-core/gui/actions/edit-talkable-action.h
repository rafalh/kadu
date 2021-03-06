/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include "gui/actions/action-description.h"
#include "model/roles.h"

#include <QtCore/QPointer>
#include <QtWidgets/QAction>
#include <injeqt/injeqt.h>

class BuddyDataWindowRepository;
class Buddy;
class ChatDataWindowRepository;
class ChatTypeManager;
class Chat;
class KaduWindowService;
class Myself;

class EditTalkableAction : public ActionDescription
{
	Q_OBJECT

	QPointer<BuddyDataWindowRepository> m_buddyDataWindowRepository;
	QPointer<ChatDataWindowRepository> m_chatDataWindowRepository;
	QPointer<ChatTypeManager> m_chatTypeManager;
	QPointer<KaduWindowService> m_kaduWindowService;
	QPointer<Myself> m_myself;

	int actionRole(ActionContext *context) const;
	Chat actionChat(ActionContext *context) const;
	Buddy actionBuddy(ActionContext *context) const;

	void setChatActionTitleAndIcon(Action *action);
	void setBuddyActionTitleAndIcon(Action *action);

	void updateChatActionState(Action *action);
	void updateBuddyActionState(Action *action);

	void chatActionTriggered(ActionContext *context);
	void buddyActionTriggered(ActionContext *context);

private slots:
	INJEQT_SET void setBuddyDataWindowRepository(BuddyDataWindowRepository *buddyDataWindowRepository);
	INJEQT_SET void setChatDataWindowRepository(ChatDataWindowRepository *chatDataWindowRepository);
	INJEQT_SET void setChatTypeManager(ChatTypeManager *chatTypeManager);
	INJEQT_SET void setKaduWindowService(KaduWindowService *kaduWindowService);
	INJEQT_SET void setMyself(Myself *myself);

protected:
	virtual void actionInstanceCreated(Action *action);
	virtual void updateActionState(Action *action);
	virtual void triggered(QWidget *widget, ActionContext *context, bool toggled);

public:
	explicit EditTalkableAction(QObject *parent);
	virtual ~EditTalkableAction();

	void trigger(ActionContext *context);

};
