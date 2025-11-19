#include "opengl.h"
#include "opengl_buffer.h"

using namespace mbcore;

static uint32_t get_buffer_type(const Buffer::Types type)
{
	if (type & Buffer::Types::Array)
		return GL_ARRAY_BUFFER;
	else if (type & Buffer::Types::Element)
		return GL_ELEMENT_ARRAY_BUFFER;
	else if (type & Buffer::Types::Indirect)
		return GL_DRAW_INDIRECT_BUFFER;
	else if (type & Buffer::Types::Storage)
		return GL_SHADER_STORAGE_BUFFER;

	return 0;
}

static uint32_t get_target_type(const Buffer::Types type)
{
	if (type & Buffer::Types::Static)
		return GL_STATIC_DRAW;
	else if (type & Buffer::Types::Dynamic)
		return GL_DYNAMIC_DRAW;

	return GL_STATIC_DRAW;
}

OpenGLBuffer::OpenGLBuffer(const Buffer::Types flags)
{
	initialize(flags);
}

OpenGLBuffer::OpenGLBuffer(const void* vertices,
	const size_t count,
	const size_t size,
	const Buffer::Types flags)
{
	initialize(vertices, count, size, flags);
}

OpenGLBuffer::~OpenGLBuffer()
{
	glDeleteBuffers(1, &m_id);
}

void OpenGLBuffer::bind() const
{
	glBindBuffer(m_type, m_id);
}

void OpenGLBuffer::buffer_data(const size_t size, const void* data, const Types flags)
{
	glBufferData(m_type, size, data, get_target_type(flags));
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

void OpenGLBuffer::initialize(const Buffer::Types flags)
{
	m_type = get_buffer_type(flags);

	glGenBuffers(1, &m_id);
	glBindBuffer(m_type, m_id);
}

void OpenGLBuffer::initialize(const void* vertices,
	const size_t count,
	const size_t size,
	const Buffer::Types flags)
{
	m_type = get_buffer_type(flags);
	m_count = (flags & Buffer::Types::Element) ? count * sizeof(uint32_t) : count;
	m_size = count * size;

	glGenBuffers(1, &m_id);
	glBindBuffer(m_type, m_id);

	if (flags & Buffer::Types::Persistent)
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
			flags & Buffer::Types::Static ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}
}
