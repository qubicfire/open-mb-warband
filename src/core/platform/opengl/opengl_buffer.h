#ifndef _OPENGL_BUFFER_H
#define _OPENGL_BUFFER_H
#include "core/platform/buffer.h"

struct OpenGLVertexBuffer : mbcore::VertexBuffer
{
	OpenGLVertexBuffer(const void* vertices, 
		const uint32_t count,
		const uint32_t size,
		int flags);
	~OpenGLVertexBuffer();

	void bind() const override;
	void* map_buffer_range() const override;
protected:
	void initialize(const void* vertices, 
		const uint32_t count, 
		const uint32_t size,
		int flags) override;
private:
	uint32_t m_size;
};

struct OpenGLIndexBuffer : mbcore::IndexBuffer
{
	OpenGLIndexBuffer(const uint32_t* indices, const uint32_t size);
	~OpenGLIndexBuffer();
protected:
	void initialize(const uint32_t* indices, const uint32_t size) override;
};

#endif // !_OPENGL_BUFFER_H
