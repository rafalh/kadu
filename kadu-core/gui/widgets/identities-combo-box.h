/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef IDENTITIES_COMBO_BOX_H
#define IDENTITIES_COMBO_BOX_H

#include "gui/widgets/actions-combo-box.h"
#include "identities/identity.h"
#include "exports.h"

class KADUAPI IdentitiesComboBox : public ActionsComboBox
{
	Q_OBJECT

	bool InActivatedSlot;

	QAction *CreateNewIdentityAction;

private slots:
	void createNewIdentity();

public:
	explicit IdentitiesComboBox(bool includeSelectIdentity, QWidget *parent = 0);
	virtual ~IdentitiesComboBox();

	void setCurrentIdentity(Identity identity);
	Identity currentIdentity();

};

#endif // IDENTITIES_COMBO_BOX_H
