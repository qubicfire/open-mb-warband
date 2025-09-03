#ifndef _MODEL_H
#define _MODEL_H
#include "core/mb.h"
#include "core/platform/vertex_array.h"
#include "core/platform/texture2d.h"

#include "object.h"

class Model : public Object
{
	object_base(Model)
public:
	void draw() override;

	mbcore::VertexArray* get_vertex_array() const;
protected:
	void draw_internal(Shader* shader) override;

	virtual void bind_all_textures(Shader* shader) const { }
protected:
	Unique<mbcore::VertexArray> m_vertex_array;
};

#endif // !_MODEL_H
