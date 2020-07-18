/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 * Copyright 2011 Wojciech Treter (juzefwt@gmail.com)
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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QThread>
#include <QtSql/QSqlError>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "misc/paths-provider.h"
#include "plugin/plugin-injected-factory.h"

#include "storage/history-sql-storage.h"
#include "storage/sql-import.h"
#include "storage/sql-restore.h"

#include "sql-initializer.h"

#define HISTORY_FILE_0 "history/history.db"
#define HISTORY_FILE_1 "history1.db"
#define HISTORY_FILE_CURRENT "history2.db"

SqlInitializer::SqlInitializer(QObject *parent) :
		QObject(parent)
{
}

SqlInitializer::~SqlInitializer()
{
}

void SqlInitializer::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void SqlInitializer::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
	m_pluginInjectedFactory = pluginInjectedFactory;
}

void SqlInitializer::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void SqlInitializer::initialize()
{
	bool ok = initDatabase();
	// remove database connection so it can be recreated in main thread
	QSqlDatabase::removeDatabase("kadu-history");
	emit databaseReady(ok);

	deleteLater();
}

bool SqlInitializer::oldHistoryFileExists()
{
	QFileInfo scheme0FileInfo(m_pathsProvider->profilePath() + QStringLiteral(HISTORY_FILE_0));
	QFileInfo scheme1FileInfo(m_pathsProvider->profilePath() + QStringLiteral(HISTORY_FILE_1));
	return scheme0FileInfo.exists() || scheme1FileInfo.exists();
}

bool SqlInitializer::currentHistoryFileExists()
{
	QFileInfo schemeCurrentFileInfo(m_pathsProvider->profilePath() + QStringLiteral(HISTORY_FILE_CURRENT));
	return schemeCurrentFileInfo.exists();
}

bool SqlInitializer::copyHistoryFile()
{
	QFileInfo schemeCurrentFileInfo(m_pathsProvider->profilePath() + QStringLiteral(HISTORY_FILE_CURRENT));
	if (schemeCurrentFileInfo.exists())
		return true;

	QFileInfo scheme1FileInfo(m_pathsProvider->profilePath() + QStringLiteral(HISTORY_FILE_1));
	if (scheme1FileInfo.exists())
		return QFile::copy(scheme1FileInfo.absoluteFilePath(), schemeCurrentFileInfo.absoluteFilePath());

	QFileInfo scheme0FileInfo(m_pathsProvider->profilePath() + QStringLiteral(HISTORY_FILE_0));
	if (scheme0FileInfo.exists())
		return QFile::copy(scheme0FileInfo.absoluteFilePath(), schemeCurrentFileInfo.absoluteFilePath());

	return false;
}

bool SqlInitializer::initDatabase()
{
	if (QSqlDatabase::contains("kadu-history"))
	{
		if (QSqlDatabase::database("kadu-history").isOpen())
			QSqlDatabase::database("kadu-history").close();
		QSqlDatabase::removeDatabase("kadu-history");
	}

	bool currentFileExists = currentHistoryFileExists();
	bool anyHistoryFileExists = currentFileExists || oldHistoryFileExists();

	if (!currentFileExists && oldHistoryFileExists())
	{
		emit progressMessage("dialog-information", tr("Copying history file to new location: %1 ...").arg(m_pathsProvider->profilePath() + QStringLiteral(HISTORY_FILE_CURRENT)));
		if (!copyHistoryFile())
		{
			emit progressFinished(false, "dialog-error", tr("Unable to copy history file to new location. Check if disk is full."));
			return false;
		}
	}

	QString historyFilePath = m_pathsProvider->profilePath() + QStringLiteral(HISTORY_FILE_CURRENT);

	QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE", "kadu-history");
	database.setDatabaseName(historyFilePath);

	if (!database.open())
	{
		emit progressFinished(false, "dialog-error", tr("Unable to open database: %1").arg(database.lastError().text()));
		return false;
	}

	if (anyHistoryFileExists && SqlRestore::isCorrupted(database)) // this is not new database
	{
		database.close();

		emit progressMessage("dialog-warning", tr("History file is corrupted, performing recovery..."));

		auto sqlRestore = m_pluginInjectedFactory->makeUnique<SqlRestore>();
		SqlRestore::RestoreError error = sqlRestore->performRestore(historyFilePath);
		if (SqlRestore::ErrorNoError == error)
			emit progressMessage("dialog-information", tr("Recovery completed."));
		else
			emit progressMessage("dialog-error", tr("Recovery failed: %s").arg(SqlRestore::errorMessage(error)));

		if (!database.open())
		{
			emit progressFinished(false, "dialog-error", tr("Unable to open database: %1").arg(database.lastError().text()));
			return false;
		}
	}

	if (SqlImport::importNeeded(database))
	{
		if (anyHistoryFileExists)
			emit progressMessage("dialog-warning", tr("History file is outdated, performing import..."));

		auto sqlImport = m_pluginInjectedFactory->makeUnique<SqlImport>();
		sqlImport->performImport(database);

		if (anyHistoryFileExists)
			emit progressFinished(true, "dialog-information", tr("Import completed."));
	}
	else
	{
		m_configuration->deprecatedApi()->writeEntry("History", "Schema", SqlImport::databaseSchemaVersion(database));
		emit progressFinished(true, "dialog-information", tr("Copying completed."));
	}
	return true;
}

#include "moc_sql-initializer.cpp"
