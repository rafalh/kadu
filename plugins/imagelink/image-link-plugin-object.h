/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class DomProcessorService;
class ImageExpanderDomVisitorProvider;
class ImageLinkConfigurator;
class MainConfigurationWindowService;
class PathsProvider;
class VideoExpanderDomVisitorProvider;

class ImageLinkPluginObject : public QObject
{
	Q_OBJECT
	INJEQT_INSTANCE_IMMEDIATE

public:
	Q_INVOKABLE explicit ImageLinkPluginObject(QObject *parent = nullptr);
	virtual ~ImageLinkPluginObject();

private:
	QPointer<DomProcessorService> m_domProcessorService;
	QPointer<ImageExpanderDomVisitorProvider> m_imageExpanderDomVisitorProvider;
	QPointer<ImageLinkConfigurator> m_imageLinkConfigurator;
	QPointer<MainConfigurationWindowService> m_mainConfigurationWindowService;
	QPointer<PathsProvider> m_pathsProvider;
	QPointer<VideoExpanderDomVisitorProvider> m_videoExpanderDomVisitorProvider;

private slots:
	INJEQT_SET void setDomProcessorService(DomProcessorService *domProcessorService);
	INJEQT_SET void setImageExpanderDomVisitorProvider(ImageExpanderDomVisitorProvider *imageExpanderDomVisitorProvider);
	INJEQT_SET void setImageLinkConfigurator(ImageLinkConfigurator *imageLinkConfigurator);
	INJEQT_SET void setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_SET void setVideoExpanderDomVisitorProvider(VideoExpanderDomVisitorProvider *videoExpanderDomVisitorProvider);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

};
