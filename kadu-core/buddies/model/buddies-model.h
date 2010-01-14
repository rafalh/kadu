/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef BUDDIES_MODEL_H
#define BUDDIES_MODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>

#include "accounts/account.h"
#include "accounts/accounts-aware-object.h"
#include "buddies/buddy.h"
#include "status/status.h"

#include "buddies-model-base.h"

class BuddyManager;

class BuddiesModel : public BuddiesModelBase
{
	Q_OBJECT

	BuddyManager *Manager;

private slots:
	void buddyAboutToBeAdded(Buddy &buddy);
	void buddyAdded(Buddy &buddy);
	void buddyAboutToBeRemoved(Buddy &buddy);
	void buddyRemoved(Buddy &buddy);

	void buddyUpdated();

	void contactAboutToBeAdded(Contact contact);
	void contactAdded(Contact contact);
	void contactAboutToBeRemoved(Contact contact);
	void contactRemoved(Contact contact);

	void contactUpdated();

public:
	explicit BuddiesModel(BuddyManager *manager, QObject *parent = 0);
	~BuddiesModel();

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	
	// AbstractContactsModel implementation
	virtual Buddy buddyAt(const QModelIndex &index) const;
	virtual const QModelIndex buddyIndex(Buddy buddy) const;

};

#endif // BUDDIES_MODEL_H
