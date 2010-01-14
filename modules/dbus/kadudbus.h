/*
 * %kadu copyright begin%
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef KADU_DBUS_H
#define KADU_DBUS_H

#include <QtCore/QObject>
#include <QtDBus/QDBusConnection>

class KaduDBus : public QObject
{
	Q_OBJECT

	QDBusConnection connection;

	public:
		KaduDBus();
		~KaduDBus();
};

extern KaduDBus* dbus;

#endif // KADU_DBUS_H
