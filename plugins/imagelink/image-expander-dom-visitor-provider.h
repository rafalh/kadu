/*
 * %kadu copyright begin%
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

#pragma once

#include "dom/ignore-links-dom-visitor.h"
#include "dom/dom-visitor-provider.h"

#include "configuration/image-link-configuration.h"

#include <QtCore/QObject>

class ImageExpanderDomVisitorProvider : public QObject, public DomVisitorProvider
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ImageExpanderDomVisitorProvider(QObject *parent = nullptr);
	virtual ~ImageExpanderDomVisitorProvider();

	virtual const DomVisitor * provide() const;

	void setConfiguration(const ImageLinkConfiguration &configuration);

private:
	ImageLinkConfiguration m_configuration;
	IgnoreLinksDomVisitor m_ignoreLinksVisitor;

};
