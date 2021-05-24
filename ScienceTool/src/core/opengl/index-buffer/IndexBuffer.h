#pragma once

#include "../core/bindable/Bindable.h"

namespace opengl
{
	class IndexBuffer : Bindable
	{
	public:
		IndexBuffer(const unsigned int* data, unsigned int count);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;


		inline unsigned int GetCount() const { return m_Count; }
	private:
		unsigned int m_Count;
		const unsigned int* m_Data;
	};
}