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
	int flags)
{
	initialize(vertices, count, size, flags);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	glDeleteBuffers(1, &m_id);
}

void OpenGLVertexBuffer::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void* OpenGLVertexBuffer::map_buffer_range() const
{
	return glMapNamedBufferRange(m_id, 0, m_size,
		GL_MAP_WRITE_BIT |
		GL_MAP_PERSISTENT_BIT |
		GL_MAP_COHERENT_BIT);
}

void OpenGLVertexBuffer::initialize(const void* vertices, 
	const uint32_t count, 
	const uint32_t size,
	int flags)
{
	m_count = count;
	m_size = count * size;

	glGenBuffers(1, &m_id);
	glBindBuffer(GL_ARRAY_BUFFER, m_id);

	if (flags & mbcore::BufferFlags::Persistent)
	{
		glNamedBufferStorage(m_id, m_size, vertices,
			GL_MAP_WRITE_BIT |
			GL_MAP_PERSISTENT_BIT |
			GL_MAP_COHERENT_BIT);
	}
	else
	{
		glBufferData(GL_ARRAY_BUFFER,
			m_size,
			vertices,
			flags & mbcore::BufferFlags::Static ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}
}
