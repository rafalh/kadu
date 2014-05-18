/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2003, 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003 Dariusz Jagodzik (mast3r@kadu.net)
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

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>
#include <QtWidgets/QApplication>

#include "configuration/xml-configuration-file.h"
#include "misc/misc.h"

#include "debug.h"

#include "configuration-file.h"

static QMutex GlobalMutex;

ConfigFile::ConfigFile(const QString &filename) : filename(filename)
{
}

bool ConfigFile::changeEntry(const QString &group, const QString &name, const QString &value)
{
	QMutexLocker locker(&GlobalMutex);

//	kdebugm(KDEBUG_FUNCTION_START, "ConfigFile::changeEntry(%s, %s, %s) %p\n", qPrintable(group), qPrintable(name), qPrintable(value), this);
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement deprecated_elem = xml_config_file->accessElement(root_elem, "Deprecated");
	QDomElement config_file_elem = xml_config_file->accessElementByFileNameProperty(
		deprecated_elem, "ConfigFile", "name", filename.section('/', -1));
	QDomElement group_elem = xml_config_file->accessElementByProperty(
		config_file_elem, "Group", "name", group);
	QDomElement entry_elem = xml_config_file->accessElementByProperty(
		group_elem, "Entry", "name", name);
	entry_elem.setAttribute("value", value);

	return true;
}

QString ConfigFile::getEntry(const QString &group, const QString &name) const
{
	QMutexLocker locker(&GlobalMutex);

//	kdebugm(KDEBUG_FUNCTION_START, "ConfigFile::getEntry(%s, %s) %p\n", qPrintable(group), qPrintable(name), this);
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement deprecated_elem = xml_config_file->findElement(root_elem, "Deprecated");
	if (!deprecated_elem.isNull())
	{
		QDomElement config_file_elem = xml_config_file->findElementByFileNameProperty(
			deprecated_elem, "ConfigFile", "name", filename.section('/', -1));
		if (!config_file_elem.isNull())
		{
			QDomElement group_elem = xml_config_file->findElementByProperty(
				config_file_elem, "Group", "name", group);
			if (!group_elem.isNull())
			{
				QDomElement entry_elem =
					xml_config_file->findElementByProperty(
						group_elem, "Entry", "name", name);
				if (!entry_elem.isNull())
					return entry_elem.attribute("value");
			}
		}
	}

	return QString();
}

void ConfigFile::writeEntry(const QString &group,const QString &name, const QString &value)
{
	changeEntry(group, name, value);
}

void ConfigFile::writeEntry(const QString &group,const QString &name, const char *value)
{
	changeEntry(group, name, QString::fromLocal8Bit(value));
}

void ConfigFile::writeEntry(const QString &group,const QString &name, const int value)
{
	changeEntry(group, name, QString::number(value));
}

void ConfigFile::writeEntry(const QString &group,const QString &name, const bool value)
{
	changeEntry(group, name, value ? "true" : "false");
}

void ConfigFile::writeEntry(const QString &group,const QString &name, const QRect &value)
{
	changeEntry(group, name, rectToString(value));
}

void ConfigFile::writeEntry(const QString &group,const QString &name, const QColor &value)
{
	changeEntry(group, name, value.name());
}

void ConfigFile::writeEntry(const QString &group,const QString &name, const QFont &value)
{
	changeEntry(group, name, value.toString());
}

QString ConfigFile::readEntry(const QString &group,const QString &name, const QString &def) const
{
	QString string = getEntry(group, name);
	if (string.isNull())
		return def;
	return string;
}

unsigned int ConfigFile::readUnsignedNumEntry(const QString &group,const QString &name, unsigned int def) const
{
	bool ok;
	QString string = getEntry(group, name);
	if (string.isNull())
		return def;
	unsigned int num = string.toUInt(&ok);
	if (!ok)
		return def;
	return num;
}

int ConfigFile::readNumEntry(const QString &group,const QString &name, int def) const
{
	bool ok;
	QString string = getEntry(group, name);
	if (string.isNull())
		return def;
	int num = string.toInt(&ok);
	if (!ok)
		return def;
	return num;
}

bool ConfigFile::readBoolEntry(const QString &group,const QString &name, bool def) const
{
	QString string = getEntry(group, name);
	if (string.isNull())
		return def;
	return string=="true";
}

QRect ConfigFile::readRectEntry(const QString &group,const QString &name, const QRect *def) const
{
	return stringToRect(getEntry(group, name), def);
}

QColor ConfigFile::readColorEntry(const QString &group,const QString &name, const QColor *def) const
{
	QString str = getEntry(group, name);
	if (str.isNull())
		return def ? *def : QColor(0, 0, 0);
	else
		return QColor(str);
}


QFont ConfigFile::readFontEntry(const QString &group,const QString &name, const QFont *def) const
{
	QString string = getEntry(group, name);
	if (string.isNull())
		return def ? *def : QApplication::font();
	QFont font;
	if(font.fromString(string))
		return font;
	return def ? *def : QApplication::font();
}

void ConfigFile::removeVariable(const QString &group, const QString &name)
{
	QMutexLocker locker(&GlobalMutex);

	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement deprecated_elem = xml_config_file->accessElement(root_elem, "Deprecated");
	QDomElement config_file_elem = xml_config_file->accessElementByFileNameProperty(
		deprecated_elem, "ConfigFile", "name", filename.section('/', -1));
	QDomElement group_elem = xml_config_file->accessElementByProperty(
		config_file_elem, "Group", "name", group);
	QDomElement entry_elem = xml_config_file->accessElementByProperty(
		group_elem, "Entry", "name", name);
	group_elem.removeChild(entry_elem);
}

void ConfigFile::addVariable(const QString &group, const QString &name, const QString &defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const char *defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const int defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const bool defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const QColor &defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}
void ConfigFile::addVariable(const QString &group, const QString &name, const QFont &defvalue)
{
	if (getEntry(group, name).isEmpty())
		writeEntry(group,name,defvalue);
}

ConfigFile *config_file;
