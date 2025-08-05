#include <array>
#include "opengl.h"

#include "opengl_vertex_array.h"

using namespace mbcore;

OpenGLVertexArray::OpenGLVertexArray()
{
	initialize();
}

OpenGLVertexArray::~OpenGLVertexArray()
{
	glDeleteVertexArrays(1, &m_id);
}

void OpenGLVertexArray::bind() const
{
	glBindVertexArray(m_id);
}

void OpenGLVertexArray::unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

struct VertexAttribute
{
	uint32_t m_size;
	uint32_t m_type;
};

constexpr std::array<VertexAttribute, static_cast<size_t>(VertexType::Count)> ATTRIBUTES =
{
	VertexAttribute { 1, GL_FLOAT }, // DataType::Float
	VertexAttribute { 2, GL_FLOAT }, // DataType::Float2
	VertexAttribute { 3, GL_FLOAT }, // DataType::Float3
	VertexAttribute { 4, GL_FLOAT }, // DataType::Float4
	VertexAttribute { 1, GL_UNSIGNED_SHORT }, // DataType::UShort
	VertexAttribute { 1, GL_UNSIGNED_INT }, // DataType::Uint
};

void OpenGLVertexArray::link(uint32_t index,
	VertexType type,
	int stride,
	const void* pointer,
	bool normalized) const
{
	const VertexAttribute& attribute = ATTRIBUTES[static_cast<uint8_t>(type)];

	glEnableVertexAttribArray(index);
	if (normalized)
	{
		glVertexAttribIPointer(index,
			attribute.m_size,
			attribute.m_type,
			stride,
			pointer);
	}
	else
	{
		glVertexAttribPointer(index,
			attribute.m_size,
			attribute.m_type,
			normalized,
			stride,
			pointer);
	}
}

void OpenGLVertexArray::initialize()
{
	glGenVertexArrays(1, &m_id);
	glBindVertexArray(m_id);
}
