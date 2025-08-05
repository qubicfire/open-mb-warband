#include "opengl.h"
#include "opengl_buffer.h"

OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* indices, const uint32_t size)
{
	initialize(indices, size);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
	glDeleteBuffers(1, &m_id);
}

void OpenGLIndexBuffer::initialize(const uint32_t* indices, const uint32_t size)
{
	glGenBuffers(1, &m_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		size * sizeof(uint32_t),
		&indices[0],
		GL_STATIC_DRAW);

	m_count = size;
}

OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, 
	const uint32_t count, 
	const uint32_t size,
	bool is_static_draw)
{
	initialize(vertices, count, size, is_static_draw);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	glDeleteBuffers(1, &m_id);
}

void OpenGLVertexBuffer::initialize(const void* vertices, 
	const uint32_t count, 
	const uint32_t size,
	bool is_static_draw)
{
	glGenBuffers(1, &m_id);
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	glBufferData(GL_ARRAY_BUFFER,
		count * size,
		vertices,
		is_static_draw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}
