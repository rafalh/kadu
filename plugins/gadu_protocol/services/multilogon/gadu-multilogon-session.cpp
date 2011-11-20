/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

// for Q_OS_WIN macro
#include <QtCore/QtGlobal>

#ifdef Q_OS_WIN
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include "gadu-multilogon-session.h"

GaduMultilogonSession::GaduMultilogonSession(Account account, QObject *parent) :
		MultilogonSession(account, parent)
{
}

GaduMultilogonSession::~GaduMultilogonSession()
{
}

GaduMultilogonSession::GaduMultilogonSession(Account account, const gg_multilogon_session &session, QObject *parent) :
		MultilogonSession(account, parent)
{
	Id = session.id;
	setName(session.name);

	QHostAddress remoteAddress;
	remoteAddress.setAddress(ntohl(session.remote_addr));
	setRemoteAddres(remoteAddress);

	QDateTime logonTime;
	logonTime.setTime_t(session.logon_time);
	setLogonTime(logonTime);
}

const gg_multilogon_id_t & GaduMultilogonSession::id() const
{
	return Id;
}
