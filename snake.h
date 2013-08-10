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
#ifndef SNAKE_H
#define SNAKE_H

#include "tile.h"

typedef enum Direction {
	DIRECTION_NORTH,
	DIRECTION_SOUTH,
	DIRECTION_EAST,
	DIRECTION_WEST,

	// Diagonal movement
	DIRECTION_NORTHWEST,
	DIRECTION_NORTHEAST,
	DIRECTION_SOUTHWEST,
	DIRECTION_SOUTHEAST,

	DIRECTION_INVALID
} Direction_t;

class Snake
{
public:
	Snake()
		: m_tile(nullptr), m_dir(DIRECTION_INVALID),
		  m_health(50)
	{ }
	~Snake() { m_tile = nullptr; }

	void setPos(const Point& pos) { m_tile->setPos(pos); }
	Point pos() const { return m_tile->pos(); }

	TilePtr tile() const { return m_tile; }
	void setTile(const TilePtr& tile) { m_tile = tile; }

	void setTexture(const TexturePtr& texture)
	{
		// Make sure we are not removing the ground texture.
		if (m_tile->getTextures().size() > 1)
			m_tile->popTexture();
		m_tile->addTexture(texture);
	}

	Direction_t direction() const { return m_dir; }
	void setDirection(Direction_t newDir) { m_dir = newDir; }

	bool dead() const { return m_health <= 0; }
	int eat(int health)
	{
		m_health += health;
		return m_health;
	}
	Point move()
	{ 
		/*
		 * Psuedo Geometric Screen:
		 *             y
		 *	       |
		 *             |
		 *     -x -----0----- x
		 *             |
		 *             |
		 *            -y
		 */
		int x = m_tile->pos().x();
		int y = m_tile->pos().y();

		switch (m_dir) {
		case DIRECTION_NORTH:
			y += 32;
			break;
		case DIRECTION_SOUTH:
			y -= 32;
			break;
		case DIRECTION_EAST:
			x += 32;
			break;
		case DIRECTION_WEST:
			x -= 32;
			break;
		// Diagonal movement
		case DIRECTION_NORTHWEST:
			x -= 32;
			y += 32;
			break;
		case DIRECTION_NORTHEAST:
			x += 32;
			y += 32;
			break;
		case DIRECTION_SOUTHWEST:
			x -= 32;
			y -= 32;
			break;
		case DIRECTION_SOUTHEAST:
			x += 32;
			y -= 32;
			break;
		default:
			return pos();
		}

		return Point(x, y);
	}

private:
	TilePtr m_tile;
	Direction_t m_dir;
	int m_health;
};

#endif

