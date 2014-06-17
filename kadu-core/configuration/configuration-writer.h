/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exports.h"

#include <injeqt/setter-injector.h>
#include <QtCore/QObject>

class Configuration;
class ConfigurationPathProvider;

class KADUAPI ConfigurationWriter final : public QObject
{
	Q_OBJECT

public:
	explicit ConfigurationWriter(QObject *parent = nullptr);
	virtual ~ConfigurationWriter();

	void write() const;
	void backup() const;

private:
	Configuration *m_configuration;
	ConfigurationPathProvider *m_configurationPathProvider;

	void write(const QString &fileName) const;

private slots:
	injeqt_setter void setConfiguration(Configuration *configuration);
	injeqt_setter void setConfigurationPathProvider(ConfigurationPathProvider *configurationPathProvider);

};
