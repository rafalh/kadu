/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "kadu-application.h"

#include "simpleview-config-ui.h"

SimpleViewConfigUi *SimpleViewConfigUi::Instance = 0;

SimpleViewConfigUi::SimpleViewConfigUi()
{
	createDefaultConfiguration();
}

SimpleViewConfigUi::~SimpleViewConfigUi()
{
}

void SimpleViewConfigUi::createInstance()
{
	if (!Instance)
		Instance = new SimpleViewConfigUi();
}

void SimpleViewConfigUi::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

void SimpleViewConfigUi::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	Q_UNUSED(mainConfigurationWindow)
	/* Keep this code for further use:
	connect(mainConfigurationWindow, SIGNAL(destroyed()), this, SLOT(mainConfigurationWindowDestroyed()));
	*/
}

/* Keep this code for further use:
void SimpleViewConfigUi::mainConfigurationWindowDestroyed()
{
}
*/

void SimpleViewConfigUi::createDefaultConfiguration()
{
	KaduApplication::instance()->configuration()->deprecatedApi()->addVariable("Look", "SimpleViewKeepSize", true);
	KaduApplication::instance()->configuration()->deprecatedApi()->addVariable("Look", "SimpleViewNoScrollBar", true);
	KaduApplication::instance()->configuration()->deprecatedApi()->addVariable("Look", "SimpleViewBorderless", true);
}



#include "moc_simpleview-config-ui.cpp"
