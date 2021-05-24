#include "IndexBuffer.h"

#include "../core/error/ErrorChecker.h"

opengl::IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
	: m_Count(count), m_Data(data)
{
	ASSERT(sizeof(unsigned int) == sizeof(GLuint));

	GLCall(glGenBuffers(1, &glId));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glId));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Count * sizeof(unsigned int), m_Data, GL_STATIC_DRAW));
}

opengl::IndexBuffer::~IndexBuffer()
{
	GLCall(glDeleteBuffers(1, &glId));
}

void opengl::IndexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glId));
}

void opengl::IndexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}


