#include "Vertices.h"

parser::Vertices::Vertices(int size, int trianglesCount) : m_Size(size), m_TrianglesCount(trianglesCount)
{
	this->m_Data = new void* [size];
}

parser::Vertices::~Vertices()
{
	delete[] this->m_Data;
}

void* parser::Vertices::GetData()
{
	return this->m_Data;
}

int parser::Vertices::GetSize()
{
	return this->m_Size;
}

int parser::Vertices::GetTrianglesCount()
{
	return this->m_TrianglesCount;
}


