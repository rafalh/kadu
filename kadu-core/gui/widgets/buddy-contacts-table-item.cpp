/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddy-contacts-table-item.h"

BuddyContactsTableItem::BuddyContactsTableItem(Contact contact)
{
	ItemContact = contact;
	ItemAccount = contact.contactAccount();
	Id = contact.id();
	Action = ItemEdit;
}
