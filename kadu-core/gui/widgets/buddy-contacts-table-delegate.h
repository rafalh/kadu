/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include <QtCore/QPointer>
#include <QtWidgets/QStyledItemDelegate>
#include <injeqt/injeqt.h>

class InjectedFactory;

class BuddyContactsTableDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	explicit BuddyContactsTableDelegate(QObject *parent = nullptr);
	virtual ~BuddyContactsTableDelegate();

	virtual QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

protected:
	virtual bool eventFilter(QObject *editor, QEvent *event);

private:
	QPointer<InjectedFactory> m_injectedFactory;

private slots:
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);

	void dataChanged();

};
