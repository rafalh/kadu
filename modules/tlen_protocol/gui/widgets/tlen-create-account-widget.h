/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@go2.pl)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef TLEN_CREATE_ACCOUNT_WIDGET_H
#define TLEN_CREATE_ACCOUNT_WIDGET_H

#include "gui/widgets/account-create-widget.h"

class QCheckBox;
class QGridLayout;
class QLineEdit;
class QPushButton;

class ChooseIdentityWidget;

class TlenCreateAccountWidget : public AccountCreateWidget
{
	Q_OBJECT

	QList<QWidget *> Widgets;

	QLineEdit *AccountName;
	QLineEdit *AccountId;
	QLineEdit *AccountPassword;
	ChooseIdentityWidget *Identity;
	QCheckBox *RememberPassword;

	QPushButton *AddThisAccount;
	//QPushButton *RemindPassword;

	void createGui();
	void createAccountGui(QGridLayout *gridLayout, int &row);

private slots:
	void iHaveAccountDataChanged();
	void addThisAccount();

public:
	explicit TlenCreateAccountWidget(QWidget *parent = 0);
	virtual ~TlenCreateAccountWidget();
	
	virtual void apply();
};

#endif // TLEN_CREATE_ACCOUNT_WIDGET_H
