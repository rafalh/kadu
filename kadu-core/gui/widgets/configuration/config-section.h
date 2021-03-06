/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/kadu-tab-widget.h"
#include "icons/kadu-icon.h"

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ConfigGroupBox;
class ConfigTab;
class ConfigurationWidget;
class Configuration;
class ConfigWidget;
class IconsManager;

class QGridLayout;
class QGroupBox;
class QListWidgetItem;

class ConfigSection : public QObject
{
	Q_OBJECT

public:
	ConfigSection(const QString &name, ConfigurationWidget *configurationWidget, QListWidgetItem *listWidgetItem, QWidget *parentConfigGroupBoxWidget,
		const KaduIcon &icon);
	virtual ~ConfigSection();

	const QString & name() const { return Name; }

	void activate();

	void show() { TabWidget->show(); }
	void hide() { TabWidget->hide(); }

	void addFullPageWidget(const QString &name, QWidget *widget);

	ConfigGroupBox * configGroupBox(const QString &tab, const QString &groupBox, bool create);

private:
	QPointer<Configuration> m_configuration;
	QPointer<IconsManager> m_iconsManager;

	QString Name;
	ConfigurationWidget *MyConfigurationWidget;
	KaduIcon Icon;

	QListWidgetItem *ListWidgetItem;
	bool Activated;
	QMap<QString, ConfigTab *> ConfigTabs;

	QWidget *ParentConfigGroupBoxWidget;
	KaduTabWidget *TabWidget;

	ConfigTab * configTab(const QString &name, bool create);

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_INIT void init();

	void configTabDestroyed(QObject *obj);
	void iconThemeChanged();

};
