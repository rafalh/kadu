/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-protocol.h"
#include "utils/vcard-factory.h"

#include "jabber-contact-personal-info-service.h"

JabberContactPersonalInfoService::JabberContactPersonalInfoService(JabberProtocol *protocol) :
		ContactPersonalInfoService(protocol), Protocol(protocol)
{
}

void JabberContactPersonalInfoService::fetchPersonalInfo(Buddy buddy)
{
	CurrentBuddy = buddy;
	if (Protocol && Protocol->client() && Protocol->client()->rootTask())
		VCardFactory::instance()->getVCard(buddy.contacts().first().id(), Protocol->client()->rootTask(), this, SLOT(fetchingVCardFinished()));
}

void JabberContactPersonalInfoService::fetchingVCardFinished()
{
	XMPP::VCard vcard;
	XMPP::JT_VCard *task = (XMPP::JT_VCard *)sender();
	
	if (task && task->success())
	{
		vcard = task->vcard();
		CurrentBuddy.setNickName(vcard.nickName());
		CurrentBuddy.setFirstName(vcard.fullName());
		CurrentBuddy.setFamilyName(vcard.familyName());
		QDate bday = QDate::fromString(vcard.bdayStr(), "yyyy-MM-dd");
		if (bday.isValid() && !bday.isNull())
			CurrentBuddy.setBirthYear(bday.year());

		if (vcard.addressList().count() > 0)
			CurrentBuddy.setCity(vcard.addressList().at(0).locality);
		if (vcard.emailList().count() > 0)
			CurrentBuddy.setEmail(vcard.emailList().at(0).userid);
		CurrentBuddy.setWebsite(vcard.url());

		emit personalInfoAvailable(CurrentBuddy);
	}
}
