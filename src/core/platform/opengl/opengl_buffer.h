#ifndef _OPENGL_BUFFER_H
#define _OPENGL_BUFFER_H
#include "core/platform/buffer.h"

struct OpenGLBuffer : mbcore::Buffer
{
	OpenGLBuffer(const void* vertices, 
		const size_t count,
		const size_t size,
		const Buffer::Types flags);
	~OpenGLBuffer();

	void bind() const override;
	void sub_data(const size_t offset, const size_t size, const void* data) override;
	void get_sub_data(const size_t offset, const size_t size, void* data) override;

	void* map_buffer_range() const override;
protected:
	void initialize(const void* vertices, 
		const size_t count,
		const size_t size,
		const Buffer::Types flags) override;
};

#endif // !_OPENGL_BUFFER_H
