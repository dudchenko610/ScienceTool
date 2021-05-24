#include "VertexBuffer.h"

#include "../core/error/ErrorChecker.h"

opengl::VertexBuffer::VertexBuffer(void* data, unsigned int size) :
	m_Data(data), m_Size(size)
{
	GLCall(glGenBuffers(1, &glId));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, glId));
	GLCall(glBufferData(GL_ARRAY_BUFFER, m_Size, m_Data, GL_STATIC_DRAW));
}

opengl::VertexBuffer::~VertexBuffer()
{
	GLCall(glDeleteBuffers(1, &glId));
}

void opengl::VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, glId));
}

void opengl::VertexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}



