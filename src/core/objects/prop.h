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

	void process_frame();
	void set_frame(const int id);
protected:
	// keyframe animation system
	int m_current_frame;
	int m_next_frame;
	int m_frames;
	float m_next_time;
	float m_current_time;
	bool m_is_frame_stopped;

	mbcore::Texture2D* m_texture;
	brf::Mesh* m_mesh;
	brf::Vertex* m_buffer;
};

#endif // !_PROP_H
