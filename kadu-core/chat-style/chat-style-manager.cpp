/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-style-manager.h"

#include "chat-style/chat-style-configuration-ui-handler.h"
#include "chat-style/engine/adium/adium-style-engine.h"
#include "chat-style/engine/configured-chat-style-renderer-factory-provider.h"
#include "chat-style/engine/kadu/kadu-style-engine.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "core/core.h"
#include "formatted-string/formatted-string-factory.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "gui/widgets/chat-style-preview.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/algorithm.h"
#include "misc/memory.h"
#include "misc/paths-provider.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtGui/QPalette>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

ChatStyleManager::ChatStyleManager(QObject *parent) :
		QObject{parent},
		CurrentEngine{},
		CfgNoHeaderRepeat{}, CfgHeaderSeparatorHeight{},
		CfgNoHeaderInterval{}, ParagraphSeparator{}, Prune{}, NoServerTime{},
		NoServerTimeDiff{}
{
	init();
}

ChatStyleManager::~ChatStyleManager()
{
	unregisterChatStyleEngine("Kadu");
	unregisterChatStyleEngine("Adium");
}

void ChatStyleManager::setConfiguredChatStyleRendererFactoryProvider(ConfiguredChatStyleRendererFactoryProvider *configuredChatStyleRendererFactoryProvider)
{
	CurrentConfiguredChatStyleRendererFactoryProvider = configuredChatStyleRendererFactoryProvider;
	configurationUpdated();
}

void ChatStyleManager::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
	CurrentFormattedStringFactory = formattedStringFactory;
}

void ChatStyleManager::init()
{
	registerChatStyleEngine("Kadu", make_unique<KaduStyleEngine>());

	auto adiumStyleEngine = make_unique<AdiumStyleEngine>();
	adiumStyleEngine.get()->setMessageHtmlRendererService(Core::instance()->messageHtmlRendererService());
	registerChatStyleEngine("Adium", std::move(adiumStyleEngine));

	loadStyles();
}

void ChatStyleManager::registerChatStyleEngine(const QString &name, std::unique_ptr<ChatStyleEngine> engine)
{
	if (engine && !contains(RegisteredEngines, name))
		RegisteredEngines.insert(std::make_pair(name, std::move(engine)));
}

void ChatStyleManager::unregisterChatStyleEngine(const QString &name)
{
	RegisteredEngines.erase(name);
}

void ChatStyleManager::configurationUpdated()
{
	if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "ChatPrune", true))
		Prune = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Chat", "ChatPruneLen");
	else
	{
		Application::instance()->configuration()->deprecatedApi()->writeEntry("Chat", "ChatPrune", true);
		Application::instance()->configuration()->deprecatedApi()->writeEntry("Chat", "ChatPruneLen", 0);
		Prune = 0;
	}

	ParagraphSeparator = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Look", "ParagraphSeparator");

	QFont font = Application::instance()->configuration()->deprecatedApi()->readFontEntry("Look","ChatFont");

	QString fontFamily = font.family();
	QString fontSize;
	if (font.pointSize() > 0)
		fontSize = QString::number(font.pointSize()) + "pt";
	else
		fontSize = QString::number(font.pixelSize()) + "px";
	QString fontStyle = font.italic() ? "italic" : "normal";
	QString fontWeight = font.bold() ? "bold" : "normal";
	QString textDecoration = font.underline() ? "underline" : "none";
	QString backgroundColor = "transparent";
	if (ChatConfigurationHolder::instance()->chatBgFilled())
		backgroundColor = ChatConfigurationHolder::instance()->chatBgColor().name();

	MainStyle = QString(
		"html {"
		"	font: %1 %2 %3 %4;"
		"	text-decoration: %5;"
		"	word-wrap: break-word;"
		"}"
		"a {"
		"	text-decoration: underline;"
		"}"
		"body {"
		"	margin: %6;"
		"	padding: 0;"
		"	background-color: %7;"
		"}"
		"p {"
		"	margin: 0;"
		"	padding: 3px;"
		"}").arg(fontStyle, fontWeight, fontSize, fontFamily, textDecoration, QString::number(ParagraphSeparator), backgroundColor);

	CfgNoHeaderRepeat = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "NoHeaderRepeat", true);

	// headers removal stuff
	if (CfgNoHeaderRepeat)
	{
		CfgHeaderSeparatorHeight = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Look", "HeaderSeparatorHeight");
		CfgNoHeaderInterval = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Look", "NoHeaderInterval");
	}
	else
	{
		CfgHeaderSeparatorHeight = 0;
		CfgNoHeaderInterval = 0;
	}

	NoServerTime = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Look", "NoServerTime");
	NoServerTimeDiff = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Look", "NoServerTimeDiff");

	auto newChatStyle = ChatStyle{Application::instance()->configuration()->deprecatedApi()->readEntry("Look", "Style"), Application::instance()->configuration()->deprecatedApi()->readEntry("Look", "ChatStyleVariant")};

	// if Style was changed, load new Style
	if (!CurrentEngine || newChatStyle != m_currentChatStyle)
	{
		auto newStyleName = fixedStyleName(newChatStyle.name());
		CurrentEngine = AvailableStyles.value(newStyleName).engine;
		auto newVariantName = fixedVariantName(newStyleName, newChatStyle.variant());
		m_currentChatStyle = {newStyleName, newVariantName};

		if (CurrentConfiguredChatStyleRendererFactoryProvider)
			CurrentConfiguredChatStyleRendererFactoryProvider->setChatStyleRendererFactory(CurrentEngine->createRendererFactory(m_currentChatStyle));
	}

	emit chatStyleConfigurationUpdated();
}

QString ChatStyleManager::fixedStyleName(QString styleName)
{
	if (!AvailableStyles.contains(styleName))
	{
		styleName = "Satin";
		if (!AvailableStyles.contains(styleName))
		{
			styleName = "kadu";
			if (!AvailableStyles.contains(styleName))
				styleName = *AvailableStyles.keys().constBegin();
		}
	}

	return styleName;
}

QString ChatStyleManager::fixedVariantName(const QString &styleName, QString variantName)
{
	if (!CurrentEngine->styleVariants(styleName).contains(variantName))
		return CurrentEngine->defaultVariant(styleName);

	return variantName;
}

//any better ideas?
void ChatStyleManager::loadStyles()
{
	QDir dir;
	QString path, StyleName;
	QFileInfo fi;
	QStringList files;

	path = Application::instance()->pathsProvider()->profilePath() + QLatin1String("syntax/chat/");
	dir.setPath(path);

	files = dir.entryList();

	AvailableStyles.clear(); // allow reloading of styles

	foreach (const QString &file, files)
	{
		fi.setFile(path + file);
		if (fi.isReadable() && !AvailableStyles.contains(file))
		{
			for (auto &&engine : RegisteredEngines)
			{
				StyleName = engine.second->isStyleValid(path + file);
				if (!StyleName.isNull())
				{
					AvailableStyles[StyleName].engine = engine.second.get();
					AvailableStyles[StyleName].global = false;
					break;
				}
			}
		}
	}

	path = Application::instance()->pathsProvider()->dataPath() + QLatin1String("syntax/chat/");
	dir.setPath(path);

	files = dir.entryList();

	foreach (const QString &file, files)
	{
		fi.setFile(path + file);
		if (fi.isReadable() && !AvailableStyles.contains(file))
		{
			for (auto &&engine : RegisteredEngines)
			{
				StyleName = engine.second->isStyleValid(path + file);
				if (!StyleName.isNull())
				{
					AvailableStyles[StyleName].engine = engine.second.get();
					AvailableStyles[StyleName].global = true;
					break;
				}
			}
		}
	}
}

bool ChatStyleManager::isChatStyleValid(const QString &name) const
{
	return AvailableStyles.contains(name) && AvailableStyles.value(name).engine;
}

StyleInfo ChatStyleManager::chatStyleInfo(const QString &name) const
{
	if (AvailableStyles.contains(name))
		return AvailableStyles.value(name);
	else
		return StyleInfo();
}

#include "moc_chat-style-manager.cpp"
