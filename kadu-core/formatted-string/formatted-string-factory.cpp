/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QFileInfo>
#include <QtGui/QTextBlock>
#include <QtGui/QTextDocument>

#include "dom/dom-processor.h"
#include "formatted-string/composite-formatted-string.h"
#include "formatted-string/force-nbsp-dom-visitor.h"
#include "formatted-string/formatted-string-image-block.h"
#include "formatted-string/formatted-string-text-block.h"
#include "misc/memory.h"
#include "services/image-storage-service.h"

#include "formatted-string-factory.h"

#include <QtXml/QDomDocument>

FormattedStringFactory::FormattedStringFactory()
{
}

FormattedStringFactory::~FormattedStringFactory()
{
}

void FormattedStringFactory::setImageStorageService(ImageStorageService *imageStorageService)
{
	CurrentImageStorageService = imageStorageService;
}

std::unique_ptr<FormattedString> FormattedStringFactory::fromPlainText(const QString& plainText)
{
	return std::make_unique<FormattedStringTextBlock>(plainText, false, false, false, QColor{});
}

std::unique_ptr<FormattedString> FormattedStringFactory::partFromQTextCharFormat(const QTextCharFormat &textCharFormat, const QString &text)
{
	QString replacedNewLine = text;
	replacedNewLine.replace("\u00A0", " ");
	replacedNewLine.replace(QChar::LineSeparator, '\n');
	return std::make_unique<FormattedStringTextBlock>(replacedNewLine, textCharFormat.font().bold(), textCharFormat.font().italic(), textCharFormat.font().underline(), textCharFormat.foreground().color());
}

std::unique_ptr<FormattedString> FormattedStringFactory::partFromQTextImageFormat(const QTextImageFormat& textImageFormat)
{
	QString filePath = textImageFormat.name();
	QFileInfo fileInfo(filePath);

	if (CurrentImageStorageService)
		filePath = CurrentImageStorageService.data()->storeImage(filePath);

	return std::make_unique<FormattedStringImageBlock>(filePath);
}

std::unique_ptr<FormattedString> FormattedStringFactory::partFromQTextFragment(const QTextFragment &textFragment, bool prependNewLine)
{
	QTextCharFormat format = textFragment.charFormat();
	if (!format.isImageFormat())
		return partFromQTextCharFormat(format, prependNewLine ? '\n' + textFragment.text() : textFragment.text());
	else
		return partFromQTextImageFormat(format.toImageFormat());
}

std::vector<std::unique_ptr<FormattedString>> FormattedStringFactory::partsFromQTextBlock(const QTextBlock &textBlock, bool firstBlock)
{
	auto result = std::vector<std::unique_ptr<FormattedString>>{};

	bool firstFragment = true;
	for (QTextBlock::iterator it = textBlock.begin(); !it.atEnd(); ++it)
	{
		if (!it.fragment().isValid())
			continue;

		auto part = partFromQTextFragment(it.fragment(), !firstBlock && firstFragment);
		if (part && !part->isEmpty())
		{
			result.push_back(std::move(part));
			firstFragment = false;
		}
	}

	return result;
}

std::unique_ptr<FormattedString> FormattedStringFactory::fromHtml(const QString &html)
{
	QDomDocument domDocument;
	// force content to be valid HTML with only one root
	domDocument.setContent(QString{"<div>%1</div>"}.arg(html));

	ForceNbspDomVisitor forceNbspDomVisitor{};
	auto domProcessor = DomProcessor{domDocument};
	domProcessor.accept(&forceNbspDomVisitor);

	if (domDocument.documentElement().childNodes().isEmpty())
		return fromTextDocument(QTextDocument{});

	auto result = domDocument.toString(-1).trimmed();
	// remove <div></div>
	Q_ASSERT(result.startsWith(QStringLiteral("<div>")));
	Q_ASSERT(result.endsWith(QStringLiteral("</div>")));
	result = result.mid(static_cast<int>(qstrlen("<div>")), result.length() - static_cast<int>(qstrlen("<div></div>")));

	QTextDocument document;
	document.setHtml(result);

	return fromTextDocument(document);
}

std::unique_ptr<FormattedString> FormattedStringFactory::fromTextDocument(const QTextDocument &textDocument)
{
	auto firstBlock = true;
	auto items = std::vector<std::unique_ptr<FormattedString>>{};

	QTextBlock block = textDocument.firstBlock();
	while (block.isValid())
	{
		auto parts = partsFromQTextBlock(block, firstBlock);
		for (auto &&part : parts)
			items.push_back(std::move(part));

		block = block.next();
		firstBlock = false;
	}

	return std::make_unique<CompositeFormattedString>(std::move(items));
}

std::unique_ptr<FormattedString> FormattedStringFactory::fromText(const QString &text)
{
	QScopedPointer<QTextDocument> document(new QTextDocument());
	if (isHtml(text))
		document->setHtml(text);
	else
		document->setPlainText(text);

	return fromTextDocument(*document.data());
}

bool FormattedStringFactory::isHtml(const QString &text) const
{
	return text.contains('<');
}

#include "moc_formatted-string-factory.cpp"
