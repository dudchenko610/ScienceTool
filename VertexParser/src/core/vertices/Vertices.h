#pragma once

namespace parser
{
	class Vertices
	{
	public:
		Vertices(int size, int trianglesCount);
		~Vertices();
	public:
		void* GetData();
		int GetSize();
		int GetTrianglesCount();
	private:
		int   m_TrianglesCount;
		int   m_Size;
		void* m_Data;

	};
}


