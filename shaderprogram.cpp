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
#include "shaderprogram.h"

#include <iostream>
#include <string.h>
#include <stdlib.h>

static GLuint compileShader(const GLenum type, const char *src)
{
	GLint len;
	GLuint ret;
	GLint status;

	if (!src)
		return 0;

	len = strlen(src);
	if (len == 0)
		return 0;

	ret = glCreateShader(type);
	glShaderSource(ret, 1, (const GLchar **)&src, &len);
	glCompileShader(ret);

	glGetShaderiv(ret, GL_COMPILE_STATUS, &status);
	if (!status) {
		glDeleteShader(ret);
		return 0;
	}

	return ret;
}

ShaderProgram::ShaderProgram()
{
}

ShaderProgram::~ShaderProgram()
{
	for (unsigned i = 0; i < m_shaders.size(); ++i)
		glDeleteShader(m_shaders[i]);
	glDeleteProgram(m_programId);
}

void ShaderProgram::create()
{
	m_programId = glCreateProgram();
}

bool ShaderProgram::compile(const GLenum shaderType, const char *sourceCode)
{
	GLuint shaderId = compileShader(shaderType, sourceCode);
	if (!shaderId) {
		std::cerr << "Failed to compile shader of type: " << (shaderType == GL_VERTEX_SHADER ? "Vertex" : "Fragment")
			  << ", reason: " << log() << std::endl;
		return false;
	}

	glAttachShader(m_programId, shaderId);
	m_shaders.push_back(shaderId);
	return true;
}

void ShaderProgram::bindAttribLocation(GLint location, const char *name)
{
	glBindAttribLocation(m_programId, location, name);
	glEnableVertexAttribArray(location);
}

void ShaderProgram::setVertexData(GLint attribLoc, const GLvoid *values, GLint size)
{
	return glVertexAttribPointer(attribLoc, size, GL_FLOAT, GL_FALSE, 0, values);
}

void ShaderProgram::setProjectionMatrix(const GLfloat *values)
{
	GLint loc = glGetUniformLocation(m_programId, "proj");
	if (loc < 0)
		return;

	return glUniformMatrix3fv(loc, 1, GL_FALSE, values);
}

bool ShaderProgram::link()
{
	glLinkProgram(m_programId);

	int status = GL_FALSE;
	glGetProgramiv(m_programId, GL_LINK_STATUS, &status);
	return !!(status != GL_FALSE);
}

void ShaderProgram::bind()
{
	glUseProgram(m_programId);
}

std::string ShaderProgram::log()
{
	int infoLogLength = 0;
	std::string infoLog;

	glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 1) {
		std::vector<char> buf(infoLogLength);
		glGetShaderInfoLog(m_programId, infoLogLength - 1, NULL, &buf[0]);
		infoLog = &buf[0];
	}

	return infoLog;
}

