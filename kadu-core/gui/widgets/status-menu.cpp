/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QMenu>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/windows/choose-description.h"
#include "protocols/protocol.h"
#include "status/status-actions.h"
#include "status/status-group.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"

#include "status-menu.h"

StatusMenu::StatusMenu(StatusContainer *statusContainer, QMenu *menu) :
		QObject(menu), Menu(menu), MyStatusContainer(statusContainer)
{
	Actions = new StatusActions(MyStatusContainer, this);

	connect(Actions, SIGNAL(statusActionTriggered(QAction *)), this, SLOT(changeStatus(QAction *)));
	connect(Actions, SIGNAL(changeDescriptionActionTriggered(bool)), this, SLOT(changeDescription()));
	connect(Actions, SIGNAL(changePrivateStatusActionTriggered(bool)), this, SLOT(changeStatusPrivate(bool)));

// 	connect(MyStatusContainer, SIGNAL(updated()), this, SLOT(statusContainerUpdated()));

	connect(Menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHide()));

	foreach (QAction *action, Actions->actions())
		Menu->addAction(action);
}

StatusMenu::~StatusMenu()
{
}

void StatusMenu::aboutToHide()
{
	MousePositionBeforeMenuHide = Menu->pos();
}

void StatusMenu::changeStatus(QAction *action)
{
	StatusType *statusType = action->data().value<StatusType *>();
	if (!statusType)
		return;

	Status status(MyStatusContainer->status());
	status.setType(statusType->name());
	MyStatusContainer->setStatus(status);
}

void StatusMenu::changeDescription()
{
	ChooseDescription::showDialog(MyStatusContainer, MousePositionBeforeMenuHide);
}

void StatusMenu::changeStatusPrivate(bool toggled)
{
	if (AccountManager::instance()->defaultAccount().protocolHandler())
		AccountManager::instance()->defaultAccount().protocolHandler()->setPrivateMode(toggled);

	config_file.writeEntry("General", "PrivateStatus", toggled);
}
