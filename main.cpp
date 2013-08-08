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
#include "timer.h"

#include <GLFW/glfw3.h>
#include <ctime>
#include <iostream>

Game g_game;

static void keyPress(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	Direction_t dir = DIRECTION_INVALID;
	if (action != GLFW_PRESS)
		return;

	// Keybad movement is supported
	// 7 (NW)    8 (N)	9 (NE)
	// 4 (W)     5 (unsup)  6 (E)
	// 1 (SW)    2 (S)      3 (SE)
	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_UP:
	case GLFW_KEY_W:
	case GLFW_KEY_KP_8:
		dir = DIRECTION_NORTH;
		break;
	case GLFW_KEY_DOWN:
	case GLFW_KEY_S:
	case GLFW_KEY_KP_2:
		dir = DIRECTION_SOUTH;
		break;
	case GLFW_KEY_RIGHT:
	case GLFW_KEY_D:
	case GLFW_KEY_KP_6:
		dir = DIRECTION_EAST;
		break;
	case GLFW_KEY_LEFT:
	case GLFW_KEY_A:
	case GLFW_KEY_KP_4:
		dir = DIRECTION_WEST;
		break;
	// Diagonal movement
	case GLFW_KEY_KP_9:
	case GLFW_KEY_E:
		dir = DIRECTION_NORTHEAST;
		break;
	case GLFW_KEY_KP_7:
	case GLFW_KEY_Q:
		dir = DIRECTION_NORTHWEST;
		break;
	case GLFW_KEY_KP_1:
	case GLFW_KEY_Z:
		dir = DIRECTION_SOUTHWEST;
		break;
	case GLFW_KEY_KP_3:
	case GLFW_KEY_C:
		dir = DIRECTION_SOUTHEAST;
		break;
	default:
		return;
	}

	g_game.setSnakeDirection(dir);
}

static void error_callback(int error, const char *description)
{
	fputs(description, stderr);
}

int main(int argc, char **argv)
{
	GLFWwindow *window;

	srand(std::time(nullptr));
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		return 1;

	window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Snake", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyPress);
	glfwSetFramebufferSizeCallback(window,
				[] (GLFWwindow *window, int w, int h) {
					g_game.resize(w, h);
				});
	glfwSetScrollCallback(window,
				[] (GLFWwindow *window, double x, double y) {
					float newZoom = g_game.getZoom();
					newZoom += (float)y / 4.f;
					if (newZoom < 0)
						newZoom = 1.0f;
					g_game.setZoom(newZoom);
				});


	glewExperimental = GL_TRUE;
	const GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
		return 1;
	}

	std::cout << "OpenGL Vendor:   " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL Version:  " << glGetString(GL_VERSION) << std::endl;

	if (!g_game.initialize()) {
		std::cerr << "Failed to initialize Game state" << std::endl;
		return 1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	g_game.resize(width, height);

	Timer timer;
	timer.setTimerFunc(std::bind(&Game::updateSnakePos, &g_game));
	while (!glfwWindowShouldClose(window)) {
		g_game.render();
		glfwSwapBuffers(window);
		glfwPollEvents();

		timer.setInterval(g_game.getWaitInterval());
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

