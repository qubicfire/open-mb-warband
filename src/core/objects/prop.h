#ifndef _PROP_H
#define _PROP_H
#include "brf/mesh.h"
#include "model.h"

class Prop : public Model
{
	object_base_impl(Prop)
public:
	void load(brf::Mesh* mesh, int flags = mbcore::BufferFlags::Static);
	void load(const std::string& name, int flags = mbcore::BufferFlags::Static);

	brf::Mesh* get_mesh() const;
	bool has_frames() const;
protected:
	void draw_internal(Shader* shader) override;

	void bind_all_textures(Shader* shader) const;

	void process_frame(brf::Vertex* buffer, int frame_id);
protected:
	// keyframe animation system
	int m_frame_id;
	int m_all_frames;

	mbcore::Texture2D* m_texture;
	brf::Mesh* m_mesh;
	brf::Vertex* m_buffer;
};

#endif // !_PROP_H
