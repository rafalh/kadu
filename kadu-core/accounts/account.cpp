/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-details.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact-manager.h"
#include "contacts/account-data/contact-account-data.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"
#include "misc/misc.h"

#include "account.h"

Account * Account::loadFromStorage(StoragePoint *accountStoragePoint)
{
	return new Account(AccountData::loadFromStorage(accountStoragePoint));
}

Account::Account(AccountData::AccountType type) :
		Data(AccountData::TypeNull != type ? new AccountData(type) : 0)
{
}

Account::Account(AccountData *data) :
		Data(data)
{

}

Account::Account(const Account &copy) :
		Data(copy.Data)
{
}

Account::~Account()
{
}

QUuid Account::uuid() const
{
	return isNull() ? QUuid() : Data->uuid();
}

StoragePoint * Account::storage() const
{
	return isNull() ? 0 : Data->storage();
}

void Account::loadProtocol(ProtocolFactory *protocolFactory)
{
	if (!Data)
		return;

	Protocol *ProtocolHandler = protocolFactory->createProtocolHandler(this);
	AccountDetails *Details = protocolFactory->createAccountDetails(this);

	Data->setProtocolHandler(ProtocolHandler);
	Data->setDetails(Details);

	connect(ProtocolHandler, SIGNAL(statusChanged(Account *, Status)), this, SIGNAL(statusChanged()));
	connect(ProtocolHandler, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
			this, SIGNAL(contactStatusChanged(Account *, Contact, Status)));
}

void Account::unloadProtocol()
{
	if (Data)
		Data->unloadProtocol();
}

void Account::contactAdded(Contact contact)
{
	// TODO: fix now
// 	ContactAccountData *cad = ProtocolHandler->protocolFactory()->loadContactAccountData(this, contact);
// 	if (cad)
// 		contact.addAccountData(cad);
}

void Account::contactRemoved(Contact contact)
{
}

void Account::store()
{
	if (Data)
		Data->store();
}

void Account::removeFromStorage()
{
	if (Data)
		Data->removeFromStorage();
}

Contact Account::getContactById(const QString& id)
{
	return ContactManager::instance()->byId(this, id);
}

Contact Account::createAnonymous(const QString& id)
{
	if (!Data)
		return Contact::null;

	Contact result(ContactData::TypeAnonymous);
	ProtocolFactory *protocolFactory = Data->protocolHandler()->protocolFactory();
	ContactAccountData *contactAccountData = protocolFactory->newContactAccountData(this, result, id);
	if (!contactAccountData->isValid())
	{
		delete contactAccountData;
		return Contact::null;
	}

	result.addAccountData(contactAccountData);
	return result;
}

void Account::importProxySettings()
{
	if (!Data)
		return;

	Account *defaultAccount = AccountManager::instance()->defaultAccount();
	if (defaultAccount && defaultAccount->proxyHost().toString() != "0.0.0.0")
	{
		Data->setUseProxy(defaultAccount->useProxy());
		Data->setProxyHost(defaultAccount->proxyHost());
		Data->setProxyPort(defaultAccount->proxyPort());
		Data->setProxyRequiresAuthentication(defaultAccount->proxyRequiresAuthentication());
		Data->setProxyUser(defaultAccount->proxyUser());
		Data->setProxyPassword(defaultAccount->proxyPassword());
	}
}
