/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddies/buddy-manager.h"
#include "buddies/model/buddies-model.h"
#include "gui/widgets/contacts-line-edit.h"
#include "gui/widgets/contacts-list-view.h"

#include "select-contact-popup.h"

SelectContactPopup::SelectContactPopup(QWidget *parent) :
		ContactsListWidget(0, parent)
{
	setWindowFlags(Qt::Popup);

	BuddiesModel *model = new BuddiesModel(BuddyManager::instance(), this);

	view()->setItemsExpandable(false);
	view()->setModel(model);
}

SelectContactPopup::~SelectContactPopup()
{
}

void SelectContactPopup::show(const QString &text)
{
	nameFilterEdit()->setText(text);
	nameFilterEdit()->setFocus();
	ContactsListWidget::show();
}
