/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef X11_SCREEN_MODE_CHECKER_H
#define X11_SCREEN_MODE_CHECKER_H

#include <X11/Xlib.h>
#include <time.h>

#if defined(Q_OS_UNIX)
#include "os/x11/x11tools.h" // this should be included as last one,
#undef KeyPress
#undef Status            // and Status defined by Xlib.h must be undefined
#include <storage/custom-properties.h>
#endif

#include "screen-mode-checker.h"


class KADUAPI X11ScreenModeChecker : public ScreenModeChecker
{
	Display *x11display;
public:
	X11ScreenModeChecker();
	virtual ~X11ScreenModeChecker();

	bool isFullscreenAppActive();
	bool isScreensaverActive();
	bool isDummy() { return false; }
};

#endif // X11_SCREEN_MODE_CHECKER_H
