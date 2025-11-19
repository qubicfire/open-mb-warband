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

	void set_animation_frame(const int frame);
	void freeze_animation(const bool state);
	bool has_frames() const;

	int get_current_frame() const;
protected:
	void set_animation_frame_ex(const int frame);
	void process_frame();
protected:
	// keyframe animation system
	int m_current_frame;
	int m_next_frame;
	int m_frames;
	float m_next_time;
	bool m_is_frame_stopped;
};

#endif // !_PROP_H
