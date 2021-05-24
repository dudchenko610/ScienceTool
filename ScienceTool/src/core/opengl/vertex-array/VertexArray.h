#pragma once

#include "../core/bindable/Bindable.h"

#include "../vertex-buffer/VertexBuffer.h"
#include "../vertex-buffer/layout/VertexBufferLayout.h"


namespace opengl
{
	class VertexArray : Bindable
	{
	public:
		VertexArray();
		~VertexArray();

		void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

		void Bind() const;
		void Unbind() const;

	};
}

