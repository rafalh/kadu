/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddy-data-extractor.h"

#include "avatars/avatar.h"
#include "buddies/buddy-preferred-manager.h"
#include "buddies/buddy.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "model/roles.h"
#include "talkable/talkable.h"

#include <QtCore/QVariant>
#include <QtGui/QIcon>

BuddyDataExtractor::BuddyDataExtractor(QObject *parent) :
		QObject{parent}
{
}

BuddyDataExtractor::~BuddyDataExtractor()
{
}

void BuddyDataExtractor::setBuddyPreferredManager(BuddyPreferredManager *buddyPreferredManager)
{
	m_buddyPreferredManager = buddyPreferredManager;
}

void BuddyDataExtractor::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

QVariant BuddyDataExtractor::decoration(const Buddy &buddy)
{
	if (!buddy.homePhone().isEmpty() || !buddy.mobile().isEmpty())
		return m_iconsManager->iconByPath(KaduIcon("phone"));
	else if (!buddy.email().isEmpty())
		return m_iconsManager->iconByPath(KaduIcon("mail-message-new"));
	else
		return m_iconsManager->iconByPath(KaduIcon("system-users"));
}

QVariant BuddyDataExtractor::data(const Buddy &buddy, int role)
{
	switch (role)
	{
		case Qt::DisplayRole:
			return buddy.display();
		case Qt::DecorationRole:
			return decoration(buddy);
		case BuddyRole:
			return QVariant::fromValue(buddy);
		case AvatarRole:
			return buddy.buddyAvatar().pixmap();
		case StatusRole:
			return QVariant::fromValue(m_buddyPreferredManager->preferredContact(buddy, false).currentStatus());
		case ItemTypeRole:
			return BuddyRole;
		case TalkableRole:
			return QVariant::fromValue(Talkable(buddy));
		default:
			return QVariant();
	}
}
