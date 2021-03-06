/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef PIXMAP_GRABBER_H
#define PIXMAP_GRABBER_H

#if defined(Q_OS_UNIX)
#include <X11/extensions/shape.h>
#include <vector>
#endif

class QPixmap;
class QRect;

class PixmapGrabber
{
	PixmapGrabber() {} // disable instances

public:
	// Methods from KSnapShot
	static QPixmap grabCurrent();
#if defined(Q_OS_UNIX)
	static Window findRealWindow(Window w, int depth = 0);
	static void getWindowsRecursive(std::vector<QRect> &windows, Window w, int rx = 0, int ry = 0, int depth = 0);
	static QPixmap grabWindow(Window child, int x, int y, uint w, uint h, uint border);
	static Window windowUnderCursor(bool includeDecorations);
#endif
};

#endif // PIXMAP_GRABBER_H
