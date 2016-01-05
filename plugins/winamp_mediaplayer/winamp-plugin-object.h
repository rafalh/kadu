/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "plugin/plugin-object.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class PluginRepository;
class WinampMediaPlayer;

class WinampPluginObject : public PluginObject
{
	Q_OBJECT
	INJEQT_INSTANCE_IMMEDIATE

public:
	Q_INVOKABLE explicit WinampPluginObject(QObject *parent = nullptr);
	virtual ~WinampPluginObject();

private:
	QPointer<PluginRepository> m_pluginRepository;
	QPointer<WinampMediaPlayer> m_winampMediaPlayer;

private slots:
	INJEQT_INIT void init();
	INJEQT_DONE void done();
	INJEQT_SET void setPluginRepository(PluginRepository *pluginRepository);
	INJEQT_SET void setWinampMediaPlayer(WinampMediaPlayer *winampMediaPlayer);

};