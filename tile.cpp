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
#include "tile.h"

#include <algorithm>

Tile::Tile(const Point& pos)
	: m_pos(pos)
{

}

Tile::~Tile()
{
	m_textures.clear();
}

void Tile::addTexture(const TexturePtr& texture)
{
	m_textures.push_back(texture);
}

void Tile::removeTexture(const TexturePtr& texture)
{
	auto it = std::find_if(m_textures.begin(), m_textures.end(),
				[=] (const TexturePtr& tex) { return tex->id() == texture->id(); } );
	if (it != m_textures.end())
		m_textures.erase(it);
}

TexturePtr Tile::popTexture()
{	
	auto it = m_textures.end() - 1;
	m_textures.erase(it);
	return *it;
}

