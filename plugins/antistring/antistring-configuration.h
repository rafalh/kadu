/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-aware-object.h"
#include "html/normalized-html-string.h"

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

typedef QPair<QString, int> ConditionPair;

class Configuration;
class PathsProvider;

class AntistringConfiguration : public QObject, ConfigurationAwareObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit AntistringConfiguration(QObject *parent = nullptr);
	virtual ~AntistringConfiguration();

	void readConditions();
	void storeConditions();

	QList<QPair<QString, int> > & conditions() { return Conditions; }

	bool enabled() const { return Enabled; }
	bool messageStop() const { return MessageStop; }
	bool logMessage() const { return LogMessage; }
	const NormalizedHtmlString & returnMessage() const { return ReturnMessage; }
	const QString & logFile() const { return LogFile; }

protected:
	virtual void configurationUpdated();

private:
	QPointer<Configuration> m_configuration;
	QPointer<PathsProvider> m_pathsProvider;

	QList<ConditionPair> Conditions;

	bool Enabled;
	bool MessageStop;
	bool LogMessage;
	NormalizedHtmlString ReturnMessage;
	QString LogFile;

	void createDefaultConfiguration();
	void addCondition(const QString &conditionString);
	void readDefaultConditions();

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_INIT void init();

};
