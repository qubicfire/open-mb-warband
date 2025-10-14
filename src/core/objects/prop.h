#ifndef _PROP_H
#define _PROP_H
#include "brf/mesh.h"
#include "object.h"

class Prop : public Object
{
	object_base(Prop)
public:
	void update() override;

	void load(brf::Mesh* mesh, 
		const mbcore::Buffer::Types flags = mbcore::Buffer::Types::Static);
	void load(const std::string& name, 
		const mbcore::Buffer::Types flags = mbcore::Buffer::Types::Static);

	bool has_frames() const;
protected:
	void process_frame();
	void set_frame(const int id);
protected:
	// keyframe animation system
	int m_current_frame;
	int m_next_frame;
	int m_frames;
	float m_next_time;
	bool m_is_frame_stopped;

	brf::Vertex* m_buffer;
};

#endif // !_PROP_H
