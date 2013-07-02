/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef OTR_ACCOUNT_CONFIGURATION_WIDGET_H
#define OTR_ACCOUNT_CONFIGURATION_WIDGET_H

#include "gui/widgets/account-configuration-widget.h"

class QCheckBox;

class SimpleConfigurationValueStateNotifier;

class OtrPolicy;

class OtrAccountConfigurationWidget : public AccountConfigurationWidget
{
	Q_OBJECT

	QCheckBox *EnableCheckBox;
	QCheckBox *AutomaticallyInitiateCheckBox;
	QCheckBox *RequireCheckBox;

	SimpleConfigurationValueStateNotifier *StateNotifier;

	void createGui();
	OtrPolicy policy();
	void loadValues();

private slots:
	void updateState();

public:
	explicit OtrAccountConfigurationWidget(const Account &account, QWidget *parent = 0);
	virtual ~OtrAccountConfigurationWidget();

    virtual const ConfigurationValueStateNotifier * stateNotifier() const;

	virtual void apply();
	virtual void cancel();

};

#endif // OTR_ACCOUNT_CONFIGURATION_WIDGET_H