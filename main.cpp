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

#include <GL/glut.h>
#include <ctime>
#include <iostream>

Game g_game;

static void timedout(int unused)
{
	g_game.updateSnakePos();
	glutPostRedisplay();
	glutTimerFunc(g_game.getTimer(), timedout, 0);
}

static void asciiKey(unsigned char key, int x, int y)
{
	Direction_t dir = DIRECTION_INVALID;
	switch (key) {
	case 27:	// Escape key
		exit(EXIT_SUCCESS);
		break;
	case 'w': 	// Move up
		dir = DIRECTION_UP;
		break;
	case 's': 	// Move down
		dir = DIRECTION_DOWN;
		break;
	case 'a': 	// Move left
		dir = DIRECTION_LEFT;
		break;
	case 'd':	// Move right
		dir = DIRECTION_RIGHT;
		break;
	}

	g_game.setSnakeDirection(dir);
	glutPostRedisplay();
}

static void specialKey(int key, int x, int y)
{
	Direction_t dir = DIRECTION_INVALID;
	switch (key) {
	case GLUT_KEY_UP:
		dir = DIRECTION_UP;
		break;
	case GLUT_KEY_DOWN:
		dir = DIRECTION_DOWN;
		break;
	case GLUT_KEY_RIGHT:
		dir = DIRECTION_RIGHT;
		break;
	case GLUT_KEY_LEFT:
		dir = DIRECTION_LEFT;
		break;
	}

	g_game.setSnakeDirection(dir);
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	srand(std::time(nullptr));
	glutInit(&argc, argv);
	glutInitWindowSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Snake");

	glutReshapeFunc([] (int w, int h) { g_game.resize(w, h); } );
	glutDisplayFunc([] {
				g_game.render();
				glutSwapBuffers();

				static bool firstCall = true;
				if (firstCall) {
					glutTimerFunc(1000, timedout, 0);
					firstCall = false;
				}
			   }
			);
	glutKeyboardFunc(&asciiKey);
	glutSpecialFunc(&specialKey);
	glutMouseFunc( []
		(int button, int state, int x, int y) {
		if (button == 3 || button == 4) {
			// Wheel Event
			if (state == GLUT_UP)
				return; // Disregard redundant GLUT_UP events

			float zoom;
			if (button == 3)	// Zoom in.
				zoom =  0.1f;
			else			// Zoom out
				zoom = -0.1f;

			g_game.updateZoom(zoom);
			glutPostRedisplay();
		}}
	);

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

	glutMainLoop();
	return 0;
}

