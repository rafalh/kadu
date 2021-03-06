/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class QVariant;

class ChatTypeManager;
class Chat;
class IconsManager;

class KADUAPI ChatDataExtractor : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ChatDataExtractor(QObject *parent = nullptr);
	virtual ~ChatDataExtractor();

	QVariant data(const Chat &chat, int role);

private:
	QPointer<ChatTypeManager> m_chatTypeManager;
	QPointer<IconsManager> m_iconsManager;

private slots:
	INJEQT_SET void setChatTypeManager(ChatTypeManager *chatTypeManager);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);

};
