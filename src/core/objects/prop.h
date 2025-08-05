#ifndef _PROP_H
#define _PROP_H
#include "brf/mesh.h"
#include "model.h"

class Prop : public Model
{
	object_base_impl(Prop)
public:
	void load(brf::Mesh* mesh, bool is_static_draw = true);
	void load(const std::string& name, bool is_static_draw = true);

	brf::Mesh* get_mesh() const noexcept;
protected:
	void bind_all_textures(Shader* shader) const;
protected:
	Unique<mbcore::Texture2D> m_texture;
	brf::Mesh* m_mesh;
};

#endif // !_PROP_H
