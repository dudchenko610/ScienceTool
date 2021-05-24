#include "VertexArray.h"

#include "../core/error/ErrorChecker.h"

#include "array-list/ArrayList.h"

opengl::VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &glId));
}

opengl::VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &glId));
}

void opengl::VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	this->Bind();
	vb.Bind();

	const collection::ArrayList<VertexBufferElement*>& elements = layout.GetElements();
	unsigned int offset = 0;

	int i = 0;

	elements.ForEach(
		[&i, &offset, &layout]
		(opengl::VertexBufferElement* element)
		{

			GLCall(glEnableVertexAttribArray(i));
			GLCall(glVertexAttribPointer(i, element->count, element->type,
				element->normalized, layout.GetStride(), (const void*)offset));

			offset += element->count * VertexBufferElement::GetSizeOfType(element->type);

			i++;
		}
	);
}


void opengl::VertexArray::Bind() const
{
	GLCall(glBindVertexArray(glId));
}

void opengl::VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}

