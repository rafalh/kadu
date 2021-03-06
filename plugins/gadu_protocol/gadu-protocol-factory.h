/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "protocols/protocol-factory.h"
#include "status/status-adapter.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>
#include <memory>

class GaduListHelper;
class GaduServersManager;
class PluginInjectedFactory;

class GaduProtocolFactory : public ProtocolFactory
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit GaduProtocolFactory(QObject *parent = nullptr);
	virtual ~GaduProtocolFactory();

	virtual Protocol * createProtocolHandler(Account account);
	virtual AccountDetails * createAccountDetails(AccountShared *accountShared);
	virtual AccountAddWidget * newAddAccountWidget(bool showButtons, QWidget *parent);
	virtual AccountCreateWidget * newCreateAccountWidget(bool showButtons, QWidget *parent);
	virtual AccountEditWidget * newEditAccountWidget(Account, QWidget *parent);
	virtual QWidget * newContactPersonalInfoWidget(Contact contact, QWidget *parent = nullptr);
	virtual QList<StatusType> supportedStatusTypes();
	virtual StatusAdapter * statusAdapter() { return MyStatusAdapter.get(); }
	virtual QString idLabel();
	virtual QValidator::State validateId(QString id);
	virtual bool canRegister();
	virtual bool canRemoveAvatar() { return false; } // this is so lame for gadu-gadu, so so lame ...

	virtual QString name() { return "gadu"; }
	virtual QString displayName() { return "Gadu-Gadu"; }
	virtual KaduIcon icon();

private:
	QPointer<GaduListHelper> m_gaduListHelper;
	QPointer<GaduServersManager> m_gaduServersManager;
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;

	QList<StatusType> SupportedStatusTypes;

	std::unique_ptr<StatusAdapter> MyStatusAdapter;

private slots:
	INJEQT_SET void setGaduListHelper(GaduListHelper *gaduListHelper);
	INJEQT_SET void setGaduServersManager(GaduServersManager *gaduServersManager);
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);

};
