/*
 * %kadu copyright begin%
 * Copyright 2012, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QMetaType>
#include <QtCore/QString>

#include "exports.h"

class KADUAPI ChatImage
{

public:
	ChatImage();
	ChatImage(QString key, quint32 size);

	bool operator == (const ChatImage &compareTo) const;
	bool operator < (const ChatImage &compareTo) const;

	bool isNull() const;

	QString key() const;
	quint32 size() const;

private:
	QString m_key;
	quint32 m_size;

};

Q_DECLARE_METATYPE(ChatImage);
