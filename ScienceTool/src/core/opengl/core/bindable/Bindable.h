#pragma once

namespace opengl 
{

	class Bindable
	{
	public:
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

	public:
		const unsigned int GetId() const
		{
			return this->glId;
		}
	protected:
		unsigned int glId;
	};


}
