/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "core/injected-factory.h"
#include "gui/actions/actions.h"
#include "gui/actions/action.h"
#include "gui/windows/add-conference-window.h"

#include "add-conference-action.h"

AddConferenceAction::AddConferenceAction(QObject *parent) :
		ActionDescription(parent)
{
}

AddConferenceAction::~AddConferenceAction()
{
}

void AddConferenceAction::setAccountManager(AccountManager *accountManager)
{
	m_accountManager = accountManager;
}

void AddConferenceAction::init()
{
	setType(ActionDescription::TypeGlobal);
	setName("addConferenceAction");
	setText(tr("Add Conference..."));

	connect(m_accountManager, SIGNAL(accountRegistered(Account)),
	        this, SLOT(updateAddChatMenuItem()));
	connect(m_accountManager, SIGNAL(accountUnregistered(Account)),
	        this, SLOT(updateAddChatMenuItem()));

	updateAddChatMenuItem();
}

void AddConferenceAction::triggered(QWidget *widget, ActionContext *context, bool toggled)
{
	Q_UNUSED(context)
	Q_UNUSED(toggled)

	(injectedFactory()->makeInjected<AddConferenceWindow>(widget))->show();
}

void AddConferenceAction::updateAddChatMenuItem()
{
	bool isConferenceSupported = false;

	foreach (const Account &account, m_accountManager->items())
	{
		if (account.protocolName() == "gadu")
			isConferenceSupported = true;
	}

	foreach (Action *action, actions())
	{
		action->setVisible(isConferenceSupported);
	}
}

#include "moc_add-conference-action.cpp"
