/*
 * serverinfomanager.cpp
 * Copyright (C) 2006  Remko Troncon
 *
 * %kadu copyright begin%
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * Copyright (C) 2006  Remko Troncon
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

#include "jabber-server-info-service.h"

JabberServerInfoService::JabberServerInfoService(JabberProtocol *protocol) :
		QObject(protocol), SupportsPep(false)
{
	//connect(XmppClient.data(), SIGNAL(disconnected()), SLOT(reset()));
}

JabberServerInfoService::~JabberServerInfoService()
{
}
/*
const Features & JabberServerInfoService::features() const
{
	return ServerFeatures;
}

const DiscoItem::Identities & JabberServerInfoService::identities() const
{
	return ServerIdentities;
}
*/
bool JabberServerInfoService::supportsPep() const
{
	return SupportsPep;
}

void JabberServerInfoService::requestServerInfo()
{/*
	if (!XmppClient)
		return;

	JT_DiscoInfo *jt = new JT_DiscoInfo(XmppClient->rootTask());
	connect(jt, SIGNAL(finished()), SLOT(requestFinished()));
	jt->get(XmppClient->jid().domain());
	jt->go(true);*/
}

void JabberServerInfoService::requestFinished()
{/*
	JT_DiscoInfo *jt = qobject_cast<JT_DiscoInfo *>(sender());
	if (!jt || !jt->success())
		return;

	ServerFeatures = jt->item().features();
	ServerIdentities = jt->item().identities();

	// TODO: Remove this, this is legacy
	if (ServerFeatures.test(QStringList("http://jabber.org/protocol/pubsub#pep")))
		SupportsPep = true;

	foreach(const DiscoItem::Identity &identity, ServerIdentities)
		if (identity.category == "pubsub" && identity.type == "pep")
			SupportsPep = true;

	emit updated();*/
}

void JabberServerInfoService::reset()
{
	SupportsPep = false;
	emit updated();
}

#include "moc_jabber-server-info-service.cpp"
