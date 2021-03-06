/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef BUDDY_OPTIONS_CONFIGURATION_WIDGET_H
#define BUDDY_OPTIONS_CONFIGURATION_WIDGET_H

#include <QtWidgets/QWidget>

#include "buddies/buddy.h"
#include "configuration/configuration-aware-object.h"

#include "exports.h"

class QCheckBox;

class KADUAPI BuddyOptionsConfigurationWidget : public QWidget, ConfigurationAwareObject
{
	Q_OBJECT

	Buddy MyBuddy;

	QCheckBox *BlockCheckBox;
	QCheckBox *OfflineToCheckBox;
	QCheckBox *NotifyCheckBox;
	QCheckBox *HideDescriptionCheckBox;

	void createGui();

private slots:
	void updateOfflineTo();
	void offlineToToggled(bool toggled);

protected:
	virtual void configurationUpdated();

public:
	explicit BuddyOptionsConfigurationWidget(const Buddy &buddy, QWidget *parent = nullptr);
	virtual ~BuddyOptionsConfigurationWidget();

	void save();

};

#endif // BUDDY_OPTIONS_CONFIGURATION_WIDGET_H
