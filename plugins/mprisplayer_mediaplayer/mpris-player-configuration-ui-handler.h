/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef MPRIS_PLAYER_CONFIGURATION_UI_HANDLER_H
#define MPRIS_PLAYER_CONFIGURATION_UI_HANDLER_H

#include <QtCore/QMap>

#include "configuration/configuration.h"
#include "configuration/configuration-aware-object.h"
#include "configuration/deprecated-configuration-api.h"
#include "gui/windows/main-configuration-window.h"

class QComboBox;

class MPRISPlayerConfigurationUiHandler : public ConfigurationUiHandler
{
	Q_OBJECT

	static MPRISPlayerConfigurationUiHandler *Instance;

	QMap<QString, QString> PlayersMap;
	QComboBox *PlayersBox;

	MPRISPlayerConfigurationUiHandler();
	virtual ~MPRISPlayerConfigurationUiHandler();

	void loadPlayersListFromFile(const QString &globalFileName, const QString &userFileName);
	void fillPlayersBox();

private slots:
	void configurationApplied();

public:
	static void registerConfigurationUi();
	static void unregisterConfigurationUi();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

public slots:
	void addPlayer();
	void editPlayer();
	void delPlayer();

};

#endif /* MPRIS_PLAYER_CONFIGURATION_UI_HANDLER_H */
