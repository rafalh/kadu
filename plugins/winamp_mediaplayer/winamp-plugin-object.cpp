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

#include "winamp-plugin-object.h"

#include "winamp.h"

#include "plugins/mediaplayer/mediaplayer-plugin-object.h"
#include "plugins/mediaplayer/mediaplayer.h"

WinampPluginObject::WinampPluginObject(QObject *parent) :
		QObject{parent}
{
}

WinampPluginObject::~WinampPluginObject()
{
}

void WinampPluginObject::setMediaPlayer(MediaPlayer *mediaPlayer)
{
	m_mediaPlayer = mediaPlayer;
}

void WinampPluginObject::setWinampMediaPlayer(WinampMediaPlayer *winampMediaPlayer)
{
	m_winampMediaPlayer = winampMediaPlayer;
}

void WinampPluginObject::init()
{
	m_mediaPlayer->registerMediaPlayer(m_winampMediaPlayer, m_winampMediaPlayer);
}

void WinampPluginObject::done()
{
	m_mediaPlayer->unregisterMediaPlayer();
}

#include "moc_winamp-plugin-object.cpp"
