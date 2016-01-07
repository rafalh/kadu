/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

#include "configuration/configuration-aware-object.h"
#include "core/application.h"
#include "misc/paths-provider.h"

#include "mpris_mediaplayer.h"

class MediaPlayer;

class QString;

class MPRISPlayer : public MPRISMediaPlayer
{
	Q_OBJECT

	static const QString UserPlayersListFile;
	static const QString GlobalPlayersListFile;

	void prepareUserPlayersFile();
	void replacePlugin();
	void choosePlayer(const QString &key, const QString &value);

public:
	static QString userPlayersListFileName() { return Application::instance()->pathsProvider()->profilePath() + UserPlayersListFile; }
	static QString globalPlayersListFileName() { return Application::instance()->pathsProvider()->dataPath() + GlobalPlayersListFile; }

	Q_INVOKABLE explicit MPRISPlayer(QObject *parent = nullptr);
	virtual ~MPRISPlayer();

	void configurationApplied();

};
