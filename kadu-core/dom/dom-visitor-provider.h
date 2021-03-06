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

#ifndef DOM_VISITOR_PROVIDER_H
#define DOM_VISITOR_PROVIDER_H

#include "provider/provider.h"

class DomVisitor;

/**
 * @addtogroup Dom
 * @{
 */

/**
 * @class DomVisitorProvider
 * @short Provider that returns DomVisitor instances.
 * @author Rafał 'Vogel' Malinowski
 *
 * Register DomVisitorProvider in DomProcessorService to make them accessible everywhere.
 */
class DomVisitorProvider : public Provider<const DomVisitor *>
{
public:
	virtual ~DomVisitorProvider() {}

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Provide DomVisitor.
	 *
	 * This method can return null value.
	 */
	virtual const DomVisitor * provide() const = 0;

};

/**
 * @}
 */

#endif // DOM_VISITOR_PROVIDER_H
