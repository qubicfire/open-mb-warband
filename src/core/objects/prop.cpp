#include "core/managers/assets.h"
#include "core/graphics/renderer.h"
#include "core/managers/time.h"

#include "utils/profiler.h"

#include "prop.h"

using namespace mbcore;

void Prop::update()
{
	process_frame();
}

void Prop::load(brf::Mesh* mesh, const Buffer::Types flags)
{
	const auto& frames = mesh->get_frames();

	add_mesh(mesh);
	m_current_frame = 0;
	m_frames = static_cast<int>(
		frames.size()
	);

	mesh->precache(m_aabb, flags);

	add_texture("test/" + mesh->get_material() + ".dds", Texture2D::DDS);

	if (has_frames())
		set_object_flag(Object::Flags::FrameSystem);
}

void Prop::load(const std::string& name, const Buffer::Types flags)
{
	load(g_assets->get_mesh(name), flags);
}

void Prop::set_animation_frame_ex(const int frame)
{
	const auto& frames = get_mesh()->get_frames();

	if (m_current_frame >= m_frames)
	{
		m_current_frame = 0;
		m_next_frame = 1;
	}
	else
	{
		// wtf is this
		while (frames[m_current_frame].m_time <= 0.0f)
			m_current_frame++;

		if (m_next_frame >= m_frames - 1)
			m_next_frame = 0;
		else
			m_next_frame = m_current_frame + 1;
	}

	constexpr float INVERSE_TIME = 1.0f / 1000.0f;
	m_next_time = Time::get_time() + frames[m_next_frame].m_time * INVERSE_TIME;
}

void Prop::set_animation_frame(const int frame)
{
	if (frame < 0 && frame >= m_frames)
		return;

	m_current_frame = frame;
	set_animation_frame_ex(frame);
}

void Prop::freeze_animation(const bool state)
{
	m_is_frame_stopped = true;
}

bool Prop::has_frames() const
{
	return m_frames > 1;
}

int Prop::get_current_frame() const
{
	return m_current_frame;
}

void Prop::process_frame()
{
	if (!has_frames() || m_is_frame_stopped)
		return;

	if (Time::get_time() < m_next_time)
		return;

	set_animation_frame_ex(m_current_frame++);
}
