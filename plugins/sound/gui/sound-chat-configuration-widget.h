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

#include "gui/widgets/chat-configuration-widget.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class PluginInjectedFactory;

class SoundManager;
class SoundSelectFile;
class SimpleConfigurationValueStateNotifier;

class QCheckBox;

class SoundChatConfigurationWidget : public ChatConfigurationWidget
{
	Q_OBJECT

public:
	explicit SoundChatConfigurationWidget(const Chat &chat, SoundManager *soundManager, QWidget *parent = nullptr);
	virtual ~SoundChatConfigurationWidget();

	virtual const ConfigurationValueStateNotifier * stateNotifier() const override;

	virtual void apply() override;
	virtual void cancel() override;

private:
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
	QPointer<SoundManager> m_soundManager;

	SimpleConfigurationValueStateNotifier *m_stateNotifier;

	QCheckBox *m_useCustomSoundCheckBox;
	SoundSelectFile *m_customSoundSelectFile;

	void createGui();
	void loadValues();

private slots:
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
	INJEQT_INIT void init();

	void updateState();

};
