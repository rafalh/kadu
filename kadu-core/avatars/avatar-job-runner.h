/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "contacts/contact.h"
#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtGui/QPixmap>
#include <injeqt/injeqt.h>

class Account;
class AvatarManager;
class AvatarService;

class QTimer;

class KADUAPI AvatarJobRunner : public QObject
{
	Q_OBJECT

public:
	explicit AvatarJobRunner(const Contact &contact, QObject *parent);
	virtual ~AvatarJobRunner();

	void runJob();

signals:
	void jobFinished(bool ok);

private:
	QPointer<AvatarManager> m_avatarManager;

	Contact MyContact;
	QTimer *Timer;

private slots:
	INJEQT_SET void setAvatarManager(AvatarManager *avatarManager);

	void avatarDownloaded(bool ok , QImage avatar);
	void timeout();

};
