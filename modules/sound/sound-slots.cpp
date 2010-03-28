/*
 * %kadu copyright begin%
 * Copyright 2006, 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2005, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
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

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QPushButton>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QMenuBar>
#include <QtGui/QVBoxLayout>

#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/configuration/notify-group-box.h"
#include "gui/widgets/select-file.h"
#include "gui/windows/kadu-window.h"

#include "debug.h"
#include "icons-manager.h"

#include "sound-slots.h"

#include "sound-theme-manager.h"

SoundSlots::SoundSlots(bool firstLoad, QObject *parent)
	: QObject(parent),
	soundfiles(), soundNames(), soundTexts()
{
	kdebugf();

	sound_manager->setMute(!config_file.readBoolEntry("Sounds", "PlaySound"));

	mute_action = new ActionDescription(this,
		ActionDescription::TypeGlobal, "muteSoundsAction",
		this, SLOT(muteActionActivated(QAction *, bool)),
		"16x16/audio-volume-high.png", "16x16/audio-volume-muted.png", tr("Play sounds"), true//, tr("Unmute sounds")
	);
	connect(mute_action, SIGNAL(actionCreated(Action *)), this, SLOT(setMuteActionState()));

	Core::instance()->kaduWindow()->insertMenuActionDescription(mute_action, KaduWindow::MenuKadu, 7);

	if (firstLoad)
		KaduWindow::addAction("muteSoundsAction");

	setMuteActionState();

	kdebugf2();
}

SoundSlots::~SoundSlots()
{
	kdebugf();
	delete mute_action;
	mute_action = 0;
	kdebugf2();
}

void SoundSlots::themeChanged(const QString &theme)
{
	SoundThemeManager::instance()->applyTheme(theme);
}

void SoundSlots::muteActionActivated(QAction  *action, bool is_on)
{
	Q_UNUSED(action)
	kdebugf();
	sound_manager->setMute(!is_on);
	foreach (Action *action, mute_action->actions())
		action->setChecked(is_on);
	config_file.writeEntry("Sounds", "PlaySound", is_on);
	kdebugf2();
}

void SoundSlots::setMuteActionState()
{
	foreach (Action *action, mute_action->actions())
		action->setChecked(!sound_manager->isMuted());
}

void SoundSlots::muteUnmuteSounds()
{
	kdebugf();
	muteActionActivated(0, !sound_manager->isMuted());
	kdebugf2();
}

void SoundSlots::configurationUpdated()
{
	muteActionActivated(0, config_file.readBoolEntry("Sounds", "PlaySound"));
}

void SoundSlots::testSoundPlaying()
{
	kdebugf();

	QString soundFIle = SoundThemeManager::instance()->themes()->themePath() + SoundThemeManager::instance()->themes()->getThemeEntry("NewChat");
	sound_manager->play(soundFIle, true);

	kdebugf2();
}
