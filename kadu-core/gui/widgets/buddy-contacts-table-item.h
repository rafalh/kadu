/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account.h"
#include "contacts/contact.h"

#include <QtCore/QPointer>

class BuddyContactsTableItem : public QObject
{
	Q_OBJECT

public:
	enum ItemAction {
		ItemEdit,
		ItemAdd,
		ItemDetach,
		ItemRemove
	};

private:
	QPointer<ContactManager> m_contactManager;

	Contact ItemContact;
	int ItemContactPriority;
	Account ItemAccount;
	QString Id;
	bool RosterDetached;
	ItemAction Action;
	QString DetachedBuddyName;

	bool isAddValid() const;
	bool isEditValid() const;

public:
	explicit BuddyContactsTableItem(ContactManager *contactManager, Contact contact = Contact::null, QObject *parent = nullptr);
	virtual ~BuddyContactsTableItem() {}

	Contact itemContact() const { return ItemContact; }

	int itemContactPriority() const { return ItemContactPriority; }
	void setItemContactPriority(int itemContactPriority);

	Account itemAccount() const { return ItemAccount; }
	void setItemAccount(Account account);

	const QString & id() const { return Id; }
	void setId(const QString &id);

	bool rosterDetached() const { return RosterDetached; }
	void setRosterDetached(bool rosterDetached);

	ItemAction action() const { return Action; }
	void setAction(ItemAction action);

	const QString & detachedBuddyName() const { return DetachedBuddyName; }
	void setDetachedBuddyName(const QString &detachedBuddyName);

	bool isValid() const;

signals:
	void updated(BuddyContactsTableItem *item);

};

Q_DECLARE_METATYPE(BuddyContactsTableItem *)
