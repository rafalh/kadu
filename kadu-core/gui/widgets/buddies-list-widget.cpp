/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Longer (longer89@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtCore/QEvent>
#include <QtGui/QApplication>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>

#include "buddies/filter/buddy-name-filter.h"
#include "gui/widgets/buddies-list-view.h"
#include "icons-manager.h"

#include "buddies-list-widget.h"

BuddiesListWidget::BuddiesListWidget(FilterPosition filterPosition, MainWindow *mainWindow, QWidget *parent) :
		QWidget(parent)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	NameFilterWidget = new FilterWidget(this);
	
	connect(NameFilterWidget, SIGNAL(textChanged(const QString &)),
		this, SLOT(nameFilterChanged(const QString &)));

	View = new BuddiesListView(mainWindow, this);

	NameFilter = new BuddyNameFilter(this);
	View->addFilter(NameFilter);

	if (FilterAtTop == filterPosition)
	{
		layout->addWidget(NameFilterWidget);
		layout->addWidget(View);
	}
	else
	{
		layout->addWidget(View);
		layout->addWidget(NameFilterWidget);
	}

/*
	setFocusProxy(NameFilterWidget);
	View->setFocusPolicy(Qt::NoFocus);*/
}

BuddiesListWidget::~BuddiesListWidget()
{
}

bool BuddiesListWidget::eventFilter(QObject *object, QEvent *event)
{
	if (QEvent::KeyPress == event->type())
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		switch (keyEvent->key())
		{
			case Qt::Key_Down:
			case Qt::Key_Up:
			case Qt::Key_PageDown:
			case Qt::Key_PageUp:
			case Qt::Key_Enter:
			case Qt::Key_Return:
				qApp->sendEvent(view(), event);
				return true;
		}
	}

	return QObject::eventFilter(object, event);
}

void BuddiesListWidget::nameFilterChanged(const QString &filter)
{
	NameFilter->setName(filter);
}
