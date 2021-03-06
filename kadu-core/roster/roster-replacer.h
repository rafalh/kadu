/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Account;
class BuddyList;
class BuddyManager;
class Buddy;
class ContactManager;
class Contact;
class IconsManager;
class Myself;
class Roster;

/**
 * @addtogroup Roster
 * @{
 */

/**
 * @class RosterReplacer
 * @short Class to replace whole roster for an account
 *
 * This comes from old times. At some time this class must be understood and probably just
 * removed.
 */

class KADUAPI RosterReplacer : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit RosterReplacer(QObject *parent = nullptr);
	virtual ~RosterReplacer();

	QPair<QList<Contact>, QList<Contact>> replaceRoster(Account account, const BuddyList &buddies, bool ask);

private:
	QPointer<BuddyManager> m_buddyManager;
	QPointer<ContactManager> m_contactManager;
	QPointer<IconsManager> m_iconsManager;
	QPointer<Myself> m_myself;
	QPointer<Roster> m_roster;

	bool askForAddingContacts(const QMap<Buddy, Contact> &contactsToAdd, const QMap<Buddy, Contact> &contactsToRename);
	QList<Contact> performAdds(const QMap<Buddy, Contact> &contactsToAdd);
	void performRenames(const QMap<Buddy, Contact> &contactsToRename);
	QPair<QList<Contact>, QList<Contact>> registerBuddies(Account account, const BuddyList &buddies, bool ask);
	void copySupportedBuddyInformation(const Buddy &destination, const Buddy &source);

private slots:
	INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
	INJEQT_SET void setContactManager(ContactManager *contactManager);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setMyself(Myself *myself);
	INJEQT_SET void setRoster(Roster *roster);

};

/**
 * @}
 */
