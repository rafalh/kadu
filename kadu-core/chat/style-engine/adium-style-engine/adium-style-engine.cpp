/*
 * %kadu copyright begin%
 * Copyright 2012 Marcel Zięba (marseel@gmail.com)
 * Copyright 2009, 2010, 2010, 2011, 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "adium-style-engine.h"

#include "chat/chat-style.h"
#include "chat/html-messages-renderer.h"
#include "chat/style-engine/adium-style-engine/adium-chat-messages-renderer.h"
#include "message/message-html-renderer-service.h"
#include "misc/kadu-paths.h"
#include "misc/memory.h"

#include <QtCore/QDir>
#include <QtWebKit/QWebFrame>

RefreshViewHack::RefreshViewHack(QVector<Message> messages, AdiumChatMessagesRenderer *engine, QWebFrame *webFrame, QObject *parent) :
		QObject(parent), Messages(messages), Engine(engine), WebFrame(webFrame)
{
	connect(Engine, SIGNAL(destroyed()), this, SLOT(cancel()));
	connect(WebFrame, SIGNAL(destroyed()), this, SLOT(cancel()));
}

RefreshViewHack::~RefreshViewHack()
{
}

void RefreshViewHack::cancel()
{
	Engine = 0;
	WebFrame = 0;

	deleteLater();
}

void RefreshViewHack::loadFinished()
{
	if (!Engine || !WebFrame)
	{
		deleteLater();
		return;
	}

	emit finished(WebFrame);

	auto lastMessage = Message::null;
	for (auto const &message : Messages)
	{
		Engine->appendChatMessage(*WebFrame, message, lastMessage);
		lastMessage = message;
	}

	deleteLater();
}

AdiumStyleEngine::AdiumStyleEngine()
{
}

AdiumStyleEngine::~AdiumStyleEngine()
{
}

void AdiumStyleEngine::setMessageHtmlRendererService(MessageHtmlRendererService *messageHtmlRendererService)
{
	CurrentMessageHtmlRendererService = messageHtmlRendererService;
}

QString AdiumStyleEngine::isStyleValid(QString stylePath)
{
	return AdiumStyle::isStyleValid(stylePath) ? QDir(stylePath).dirName() : QString();
}

bool AdiumStyleEngine::styleUsesTransparencyByDefault(QString styleName)
{
	auto style = AdiumStyle{styleName};
	return style.defaultBackgroundIsTransparent();
}

QString AdiumStyleEngine::defaultVariant(const QString &styleName)
{
	auto style = AdiumStyle{styleName};
	return style.defaultVariant();
}

QStringList AdiumStyleEngine::styleVariants(QString styleName)
{
	QDir dir;
	QString styleBaseHref = KaduPaths::instance()->profilePath() + QLatin1String("syntax/chat/") + styleName + QLatin1String("/Contents/Resources/Variants/");
	if (!dir.exists(styleBaseHref))
		styleBaseHref = KaduPaths::instance()->dataPath() + QLatin1String("syntax/chat/") + styleName + QLatin1String("/Contents/Resources/Variants/");
	dir.setPath(styleBaseHref);
	dir.setNameFilters(QStringList("*.css"));
	return dir.entryList();
}

std::unique_ptr<ChatMessagesRenderer> AdiumStyleEngine::createRenderer(const ChatStyle &chatStyle)
{
	auto style = AdiumStyle{chatStyle.name()};
	style.setCurrentVariant(chatStyle.variant());

	auto result = make_unique<AdiumChatMessagesRenderer>(style);
	result.get()->setMessageHtmlRendererService(CurrentMessageHtmlRendererService);
	return std::move(result);
}

#include "moc_adium-style-engine.cpp"
