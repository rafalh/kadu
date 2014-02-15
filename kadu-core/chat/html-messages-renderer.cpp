/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebPage>

#include "chat/chat-styles-manager.h"
#include "chat/style-engines/chat-messages-renderer.h"
#include "configuration/chat-configuration-holder.h"
#include "configuration/configuration-file.h"

#include "html-messages-renderer.h"

HtmlMessagesRenderer::HtmlMessagesRenderer(const Chat &chat, QObject *parent) :
		QObject(parent), MyChat(chat), PruneEnabled(true), ForcePruneDisabled(false)
{
	MyWebPage = new QWebPage(this);
	MyWebPage->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
}

HtmlMessagesRenderer::~HtmlMessagesRenderer()
{
	MyChatMessages.clear();
}

void HtmlMessagesRenderer::setChat(const Chat &chat)
{
	MyChat = chat;
}

void HtmlMessagesRenderer::setForcePruneDisabled(bool forcePruneDisabled)
{
	ForcePruneDisabled = forcePruneDisabled;
	pruneMessages();
}

QString HtmlMessagesRenderer::content()
{
	return MyWebPage->mainFrame()->toHtml();
}

bool HtmlMessagesRenderer::pruneEnabled()
{
	return !ForcePruneDisabled && PruneEnabled;
}

void HtmlMessagesRenderer::pruneMessages()
{
	if (ForcePruneDisabled)
		return;

	if (ChatStylesManager::instance()->prune() == 0)
	{
		PruneEnabled = false;
		return;
	}

	if (MyChatMessages.count() <= ChatStylesManager::instance()->prune())
	{
		PruneEnabled = false;
		return;
	}

	PruneEnabled = true;

	auto start = MyChatMessages.begin();
	auto stop = MyChatMessages.end() - ChatStylesManager::instance()->prune();
	for (auto it = start; it != stop; ++it)
		ChatStylesManager::instance()->currentRenderer()->pruneMessage(this);

	MyChatMessages.erase(start, stop);
}

void HtmlMessagesRenderer::appendMessage(const Message &message)
{
	MyChatMessages.append(message);
	pruneMessages();

	ChatStylesManager::instance()->currentRenderer()->appendMessage(this, message);
}

void HtmlMessagesRenderer::appendMessages(const QVector<Message> &messages)
{
	auto engineMessages = QVector<Message>{};
	for (auto message : messages)
	{
		engineMessages.append(message);
		MyChatMessages.append(message);
	}

//  Do not prune messages here. When we are adding many massages to renderer, probably
//  we want all of them to be visible on message view. This also fixes crash from
//  bug #1963 . This crash occured, when we are trying to
//  cite more messages from history, than our message pruning setting
//	pruneMessages();

	ChatStylesManager::instance()->currentRenderer()->appendMessages(this, engineMessages);
}

void HtmlMessagesRenderer::clearMessages()
{
	qDeleteAll(MyChatMessages);
	MyChatMessages.clear();

	LastMessage = Message::null;
	ChatStylesManager::instance()->currentRenderer()->clearMessages(this);
}

void HtmlMessagesRenderer::setLastMessage(Message message)
{
	LastMessage = message;
}

void HtmlMessagesRenderer::refresh()
{
	ChatStylesManager::instance()->currentRenderer()->refreshView(this);
}

void HtmlMessagesRenderer::chatImageAvailable(const ChatImage &chatImage, const QString &fileName)
{
	ChatStylesManager::instance()->currentRenderer()->chatImageAvailable(this, chatImage, fileName);
}

void HtmlMessagesRenderer::messageStatusChanged(Message message, MessageStatus status)
{
	ChatStylesManager::instance()->currentRenderer()->messageStatusChanged(this, message, status);
}

void HtmlMessagesRenderer::contactActivityChanged(const Contact &contact, ChatStateService::State state)
{
	QString display = contact.display(true);
	QString message;
	switch (state)
	{
		case ChatStateService::StateActive:
			message = tr("%1 is active").arg(display);
			break;
		case ChatStateService::StateComposing:
			message = tr("%1 is composing...").arg(display);
			break;
		case ChatStateService::StateGone:
			message = tr("%1 is gone").arg(display);
			break;
		case ChatStateService::StateInactive:
			message = tr("%1 is inactive").arg(display);
			break;
		case ChatStateService::StateNone:
			break;
		case ChatStateService::StatePaused:
			message = tr("%1 has paused composing").arg(display);
			break;
	}
	ChatStylesManager::instance()->currentRenderer()->contactActivityChanged(this, state, message, display);
}

#include "moc_html-messages-renderer.cpp"
