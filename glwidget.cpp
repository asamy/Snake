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
#include "glwidget.h"

#include <QMessageBox>
#include <QKeyEvent>
#include <QWheelEvent>

#include <time.h>
#include <math.h>

static const char *vertexSource =
	"uniform mat3 proj;\n"
	"varying vec2 TexCoord;\n"
	""
	"attribute vec2 texcoord;\n"
	"attribute vec2 vertex;\n"
	""
	"void main() {\n"
	"	gl_Position = vec4(proj * vec3(vertex.xy, 1), 1);\n"
	"	//gl_Position = proj * vec4(vertex.xy, 1.0, 1.0);\n"
	"	TexCoord = texcoord;\n"
	"}";

static const char *fragmentSource =
	"varying vec2 TexCoord;\n"
	"uniform sampler2D texture;\n"
	""
	"void main() {\n"
	"	gl_FragColor = texture2D(texture, TexCoord);\n"
	"}";

static GLubyte indices[] = {
	0, 1, 2,
	0, 2, 3
};

static GLfloat texcoord[] = {
	0, 0,
	0, 1,
	1, 1,
	1, 0
};

static const int maxZoomIn = 3;
static const int maxZoomOut = 512;
static const int deltaZoom = 2;

GLWidget::GLWidget(QWidget *parent)
	: QGLWidget(parent)
{
	m_snakePos = QPointF(float(width() / 2) - 32.f, float(height() / 2));
	m_zoom = 1.0f;

	setMouseTracking(true);
	setAutoBufferSwap(true);
}

GLWidget::~GLWidget()
{
	m_program.deleteLater();
	glDeleteTextures(1, &m_grassTexture);

	QList<GLuint> snakeTextures = m_snakeTextures.values();
	for (auto it : snakeTextures)
		glDeleteTextures(1, &it);
}

void GLWidget::renderSnake()
{
	float x = m_snakePos.x(), y = m_snakePos.y();
	GLfloat vertices[] = {
		x,    y,
		x,    y+32,
		x+32, y+32,
		x+32, y
	};

	glVertexAttribPointer(m_program.attributeLocation("vertex"), 2, GL_FLOAT, GL_FALSE,
			      0, vertices);
	glVertexAttribPointer(m_program.attributeLocation("texcoord"), 2, GL_FLOAT, GL_FALSE,
			      0, texcoord);
	glBindTexture(GL_TEXTURE_2D, m_currentSnakeTexture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
}

void GLWidget::initializeGL()
{
	glewExperimental = GL_TRUE;
	glewInit();

	if (!m_program.addShaderFromSourceCode(QGLShader::Vertex, vertexSource)) {
		QMessageBox::critical(this, "Vertex Shader", tr("Could not compile vertex shader: %1").arg(m_program.log()));
		return;
	}

	if (!m_program.addShaderFromSourceCode(QGLShader::Fragment, fragmentSource)) {
		QMessageBox::critical(this, "Fragment Shader", tr("Could not compile fragment shader: %1").arg(m_program.log()));
		return;
	}

	if (!m_program.link() || !m_program.bind()) {
		QMessageBox::critical(this, "Shader Program", tr("Could not bind the shader program: %1").arg(m_program.log()));
		return;
	}

	// Load the floor texture (aka Grass)
	QImage grassImage(32, 32, QImage::Format_ARGB32);
	if (!grassImage.load("textures/grass.png", "png")) {
		QMessageBox::critical(this, "Grass Texture", "Could not load textures/grass.png");
		return;
	}
	m_grassTexture = bindTexture(grassImage, GL_TEXTURE_2D, GL_RGBA);

	// Load all of the snake textures
	const QString directions[] = {
		"right",	// 0 - lookin right
		"left",		// 2 - lookin left
		"up",		// 3 - lookin up
		"down",		// 4 - lookin down
		QString()	// null terminator
	};

	for (int i = 0; directions[i] != QString(); ++i) {
		QImage snakeImage(32, 32, QImage::Format_ARGB32);
		if (!snakeImage.load(QString(tr("textures/snake_%1.png").arg(directions[i])), "png")) {
			QMessageBox::critical(this, "Snake Texture", tr("Could not load textures/snake_%1.png").arg(directions[i]));
			return;
		}

		GLuint texture;
		glGenTextures(1, &texture);

		texture = bindTexture(snakeImage, GL_TEXTURE_2D, GL_RGBA);
		m_snakeTextures[directions[i]] = texture;
	}
	m_currentSnakeTexture = m_snakeTextures["right"]; // Right direction is the default.

	m_program.enableAttributeArray("vertex");
	m_program.enableAttributeArray("texcoord");
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void GLWidget::paintGL()
{
	float x, y;

	glClear(GL_COLOR_BUFFER_BIT);
	for (x = 0, y = 0; y < height(); x += 32) {
		if (x == width()) {
			y += 32;
			x  = 0;
		}

		GLfloat vertices[] = {
			x,    y,
			x,    y+32,
			x+32, y+32,
			x+32, y
		};
		glVertexAttribPointer(m_program.attributeLocation("vertex"), 2, GL_FLOAT, GL_FALSE,
				      0, vertices);
		glVertexAttribPointer(m_program.attributeLocation("texcoord"), 2, GL_FLOAT, GL_FALSE,
				      0, texcoord);
		glBindTexture(GL_TEXTURE_2D, m_grassTexture);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	renderSnake();
	glDisable(GL_BLEND);
}

void GLWidget::resizeGL(int w, int h)
{
	GLfloat proj[] = {
		 2.0f/w*m_zoom,   0.0f,		 0.0f,
		 0.0f,		 -2.0f/h*m_zoom, 0.0f,
		-1.0f,		  1.0f,		 1.0f
	};

	glUniformMatrix3fv(m_program.uniformLocation("proj"), 1, GL_FALSE, proj);
	glViewport(0, 0, w, h);
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
	int key = event->key();
	/* We only handle up, down, left, right.  Disregard any other key  */
	if (key != Qt::Key_Down && key != Qt::Key_Up
		&& key != Qt::Key_Right && key != Qt::Key_Left) {
		event->ignore();
		return;
	}

	qreal& x = m_snakePos.rx();
	qreal& y = m_snakePos.ry();

	/*
	 * Screen:
	 *             y
	 *             |
	 *             |
	 *     -x -----0----- x
	 *             |
	 *             |
	 *            -y
	 */
	switch (key) {
	case Qt::Key_Right:
		// Moving right, positive x-axis
		x += 32;
		m_currentSnakeTexture = m_snakeTextures["right"];
		break;
	case Qt::Key_Left:
		// Moving left, negative x-axis
		x -= 32;
		m_currentSnakeTexture = m_snakeTextures["left"];
		break;
	case Qt::Key_Up:
		// Moving up, negative y-axis
		y -= 32;
		m_currentSnakeTexture = m_snakeTextures["down"];
		break;
	case Qt::Key_Down:
		// Moving down, positive y-axis.
		y += 32;
		m_currentSnakeTexture = m_snakeTextures["up"];
		break;
	}

	event->accept();
	updateGL();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
	double numDegrees = event->delta() / 8.0;
	double numSteps = numDegrees / 15.0;

	if (event->orientation() == Qt::Vertical)
		m_zoom *= std::pow(1.125, numSteps);
	else if (event->orientation() == Qt::Horizontal);
	event->accept();

	int w = width(), h = height();
	GLfloat proj[] = {
		 2.0f/w*m_zoom,   0.0f,		 0.0f,
		 0.0f,		 -2.0f/h*m_zoom, 0.0f,
		-1.0f,		  1.0f,		 1.0f
	};

	glUniformMatrix3fv(m_program.uniformLocation("proj"), 1, GL_FALSE, proj);
	updateGL(); // re-render scene
}
