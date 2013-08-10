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
#include "game.h"
#include "shadersources.h"

#include <iostream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <assert.h>

Game::Game() :
	  m_width(DEFAULT_WIDTH),
	  m_height(DEFAULT_HEIGHT),
	  m_lastInterval(190),
	  m_zoom(1.0f),
	  m_newFood(true),
	  m_snake(nullptr),
	  m_foodTile(nullptr)
{
}

Game::~Game()
{
	m_map.clear();
	delete m_snake;
}

TilePtr Game::getRandomTile() const
{
	TilePtr ret;
	TilePtr snakeTile = m_snake->tile();
	Point offscreenPoint(m_viewportWidth, m_viewportHeight);

	do
		ret = m_map.getRandomTile();
	while (ret && ret->pos() != snakeTile->pos() && ret->pos() != offscreenPoint);
	return ret;
}

void Game::createMapTiles()
{
	TilePtr newTile;

	int x, y;
	for (x = 0, y = 0 ;; x += 32) {
		if (x >= m_width) {
			m_viewportWidth = x - 32;
			x = 0;
			y += 32;
		}
		// Make sure we don't render offscreen
		if (y >= m_height)
			break;

		newTile = TilePtr(new Tile(Point(x, y)));
		newTile->addTexture(m_grassTexture);
		m_map.addTile(newTile);
	}

	m_viewportHeight = y - 32;
}

void Game::makeFood()
{
	if (!m_newFood)
		return;

	const auto& textures = !(rand() % 5) ? m_baitTextures : m_appleTextures;
	int randomIndex = rand() % 8;
	const TexturePtr& foodTex = textures[randomIndex];
	if (!foodTex) {
		std::cerr << "Internal error: No apple texture at index " << randomIndex << ", aborting..." << std::endl;
		std::abort();
	}

	/* Figure out place position.  */
	const TilePtr& placeTile = getRandomTile();
	if (!placeTile) {
		/* Impossible to reach here...  */
		std::cerr << "Internal error: Could find a suitable tile to place the food over, aborting..." << std::endl;
		std::abort();
	}

	m_foodTile = placeTile;
	m_foodTile->addTexture(foodTex);
	m_newFood = false;
}

bool Game::initialize()
{
	m_program.create();
	if (!m_program.compile(GL_VERTEX_SHADER, vertexSource))
		return false;

	if (!m_program.compile(GL_FRAGMENT_SHADER, fragmentSource))
		return false;

	m_program.bindAttribLocation(Position, "vertex");
	m_program.bindAttribLocation(TexCoord, "texcoord");
	if (!m_program.link()) {
		std::cerr << "Failed to link the GL shader program: " << m_program.log() << std::endl;
		return false;
	}
	m_program.bind();

	m_grassTexture = TexturePtr(new Texture);
	if (!m_grassTexture->loadTexture("textures/grass.png")) {
		std::cerr << "Failed to load the grass texture." << std::endl;
		return false;
	}

	for (int i = 0; directions[i]; ++i) {
		std::stringstream ss;
		ss << "textures/snake_" << directions[i] << ".png";

		TexturePtr newTexture(new Texture);
		if (!newTexture->loadTexture(ss.str())) {
			std::cerr << "Failed to load Snake Texture from: " << ss.str() << std::endl;
			continue;
		}

		m_snakeTextures[i] = newTexture;
	}
	m_snake = new Snake;

	for (int i = 1; i < 9; i++) {
		std::stringstream ss;
		ss << "textures/food/apple" << i << ".png";

		TexturePtr newTexture(new Texture);
		if (!newTexture->loadTexture(ss.str())) {
			std::cerr << "Failed to load Apple Texture from: " << ss.str() << std::endl;
			continue;
		}
		m_appleTextures[i - 1] = newTexture;
	}

	for (int i = 1; i < 9; ++i) {
		std::stringstream ss;
		ss << "textures/food/strawberry" << i << ".png";

		TexturePtr newTexture(new Texture);
		if (!newTexture->loadTexture(ss.str())) {
			std::cerr << "Failed to load Strawberry texture from: " << ss.str() << std::endl;
			continue;
		}
		m_baitTextures[i - 1] = newTexture;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	return true;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	for (const TilePtr& tile : m_map.getTiles())
		for (const TexturePtr& texture : tile->getTextures())
			renderAt(tile->pos(), texture);
	if (m_newFood)
		makeFood();
}

void Game::resize(int w, int h)
{
	m_width  = w;
	m_height = h;

	glViewport(0, 0, w, h);
	updateProjectionMatrix();

	m_map.clear();
	createMapTiles();

	static bool firstTime = true;
	if (firstTime) {
		m_snake->setTile(m_map.getTile(Point(32, 32)));
		m_snake->setTexture(m_snakeTextures[0]);
		m_snake->setDirection(DIRECTION_EAST);
		firstTime = false;
	}

	if (m_foodTile) {
		Point foodPos = m_foodTile->pos();
		// If we are resized from a high size into
		// a low one, then we need to reposition the
		// apple to fit the scene viewport.
		// However, instead of doing such job,
		// we will just create new food elsewhere.
		if (foodPos.x() >= w || foodPos.y() >= h) {
			const TilePtr& placeTile = getRandomTile();
			if (!placeTile) {
				/* Impossible to reach here...  */
				std::cerr << "Internal error: Could find a suitable tile to place the food over, aborting..." << std::endl;
				std::abort();
			}

			const TexturePtr& foodTexture = m_foodTile->getTextures()[1];
			m_foodTile = placeTile;
			m_foodTile->addTexture(foodTexture);
		} else {
			// Remove the tile at the last food position
			m_map.removeTile(foodPos);
			// Now render the apple at it's previous position.
			m_map.addTile(m_foodTile);
		}
	}
}

void Game::updateProjectionMatrix()
{
	float w = m_width;
	float h = m_height;
	//  Coordinate      Projection Matrix			        GL Coordinate (Transformed)
	//                  | 2.0 / width |   0.0           | 0.0  |
	//  | x  y  1 |  *  | 0.0         |  -2.0 / height  | 0.0  | =  | x' y' 1 |
	//                  |-1.0         |   1.0           | 1.0  |
	GLfloat projectionMatrix[] = {
		 2.0f/w*m_zoom,	  0.0f,		 0.0f,
		 0.0f,		  2.0f/h*m_zoom, 0.0f,
		-1.0f,		 -1.0f,		 1.0f
	};

	m_program.setProjectionMatrix(projectionMatrix);
}

void Game::setSnakeDirection(Direction_t dir)
{
	if (m_snake->dead())
		return;

	m_snake->setDirection(dir);
	TexturePtr newTexture;
	// Have a look at Game::initialize() for the trivial indices.
	switch (dir) {
	case DIRECTION_NORTH:
		newTexture = m_snakeTextures[3];
		break;
	case DIRECTION_SOUTH:
		newTexture = m_snakeTextures[2];
		break;
	case DIRECTION_EAST:
	case DIRECTION_NORTHEAST:
	case DIRECTION_SOUTHEAST:
		newTexture = m_snakeTextures[0];
		break;
	case DIRECTION_WEST:
	case DIRECTION_NORTHWEST:
	case DIRECTION_SOUTHWEST:
		newTexture = m_snakeTextures[1];
		break;
	case DIRECTION_INVALID:
	default:
		std::cerr << "Invalid direction!" << std::endl;
		return;
	}
	m_snake->setTexture(newTexture);
}

void Game::updateSnakePos()
{
	if (m_snake->dead())
		return;

	// Snake Position Controller
	Point movePos = m_snake->move();
	if (m_foodTile) {
		eatApple(movePos);	// First try, don't know if offscreen yet...
		movePos.checkBounds(m_viewportWidth, m_viewportHeight);
		eatApple(movePos);	// Second try, if offscreen eat apple and switch position.
	}

	TilePtr moveTile = m_map.getTile(movePos);
	if (!moveTile) {
		std::cerr << "Internal error: Failed to find a tile to move the snake on."
			<< " Move pos: " << movePos << std::endl;
		return;
	}

	moveTile->addTexture(m_snake->tile()->popTexture());
	m_snake->setTile(moveTile);
}

void Game::removeFood()
{
	if (!m_foodTile || m_newFood)
		return;

	const auto& textures = m_foodTile->getTextures();
	if (textures.size() > 1) {
		m_foodTile->removeTexture(textures[1]);
		m_newFood = true;
	}
}

void Game::eatApple(const Point& foodPos)
{
	const auto& textures = m_foodTile->getTextures();

	if (textures.size() > 1 && foodPos == m_foodTile->pos()) {
		int damage = 0;
		const TexturePtr& foodTexture = textures[1];
		if (foodTexture) {
			for (int i = 0; i < 8; ++i) {
				if (foodTexture == m_appleTextures[i]) {
					damage = i;
					break;
				} else if (foodTexture == m_baitTextures[i]) {
					damage = -i;
					break;
				}
			}
		}

		int hp = m_snake->eat(damage);
		if (hp) {
			m_newFood = true;
			if (m_lastInterval - (hp / 6) >= 75)
				m_lastInterval -= hp / 6;
		}

		m_foodTile->removeTexture(foodTexture);
	}
}

void Game::renderAt(const Point& pos, const TexturePtr& texture)
{
	static const GLubyte indices[] = {
		0, 1, 2,
		0, 2, 3
	};
	static const GLfloat texcoord[] = {
		0, 0,
		1, 0,
		1, 1,
		0, 1
	};

	float x = std::floor(pos.x() / 32.f) * 32.f;
	float y = std::floor(pos.y() / 32.f) * 32.f;

	const GLfloat vertices[] = {
		x,	y,
		x + 32, y,
		x + 32, y + 32,
		x,	y + 32
	};

	m_program.setVertexData(Position, vertices, 2);
	m_program.setVertexData(TexCoord, texcoord, 2);

	texture->bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
}

