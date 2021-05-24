#pragma once

#include "../core/bindable/Bindable.h"

namespace opengl
{
	class VertexBuffer : Bindable
	{
	public:
		VertexBuffer(void* data, unsigned int size);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;


	private:
		unsigned int m_Size;
		void* m_Data;
	};
}

