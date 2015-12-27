/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "cenzor-message-filter.h"

#include "notification/cenzor-notification.h"

#include "message/message-manager.h"

CenzorMessageFilter::CenzorMessageFilter(QObject *parent) :
		QObject{parent}
{
}

CenzorMessageFilter::~CenzorMessageFilter()
{
}

bool CenzorMessageFilter::acceptMessage(const Message &message)
{
	if (MessageTypeSent == message.type())
		return true;

	if (!Configuration.enabled())
		return true;

	if (!shouldIgnore(message.plainTextContent()))
		return true;

	Account account = message.messageChat().chatAccount();

	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return false;


	if (MessageManager::instance()->sendMessage(message.messageChat(), Configuration.admonition(), true))
		CenzorNotification::notifyCenzored(message.messageChat());

	return false;
}

bool CenzorMessageFilter::shouldIgnore(const QString &message)
{
	QStringList words = message.split(' ', QString::SkipEmptyParts);

	for (const QString &word : words)
	{
		QString lowerWord = word.toLower();
		for (const QRegExp &swear : Configuration.swearList())
			if ((swear.indexIn(lowerWord) >= 0) && (!isExclusion(lowerWord)))
				return true;
	}

	return false;
}

bool CenzorMessageFilter::isExclusion(const QString &word)
{
	for (const QRegExp &exclusion : Configuration.exclusionList())
		if (exclusion.indexIn(word) >= 0)
			return true;

	return false;
}

#include "moc_cenzor-message-filter.cpp"