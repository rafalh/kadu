/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef JABBER_RESOURCE_H
#define JABBER_RESOURCE_H

#include <QtCore/QObject>

#include <im.h>
#include <xmpp_tasks.h>

class JabberResource : public QObject
{
	Q_OBJECT

	XMPP::Jid Jid;
	XMPP::Resource Resource;

public:

	explicit JabberResource(const XMPP::Jid &jid, const XMPP::Resource &resource);

	virtual ~JabberResource();

	const XMPP::Jid &jid() const { return Jid; }

	void setResource (const XMPP::Resource &resource);
	const XMPP::Resource &resource() const { return Resource; }

};

#endif
