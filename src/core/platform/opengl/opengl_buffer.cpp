#include "opengl.h"
#include "opengl_buffer.h"

static const std::array<uint32_t, mbcore::Buffer::LastBufferType> BUFFER_TYPES =
{
	GL_ARRAY_BUFFER,
	GL_ELEMENT_ARRAY_BUFFER,
	GL_DRAW_INDIRECT_BUFFER,
	GL_SHADER_STORAGE_BUFFER
};

OpenGLBuffer::OpenGLBuffer(const void* vertices, 
	const size_t count,
	const size_t size,
	const int type,
	int flags)
{
	initialize(vertices, count, size, type, flags);
}

OpenGLBuffer::~OpenGLBuffer()
{
	glDeleteBuffers(1, &m_id);
}

void OpenGLBuffer::bind() const
{
	glBindBuffer(m_type, m_id);
}

void OpenGLBuffer::sub_data(const size_t offset, const size_t size, const void* data)
{
	glBufferSubData(m_type, offset, size, data);
}

void OpenGLBuffer::get_sub_data(const size_t offset, const size_t size, void* data)
{
	glGetBufferSubData(m_type, offset, size, data);
}

void* OpenGLBuffer::map_buffer_range() const
{
	return glMapNamedBufferRange(m_id, 0, m_size,
		GL_MAP_WRITE_BIT |
		GL_MAP_PERSISTENT_BIT |
		GL_MAP_COHERENT_BIT);
}

void OpenGLBuffer::initialize(const void* vertices, 
	const size_t count,
	const size_t size,
	const int type,
	int flags)
{
	m_type = BUFFER_TYPES[type];
	m_count = (type == Buffer::Element) ? count * sizeof(uint32_t) : count;
	m_size = count * size;

	glGenBuffers(1, &m_id);
	glBindBuffer(m_type, m_id);

	if (flags & mbcore::BufferFlags::Persistent)
	{
		glNamedBufferStorage(m_id, m_size, vertices,
			GL_MAP_WRITE_BIT |
			GL_MAP_PERSISTENT_BIT |
			GL_MAP_COHERENT_BIT);
	}
	else
	{
		glBufferData(m_type,
			m_size,
			vertices,
			flags & mbcore::BufferFlags::Static ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}
}
