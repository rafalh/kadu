/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef JABBER_SERVER_INFO_SERVICE_H
#define JABBER_SERVER_INFO_SERVICE_H

#include <QtCore/QObject>
#include <QtCore/QString>

class JabberProtocol;

class JabberServerInfoService : public QObject
{
	Q_OBJECT

	//Features ServerFeatures;
	//DiscoItem::Identities ServerIdentities;

	bool SupportsPep;

private slots:
	void requestFinished();
	void reset();

public:
	explicit JabberServerInfoService(JabberProtocol *protocol);
	virtual ~JabberServerInfoService();

//	const Features & features() const;
//	const DiscoItem::Identities & identities() const;

	bool supportsPep() const;

	void requestServerInfo();

signals:
	void updated();

};

#endif // JABBER_SERVER_INFO_SERVICE_H
