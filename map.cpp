/*
 * Copyright (c) 2013 Ahmed Samy  <f.fallen45@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "map.h"

#include <algorithm>
#include <assert.h>

void Map::addTile(const TilePtr& tile)
{
	m_tiles.push_back(tile);
}

void Map::removeTile(const Point& pos)
{
	auto it = std::find_if(m_tiles.begin(), m_tiles.end(),
				[=] (const TilePtr& tile) -> bool { return tile->pos() == pos; } );
	if (it != m_tiles.end())
		m_tiles.erase(it);
}

TilePtr Map::getTile(const Point& pos) const
{
	auto it = std::find_if(m_tiles.begin(), m_tiles.end(),
				[=] (const TilePtr& tile) -> bool { return tile->pos() == pos; } );
	if (it != m_tiles.end())
		return *it;
	return nullptr;
}

TilePtr Map::getRandomTile() const
{
	auto begin = m_tiles.begin();
	auto end = m_tiles.end();

	const unsigned long n = std::distance(begin, end);
	const unsigned long divisor = RAND_MAX / n;

	unsigned long k;
	do
		k = std::rand() / divisor;
	while (k >= n);

	std::advance(begin, k);
	return *begin;
}

