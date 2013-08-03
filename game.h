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
#ifndef GAME_H
#define GAME_H

#include "map.h"
#include "snake.h"
#include "shaderprogram.h"

static const char *directions[] = {
	"right",	// 0 - lookin right
	"left",		// 1 - lookin left
	"up",		// 2 - lookin up
	"down",		// 3 - lookin down
	NULL
};

#define Position 0
#define TexCoord 1

#define DEFAULT_WIDTH 400
#define DEFAULT_HEIGHT 400

class Game
{
public:
	Game();
	~Game();

	bool initialize();
	void render();
	void resize(int w, int h);
	void updateZoom(float f);

	int getTimer() const { return m_lastInterval; }
	void setSnakeDirection(Direction_t dir);
	void updateSnakePos();

protected:
	void createMapTiles();
	void makeApple();
	void renderAt(const Point& pos, const TexturePtr& texture);
	void updateProjectionMatrix();

private:
	int m_width;
	int m_height;
	int m_viewportWidth;
	int m_viewportHeight;
	int m_lastInterval;
	int m_applesEaten;
	float m_zoom;
	bool m_newApple;

	Map m_map;
	ShaderProgram m_program;

	TexturePtr m_grassTexture;

	std::array<TexturePtr, sizeof(directions) / sizeof(directions[0])> m_snakeTextures;
	std::array<TexturePtr, 8> m_appleTextures;

	Snake *m_snake;
	TilePtr m_appleTile;
};

extern Game g_game;

#endif

