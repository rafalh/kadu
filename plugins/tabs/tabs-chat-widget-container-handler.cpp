/*
 * %kadu copyright begin%
 * Copyright 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "tabs-chat-widget-container-handler.h"

#include "gui/widgets/tab-widget.h"
#include "tabs.h"

#include "gui/widgets/chat-widget/chat-widget-container-handler-repository.h"

TabsChatWidgetContainerHandler::TabsChatWidgetContainerHandler(QObject *parent) :
		ChatWidgetContainerHandler{parent}
{
}

TabsChatWidgetContainerHandler::~TabsChatWidgetContainerHandler()
{
}

void TabsChatWidgetContainerHandler::setChatWidgetContainerHandlerRepository(ChatWidgetContainerHandlerRepository *chatWidgetContainerHandlerRepository)
{
	m_chatWidgetContainerHandlerRepository = chatWidgetContainerHandlerRepository;
}

void TabsChatWidgetContainerHandler::setTabsManager(TabsManager *tabsManager)
{
	m_tabsManager = tabsManager;
}

void TabsChatWidgetContainerHandler::init()
{
	connect(m_tabsManager.data(), SIGNAL(chatAcceptanceChanged(Chat)),
			this, SIGNAL(chatAcceptanceChanged(Chat)));
	connect(m_tabsManager.data(), SIGNAL(chatWidgetActivated(ChatWidget*)),
			this, SIGNAL(chatWidgetActivated(ChatWidget*)));
	// Note: m_tabsManager may not be fully inited here so delay getting tab widget from it to the next main loop iteration
	QTimer::singleShot(0, this, &TabsChatWidgetContainerHandler::lateInit);
	m_chatWidgetContainerHandlerRepository->registerChatWidgetContainerHandler(this);
}

void TabsChatWidgetContainerHandler::lateInit()
{
	connect(m_tabsManager->tabWidget(), SIGNAL(chatWidgetActivated(ChatWidget*)),
			this, SIGNAL(chatWidgetActivated(ChatWidget*)));
}

void TabsChatWidgetContainerHandler::done()
{
	m_chatWidgetContainerHandlerRepository->unregisterChatWidgetContainerHandler(this);
}

bool TabsChatWidgetContainerHandler::acceptChat(Chat chat) const
{
	if (!chat || !m_tabsManager)
		return false;

	return m_tabsManager.data()->acceptChat(chat);
}

ChatWidget * TabsChatWidgetContainerHandler::addChat(Chat chat, OpenChatActivation activation)
{
	if (!chat || !m_tabsManager)
		return nullptr;

	return m_tabsManager.data()->addChat(chat, activation);
}

void TabsChatWidgetContainerHandler::removeChat(Chat chat)
{
	if (!chat || !m_tabsManager)
		return;

	return m_tabsManager.data()->removeChat(chat);
}

bool TabsChatWidgetContainerHandler::isChatWidgetActive(ChatWidget *chatWidget)
{
	return m_tabsManager->tabWidget() ? m_tabsManager->tabWidget()->isChatWidgetActive(chatWidget) : false;
}

void TabsChatWidgetContainerHandler::tryActivateChatWidget(ChatWidget *chatWidget)
{
	if (m_tabsManager->tabWidget())
		m_tabsManager->tabWidget()->tryActivateChatWidget(chatWidget);
}

void TabsChatWidgetContainerHandler::tryMinimizeChatWidget(ChatWidget *chatWidget)
{
	if (m_tabsManager->tabWidget())
		m_tabsManager->tabWidget()->tryMinimizeChatWidget(chatWidget);
}
