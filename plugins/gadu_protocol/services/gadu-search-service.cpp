/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "misc/misc.h"
#include "debug.h"

#include "helpers/gadu-protocol-helper.h"
#include "gadu-contact-details.h"
#include "gadu-protocol.h"

#include "gadu-search-service.h"

GaduSearchService::GaduSearchService(GaduProtocol *protocol) :
		SearchService(protocol), Protocol(protocol), Query(BuddySearchCriteria()),
		SearchSeq(0), From(0), Stopped(false)
{
}

void GaduSearchService::searchFirst(BuddySearchCriteria criteria)
{
	Query = criteria;
	From = 0;
	searchNext();
}

void GaduSearchService::searchNext()
{
	Stopped = false;
	gg_pubdir50_t req = gg_pubdir50_new(GG_PUBDIR50_SEARCH);

	if (Query.SearchBuddy.hasContact(Protocol->account()))
		gg_pubdir50_add(req, GG_PUBDIR50_UIN, Query.SearchBuddy.id(Protocol->account()).toUtf8().constData());
	if (!Query.SearchBuddy.firstName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, Query.SearchBuddy.firstName().toUtf8().constData());
	if (!Query.SearchBuddy.lastName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, Query.SearchBuddy.lastName().toUtf8().constData());
	if (!Query.SearchBuddy.nickName().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, Query.SearchBuddy.nickName().toUtf8().constData());
	if (!Query.SearchBuddy.city().isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_CITY, Query.SearchBuddy.city().toUtf8().constData());
	if (!Query.BirthYearFrom.isEmpty())
	{
		QString bufYear = Query.BirthYearFrom + ' ' + Query.BirthYearTo;
		gg_pubdir50_add(req, GG_PUBDIR50_BIRTHYEAR, bufYear.toUtf8().constData());
	}

	switch (Query.SearchBuddy.gender())
	{
		case GenderMale:
			gg_pubdir50_add(req, GG_PUBDIR50_GENDER, GG_PUBDIR50_GENDER_MALE);
			break;
		case GenderFemale:
			gg_pubdir50_add(req, GG_PUBDIR50_GENDER, GG_PUBDIR50_GENDER_FEMALE);
			break;
		case GenderUnknown:
			// do nothing
			break;
	}

	if (Query.Active)
		gg_pubdir50_add(req, GG_PUBDIR50_ACTIVE, GG_PUBDIR50_ACTIVE_TRUE);

	gg_pubdir50_add(req, GG_PUBDIR50_START, QString::number(From).toUtf8().constData());

	Protocol->disableSocketNotifiers();
	SearchSeq = gg_pubdir50(Protocol->gaduSession(), req);
	Protocol->enableSocketNotifiers();
	gg_pubdir50_free(req);
}

void GaduSearchService::stop()
{
	Stopped = true;
}

void GaduSearchService::handleEventPubdir50SearchReply(struct gg_event *e)
{
	gg_pubdir50_t res = e->event.pubdir50;

	BuddyList results;

	int count = gg_pubdir50_count(res);
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "found %d results\n", count);

	for (int i = 0; i < count; i++)
		results.append(GaduProtocolHelper::searchResultToBuddy(Protocol->account(), res, i));

	From = gg_pubdir50_next(res);

	emit newResults(results);
}

#include "moc_gadu-search-service.cpp"
