/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "core/core.h"
#include "url-handlers/standard-url-expander-configuration.h"
#include "url-handlers/standard-url-expander.h"

#include "standard-url-expander-configurator.h"

StandardUrlExpanderConfigurator::StandardUrlExpanderConfigurator(Configuration *configuration) :
		m_configuration{configuration}
{
}

void StandardUrlExpanderConfigurator::setStandardUrlExpander(StandardUrlExpander *standardUrlExpander)
{
	ConfigurableStandardUrlExpander = standardUrlExpander;

	createDefaultConfiguration();
	configurationUpdated();
}

void StandardUrlExpanderConfigurator::createDefaultConfiguration()
{
	m_configuration->deprecatedApi()->addVariable("Chat", "FoldLink", true);
	m_configuration->deprecatedApi()->addVariable("Chat", "LinkFoldTreshold", 50);
}

void StandardUrlExpanderConfigurator::configurationUpdated()
{
	if (!ConfigurableStandardUrlExpander)
		return;

	StandardUrlExpanderConfiguration configuration;

	configuration.setFoldLink(m_configuration->deprecatedApi()->readBoolEntry("Chat", "FoldLink"));
	configuration.setFoldLinkThreshold(m_configuration->deprecatedApi()->readNumEntry("Chat", "LinkFoldTreshold"));

	ConfigurableStandardUrlExpander->setConfiguration(configuration);
}
