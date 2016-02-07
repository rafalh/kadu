/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QTimer>

#include "core/core.h"
#include "icons/kadu-icon.h"
#include "identities/identity.h"
#include "notification/notifier.h"
#include "parser/parser.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "debug.h"

#include "notification.h"

static QString getAccountName(const ParserData * const object)
{
	auto notification = dynamic_cast<const Notification * const>(object);
	if (!notification)
		return QString{};

	auto account = notification->data()["account"].value<Account>();
	return account.accountIdentity().name();
}

static QString getProtocolName(const ParserData * const object)
{
	auto notification = dynamic_cast<const Notification * const>(object);
	if (!notification)
		return QString{};

	auto account = notification->data()["account"].value<Account>();
	return account.protocolHandler() && account.protocolHandler()->protocolFactory()
		? account.protocolHandler()->protocolFactory()->displayName()
		: QString{};
}

static QString getNotificationTitle(const ParserData * const object)
{
	kdebugf();

	const Notification * const notification = dynamic_cast<const Notification * const>(object);
	if (notification)
		return notification->title();
	else
		return QString();
}

void Notification::registerParserTags(Parser *parser)
{
	parser->registerObjectTag("account", getAccountName);
	parser->registerObjectTag("event", getNotificationTitle);
	parser->registerObjectTag("protocol", getProtocolName);
}

void Notification::unregisterParserTags(Parser *parser)
{
	parser->unregisterObjectTag("account");
	parser->unregisterObjectTag("event");
	parser->unregisterObjectTag("protocol");
}

Notification::Notification(QVariantMap data, const QString &type, const KaduIcon &icon) :
		m_data{std::move(data)},
		Type(type),
		Icon(icon)
{
}

Notification::Notification(Account account, Chat chat, const QString &type, const KaduIcon &icon) :
		Closing(false),
		Type(type),
		Icon(icon),
		m_account{account},
		m_chat{chat}
{
	m_data.insert("account", qVariantFromValue(account));
	m_data.insert("chat", qVariantFromValue(chat));
}

Notification::~Notification()
{
}

const QVariantMap & Notification::data() const
{
	return m_data;
}

void Notification::acquire(Notifier *notifier)
{
	kdebugf();

	Notifiers.insert(notifier);
}

void Notification::release(Notifier *notifier)
{
	kdebugf();

	Notifiers.remove(notifier);

	if (Notifiers.size() <= 0)
		close();
}

void Notification::close()
{
	kdebugf();

	if (!Closing)
	{
		Closing = true;
		emit closed(this);
		deleteLater();
	}
}

void Notification::clearCallbacks()
{
	Callbacks.clear();
}

void Notification::addCallback(const QString &name)
{
	Callbacks.append(name);
}

void Notification::addChatCallbacks()
{
	addCallback("chat-open");
	addCallback("ignore");
}

void Notification::setAcceptCallback(QString acceptCallback)
{
	m_acceptCallback = std::move(acceptCallback);
}

QString Notification::acceptCallback() const
{
	return m_acceptCallback;
}

void Notification::setDiscardCallback(QString discardCallback)
{
	m_discardCallback = std::move(discardCallback);
}

QString Notification::discardCallback() const
{
	return m_discardCallback;
}

void Notification::setTitle(const QString &title)
{
	Title = title;
}

void Notification::setText(const QString &text)
{
	Text = text;
}

void Notification::setDetails(const QStringList &details)
{
	Details = details;
}

void Notification::setDetails(const QString &details)
{
	Details = QStringList(details);
}

void Notification::setIcon(const KaduIcon &icon)
{
	Icon = icon;
}

#include "moc_notification.cpp"
