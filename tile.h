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
#ifndef TILE_H
#define TILE_H

#include "point.h"
#include "texture.h"

#include <vector>

class Tile
{
public:
	Tile(const PointF& pos);
	~Tile();

	void addTexture(const TexturePtr& texture);
	void removeTexture(const TexturePtr& texture);
	TexturePtr popTexture();

	PointF pos() const { return m_pos; }
	PointF& pos() { return m_pos; }
	void setPos(const PointF& pos) { m_pos = pos; }

	void clear() { m_textures.clear(); }
	std::vector<TexturePtr> getTextures() const { return m_textures; }

private:
	PointF m_pos;
	std::vector<TexturePtr> m_textures;
};

typedef std::shared_ptr<Tile> TilePtr;

#endif

