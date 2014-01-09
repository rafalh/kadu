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

#pragma once

#include <QtCore/QString>
#include <exception>

class PluginActivationErrorException : public std::exception
{

public:
	PluginActivationErrorException(QString pluginName, QString errorMessage) :
			m_pluginName{std::move(pluginName)}, m_errorMessage{std::move(errorMessage)} {}

	QString pluginName() const { return m_pluginName; }
	QString errorMessage() const { return m_errorMessage; }

private:
	QString m_pluginName;
	QString m_errorMessage;

	virtual const char * what() const noexcept { return "Plugin activation error"; }

};