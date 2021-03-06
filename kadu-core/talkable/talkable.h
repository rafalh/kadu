/*
 * %kadu copyright begin%
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

#pragma once

#include "buddies/buddy.h"
#include "chat/chat.h"
#include "contacts/contact.h"
#include "exports.h"

/**
 * @addtogroup Talkable
 * @{
 */

/**
 * @class Talkable
 * @author Rafał 'Vogel' Malinowski
 * @short Class representing a talkable entity in Kadu - Chat, Buddy or Contact.
 *
 * Objects of this class can contain instance of one of talkable types - Chat, Buddy or Contact.
 * Empty Talkable objects does not contain anything.
 *
 * Talkable contructors can take an object of Chat, Buddy or Contact type. Getter methods toChat() toBuddy()
 * and toContact() can convert object contained in Talkable from one type to another. For example, if Talkable
 * was created with Contact object, then toBuddy() getter will return ownerBuddy() of given Contact. Check
 * documentation of getters for more information about possible conversions.
 */
class KADUAPI Talkable
{
public:
	/**
	 * @enum ItemType
	 * @author Rafał 'Vogel' Malinowski
	 * @short Enum describing content of Talkable object.
	 */
	enum ItemType
	{
		/**
		 * Talkable object does not contain anything.
		 */
		ItemNone,
		/**
		 * Talkable object does contains instance of Buddy object.
		 */
		ItemBuddy
		/**
		 * Talkable object does contains instance of Contact object.
		 */,
		ItemContact,
		/**
		 * Talkable object does contains instance of Chat object.
		 */
		ItemChat
	};

private:
	ItemType Type;
	Buddy MyBuddy;
	Contact MyContact;
	Chat MyChat;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates empty Talkable object.
	 */
	Talkable();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates Talkable object that contains Buddy instance.
	 * @param buddy this Buddy instance will be hold by new Talkable object
	 */
	Talkable(const Buddy &buddy);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates Talkable object that contains Contact instance.
	 * @param contact this Contact instance will be hold by new Talkable object
	 */
	Talkable(const Contact &contact);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates Talkable object that contains Chat instance.
	 * @param chat this Chat instance will be hold by new Talkable object
	 */
	Talkable(const Chat &chat);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Copy construtor.
	 * @param copyMe Talkable instance to copy
	 */
	Talkable(const Talkable &copyMe);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Assignment operator.
	 * @param copyMe Talkable instance to copy
	 *
	 * Copies value of copyMe into this Talkable instance.
	 */
	Talkable & operator = (const Talkable &copyMe);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Equality operator.
	 * @param compareTo Talkable instance to compare
	 *
	 * Two Talkable instances are equal when their content objects are of equal type and value.
	 */
	bool operator == (const Talkable &compareTo) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Non-equality operator.
	 * @param compareTo Talkable instance to compare
	 *
	 * Two Talkable instances are non equal when their content objects are of different types
	 * or values.
	 */
	bool operator != (const Talkable &compareTo) const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns type of Talkable instance.
	 * @return type of Talkable instance
	 */
	ItemType type() const { return Type; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns true if this Talkable instance does not contains non-null object.
	 * @return true if this Talkable instance does not contains non-null object
	 *
	 * True is returned if type of this Talkable instance is ItemNone or if contained object is null.
	 */
	bool isEmpty() const;

	Buddy buddy() const;

	Chat chat() const;

	Contact contact() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns true if Talkable is not-null Chat.
	 * @return true if current item is not-null Chat
	 */
	bool isValidChat() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns true if Talkable is not-null Buddy.
	 * @return true if current item is not-null Buddy
	 */
	bool isValidBuddy() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns true if Talkable is not-null Contact.
	 * @return true if current item is not-null Contact
	 */
	bool isValidContact() const;

};

Q_DECLARE_METATYPE(Talkable)

/**
 * @}
 */
