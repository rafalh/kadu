/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "recent-chats-action.h"

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "core/injected-factory.h"
#include "gui/actions/action.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/recent-chats-menu.h"

#include <QtWidgets/QMenu>

RecentChatsAction::RecentChatsAction(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeMainMenu);
	setName("openRecentChatsAction");
	setIcon(KaduIcon("internet-group-chat"));
	setText(tr("Recent Chats"));
}

RecentChatsAction::~RecentChatsAction()
{
}

void RecentChatsAction::setChatWidgetManager(ChatWidgetManager *chatWidgetManager)
{
	m_chatWidgetManager = chatWidgetManager;
}

void RecentChatsAction::init()
{
	m_recentChatsMenu = injectedFactory()->makeNotOwned<RecentChatsMenu>();
	connect(m_recentChatsMenu, SIGNAL(triggered(QAction *)), this, SLOT(openRecentChats(QAction *)));
}

void RecentChatsAction::actionInstanceCreated(Action *action)
{
	action->setEnabled(false);
	action->setMenu(m_recentChatsMenu);
	connect(m_recentChatsMenu, SIGNAL(chatsListAvailable(bool)), action, SLOT(setEnabled(bool)));
	m_recentChatsMenu->invalidate();
}

void RecentChatsAction::openRecentChats(QAction *action)
{
	m_chatWidgetManager->openChat(action->data().value<Chat>(), OpenChatActivation::Activate);
}

#include "moc_recent-chats-action.cpp"
