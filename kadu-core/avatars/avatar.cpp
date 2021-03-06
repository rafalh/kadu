/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "contacts/contact.h"
#include "misc/paths-provider.h"

#include "avatar.h"

KaduSharedBaseClassImpl(Avatar)

Avatar Avatar::null;

Avatar::Avatar()
{
}

Avatar::Avatar(AvatarShared *data) :
		SharedBase<AvatarShared>(data)
{
}

Avatar::Avatar(QObject *data)
{
	AvatarShared *shared = qobject_cast<AvatarShared *>(data);
	if (shared)
		setData(shared);
}

Avatar::Avatar(const Avatar &copy) :
		SharedBase<AvatarShared>(copy)
{
}

Avatar::~Avatar()
{
}

void Avatar::storeAvatar()
{
	if (!isNull())
		data()->storeAvatar();
}

KaduSharedBase_PropertyBoolReadDef(Avatar, Empty, true)
KaduSharedBase_PropertyDefCRW(Avatar, QDateTime, lastUpdated, LastUpdated, QDateTime())
KaduSharedBase_PropertyDefCRW(Avatar, QDateTime, nextUpdate, NextUpdate, QDateTime())
KaduSharedBase_PropertyDefCRW(Avatar, QPixmap, pixmap, Pixmap, QPixmap())
KaduSharedBase_PropertyReadDef(Avatar, QString, smallFilePath, SmallFilePath, QString())
KaduSharedBase_PropertyDefCRW(Avatar, QString, filePath, FilePath, QString())
