/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-widget-message-handler.h"

#include "chat/buddy-chat-manager.h"
#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/windows/kadu-window.h"
#include "message/message-manager.h"
#include "message/unread-message-repository.h"
#include "protocols/protocol.h"
#include "services/notification-service.h"

#include <QtGui/QApplication>

ChatWidgetMessageHandler::ChatWidgetMessageHandler(QObject *parent) :
		QObject(parent)
{
}

ChatWidgetMessageHandler::~ChatWidgetMessageHandler()
{
}

void ChatWidgetMessageHandler::setBuddyChatManager(BuddyChatManager *buddyChatManager)
{
	m_buddyChatManager = buddyChatManager;
}

void ChatWidgetMessageHandler::setChatWidgetManager(ChatWidgetManager *chatWidgetManager)
{
	m_chatWidgetManager = chatWidgetManager;
}

void ChatWidgetMessageHandler::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;

	if (!m_chatWidgetRepository)
		return;

	connect(m_chatWidgetRepository.data(), SIGNAL(chatWidgetAdded(ChatWidget*)), this, SLOT(chatWidgetAdded(ChatWidget*)));
	connect(m_chatWidgetRepository.data(), SIGNAL(chatWidgetRemoved(ChatWidget*)), this, SLOT(chatWidgetRemoved(ChatWidget*)));

	foreach (auto chatWidget, m_chatWidgetRepository.data()->widgets())
		chatWidgetAdded(chatWidget);
}

void ChatWidgetMessageHandler::setMessageManager(MessageManager *messageManager)
{
	m_messageManager = messageManager;

	if (!m_messageManager)
		return;

	connect(m_messageManager.data(), SIGNAL(messageReceived(Message)), this, SLOT(messageReceived(Message)));
	connect(m_messageManager.data(), SIGNAL(messageSent(Message)), this, SLOT(messageSent(Message)));
}

void ChatWidgetMessageHandler::setNotificationService(NotificationService *notificationService)
{
	m_notificationService = notificationService;
}

void ChatWidgetMessageHandler::setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository)
{
	m_unreadMessageRepository = unreadMessageRepository;
}

void ChatWidgetMessageHandler::setConfiguration(ChatWidgetMessageHandlerConfiguration configuration)
{
	m_configuration = configuration;
}

void ChatWidgetMessageHandler::chatWidgetAdded(ChatWidget *chatWidget)
{
	connect(chatWidget, SIGNAL(activated(ChatWidget*)), this, SLOT(chatWidgetActivated(ChatWidget*)));
}

void ChatWidgetMessageHandler::chatWidgetRemoved(ChatWidget *chatWidget)
{
	auto chat = chatWidget->chat();
	chat.removeProperty("message:unreadMessagesAppended");

	disconnect(chatWidget, SIGNAL(activated(ChatWidget*)), this, SLOT(chatWidgetActivated(ChatWidget*)));
}

void ChatWidgetMessageHandler::chatWidgetActivated(ChatWidget *chatWidget)
{
	if (!m_unreadMessageRepository)
		return;

	auto chat = chatWidget->chat();
	auto unreadMessagesAppended = chat.property("message:unreadMessagesAppended", false).toBool();

	auto messages = unreadMessagesAppended ? m_unreadMessageRepository.data()->unreadMessagesForChat(chat) : loadAllUnreadMessages(chat);
	m_unreadMessageRepository.data()->markMessagesAsRead(messages);

	if (!unreadMessagesAppended)
	{
		chatWidget->appendMessages(messages);
		chat.addProperty("message:unreadMessagesAppended", true, CustomProperties::NonStorable);
	}
}

QVector<Message> ChatWidgetMessageHandler::loadAllUnreadMessages(const Chat &chat) const
{
	auto buddyChat = m_buddyChatManager ? m_buddyChatManager.data()->buddyChat(chat) : Chat::null;
	auto unreadChat = buddyChat ? buddyChat : chat;
	return m_unreadMessageRepository.data()->unreadMessagesForChat(unreadChat);
}

void ChatWidgetMessageHandler::messageReceived(const Message &message)
{
	if (!m_chatWidgetRepository)
		return;

	auto chat = message.messageChat();
	auto chatWidget = m_chatWidgetRepository.data()->widgetForChat(chat);
	if (chatWidget)
	{
		chatWidget->appendMessage(message);
		return;
	}

	if (shouldOpenChatWidget(chat))
		m_chatWidgetManager.data()->openChat(chat, OpenChatActivation::Activate);
	else
		qApp->alert(Core::instance()->kaduWindow());
}

bool ChatWidgetMessageHandler::shouldOpenChatWidget(const Chat &chat) const
{
	if (!m_configuration.openChatOnMessage())
		return false;

	auto silentMode = m_notificationService ? m_notificationService.data()->silentMode() : false;
	if ((m_configuration.openChatOnMessage() || m_configuration.openChatOnMessageOnlyWhenOnline()) && silentMode)
		return false;

	auto handler = chat.chatAccount().protocolHandler();
	if (!handler)
		return false;

	if (m_configuration.openChatOnMessageOnlyWhenOnline())
		return StatusTypeGroupOnline == handler->status().group();
	else
		return true;
}

void ChatWidgetMessageHandler::messageSent(const Message &message)
{
	if (!m_chatWidgetRepository)
		return;

	auto chat = message.messageChat();
	auto chatWidget = m_chatWidgetRepository.data()->widgetForChat(chat);
	if (chatWidget)
		chatWidget->appendMessage(message);
}

#include "moc_chat-widget-message-handler.cpp"
