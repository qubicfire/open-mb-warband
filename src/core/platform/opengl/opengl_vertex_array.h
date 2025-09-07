#ifndef _OPENGL_VERTEX_ARRAY_H
#define _OPENGL_VERTEX_ARRAY_H
#include "core/platform/vertex_array.h"

struct OpenGLVertexArray : mbcore::VertexArray
{
	OpenGLVertexArray(int flags);
	~OpenGLVertexArray();

	void bind() const override;
	void unbind() const override;
	void link(uint32_t index,
		VertexType type,
		int stride,
		const void* pointer,
		bool normalized = false) const override;

	void initialize() override;
};

#endif // !_OPENGL_VERTEX_ARRAY_H
