/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy-preferred-manager.h"
#include "chat/chat-manager.h"
#include "chat/message/pending-messages-manager.h"
#include "model/roles.h"

#include "model-index-list-converter.h"

ModelIndexListConverter::ModelIndexListConverter(const QModelIndexList &modelIndexList) :
		ModelIndexList(modelIndexList)
{
	buildRoles();
	buildBuddies();
	buildContacts();
	buildChat();
}

RoleSet ModelIndexListConverter::roles() const
{
	return Roles;
}

BuddySet ModelIndexListConverter::buddies() const
{
	return Buddies;
}

ContactSet ModelIndexListConverter::contacts() const
{
	return Contacts;
}

Chat ModelIndexListConverter::chat() const
{
	return ComputedChat;
}

void ModelIndexListConverter::buildRoles()
{
	foreach (const QModelIndex &selection, ModelIndexList)
		Roles.insert(selection.data(ItemTypeRole).toInt());
}

void ModelIndexListConverter::buildBuddies()
{
	foreach (const QModelIndex &index, ModelIndexList)
	{
		const Buddy &buddy = index.data(BuddyRole).value<Buddy>();
		if (buddy)
			Buddies.insert(buddy);
	}
}

void ModelIndexListConverter::buildContacts()
{
	foreach (const QModelIndex &index, ModelIndexList)
	{
		const Contact &contact = index.data(ContactRole).value<Contact>();
		if (contact)
			Contacts.insert(contact);
	}
}

Chat ModelIndexListConverter::chatFromIndex(const QModelIndex &index) const
{
	switch (index.data(ItemTypeRole).toInt())
	{
		case ChatRole:
			return index.data(ChatRole).value<Chat>();
		case BuddyRole:
			return PendingMessagesManager::instance()->chatForBuddy(index.data(BuddyRole).value<Buddy>());
		case ContactRole:
			return PendingMessagesManager::instance()->chatForContact(index.data(ContactRole).value<Contact>());
	}

	return Chat::null;
}

Chat ModelIndexListConverter::chatFromBuddies() const
{
	BuddySet buddies;
	foreach (const QModelIndex &index, ModelIndexList)
		buddies.insert(index.data(BuddyRole).value<Buddy>());

	return ChatManager::instance()->findChat(buddies, true);
}

Chat ModelIndexListConverter::chatFromContacts(const Account &account) const
{
	if (!account)
		return Chat::null;

	ContactSet contacts;
	foreach (const QModelIndex &index, ModelIndexList)
	{
		Contact contact = contactForAccount(index, account);
		if (!contact)
			return Chat::null;

		contacts.insert(contact);
	}

	return ChatManager::instance()->findChat(contacts, true);
}

Account ModelIndexListConverter::commonAccount() const
{
	foreach (const QModelIndex &index, ModelIndexList)
	{
		if (index.data(ItemTypeRole) == ContactRole)
		{
			const Contact &contact = index.data(ContactRole).value<Contact>();
			if (contact)
				return contact.contactAccount();
		}
	}

	return Account::null;
}

Contact ModelIndexListConverter::contactForAccount(const QModelIndex &index, const Account &account) const
{
	if (index.data(ItemTypeRole) == BuddyRole)
		return BuddyPreferredManager::instance()->preferredContact(index.data(BuddyRole).value<Buddy>(), account);

	const Contact &contact = index.data(ContactRole).value<Contact>();
	if (contact.contactAccount() == account)
		return contact;

	return Contact::null;
}

void ModelIndexListConverter::buildChat()
{
	if (ModelIndexList.size() == 1)
	{
		ComputedChat = chatFromIndex(ModelIndexList.at(0));
		if (ComputedChat)
			return;
	}

	// more than one item selected and a Chat selected
	// we can not return one chat in this situation
	if (Roles.contains(ChatRole))
		return;

	// only buddies selected
	if (Roles.contains(BuddyRole) && Roles.size() == 1)
	{
		ComputedChat = chatFromBuddies();
		return;
	}

	// buddies and contacts selected
	if (Roles.contains(ContactRole))
		ComputedChat = chatFromContacts(commonAccount());
}