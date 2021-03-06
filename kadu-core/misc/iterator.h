/*
 * %kadu copyright begin%
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

#pragma once

#include <functional>

template<typename Iterator, typename Type>
class IteratorWrapper
{

public:
	using Converter = std::function<Type(Iterator)>;

	inline IteratorWrapper(Iterator iterator, Converter converter) : m_iterator{iterator}, m_converter{converter} {}

	friend inline bool operator == (const IteratorWrapper &x, const IteratorWrapper &y)
	{
		return x.m_iterator == y.m_iterator;
	}

	friend inline bool operator != (const IteratorWrapper &x, const IteratorWrapper &y)
	{
		return !(x == y);
	}

	inline IteratorWrapper & operator ++ ()
	{
		++m_iterator;
		return *this;
	}

	inline IteratorWrapper operator ++ (int)
	{
		auto copy = m_iterator;
		m_iterator++;
		return IteratorWrapper{copy, m_converter};
	}

	inline Type operator * () const
	{
		return m_converter(m_iterator);
	}

private:
	Iterator m_iterator;
	Converter m_converter;

};
