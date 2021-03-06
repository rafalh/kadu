/*
 * %kadu copyright begin%
 * Copyright 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/widgets/kadu-web-view.h"

#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "misc/memory.h"
#include "os/generic/compositing-aware-object.h"
#include "protocols/services/chat-state-service.h"
#include "exports.h"

#include <injeqt/injeqt.h>

class ChatConfigurationHolder;
class ChatImage;
class ChatImageRequestService;
class ChatStyleManager;
class ChatStyleRendererConfiguration;
class ChatStyleRendererFactory;
class InjectedFactory;
class Message;
class PathsProvider;
class WebkitMessagesViewHandler;
class WebkitMessagesViewHandlerFactory;
class SortedMessages;

class KADUAPI WebkitMessagesView : public KaduWebView, public ConfigurationAwareObject, CompositingAwareObject
{
	Q_OBJECT

public:
	explicit WebkitMessagesView(const Chat &chat = Chat::null, bool supportTransparency = true, QWidget *parent = nullptr);
	virtual ~WebkitMessagesView();

	void add(const Message &message);
	void add(const SortedMessages &messages);
	SortedMessages messages() const;

	int countMessages();

	void setForcePruneDisabled(bool disable);

	Chat chat() const { return m_chat; }
	void setChat(const Chat &chat);

	bool supportTransparency() { return m_supportTransparency; }

	void setWebkitMessagesViewHandlerFactory(WebkitMessagesViewHandlerFactory *webkitMessagesViewHandlerFactory);

public slots:
	void setChatStyleRendererFactory(std::shared_ptr<ChatStyleRendererFactory> chatStyleRendererFactory);
	void refreshView();

	void clearMessages();
	void contactActivityChanged(const Contact &contact, ChatState state);
	void updateAtBottom();

	void pageUp();
	void pageDown();
	void scrollToTop();
	void scrollToBottom();
	void forceScrollToBottom();

signals:
	void messagesUpdated();

protected:
	virtual void configurationUpdated() override;
	virtual void mouseReleaseEvent(QMouseEvent *e) override;
	virtual void resizeEvent(QResizeEvent *e) override;
	virtual void wheelEvent(QWheelEvent *e) override;

	virtual void compositingEnabled() override;
	virtual void compositingDisabled() override;

private:
	QPointer<ChatConfigurationHolder> m_chatConfigurationHolder;
	QPointer<ChatImageRequestService> m_chatImageRequestService;
	QPointer<ChatStyleManager> m_chatStyleManager;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<PathsProvider> m_pathsProvider;
	QPointer<WebkitMessagesViewHandlerFactory> m_webkitMessagesViewHandlerFactory;

	Chat m_chat;
	owned_qptr<WebkitMessagesViewHandler> m_handler;
	bool m_forcePruneDisabled;
	std::shared_ptr<ChatStyleRendererFactory> m_chatStyleRendererFactory;

	bool m_supportTransparency;
	bool m_atBottom;

	void connectChat();
	void disconnectChat();

	ChatStyleRendererConfiguration rendererConfiguration();
	void setWebkitMessagesViewHandler(owned_qptr<WebkitMessagesViewHandler> handler);

private slots:
	INJEQT_SET void setChatConfigurationHolder(ChatConfigurationHolder *chatConfigurationHolder);
	INJEQT_SET void setChatImageRequestService(ChatImageRequestService *chatImageRequestService);
	INJEQT_SET void setChatStyleManager(ChatStyleManager *chatStyleManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_INIT void init();

	void chatImageStored(const ChatImage &chatImage, const QString &fullFilePath);
	void sentMessageStatusChanged(const Message &message);
	void chatStyleConfigurationUpdated();

};
