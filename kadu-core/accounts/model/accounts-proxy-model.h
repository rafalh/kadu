/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef ACCOUNTS_PROXY_MODEL_H
#define ACCOUNTS_PROXY_MODEL_H

#include <QtCore/QSortFilterProxyModel>

class AbstractAccountFilter;

class AccountsProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

	QList<AbstractAccountFilter *> Filters;

	bool BrokenStringCompare;
	int compareNames(QString n1, QString n2) const;

private slots:
	void filterChangedSlot();

protected:
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

public:
	AccountsProxyModel(QObject *parent = nullptr);
	virtual ~AccountsProxyModel();

	void addFilter(AbstractAccountFilter *filter);
	void removeFilter(AbstractAccountFilter *filter);

signals:
	void filterChanged();

};

#endif // ACCOUNTS_PROXY_MODEL_H
