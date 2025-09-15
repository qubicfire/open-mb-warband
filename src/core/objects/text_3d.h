#ifndef _TEXT_3D_H
#define _TEXT_3D_H
#include "object.h"
#include "core/graphics/text_builder_3d.h"

class Text3D final : public Object
{
	object_client_base(text_3d)
public:
	void set_text(const std::string& text, const float scale = 1.0f);
};

#endif // !_TEXT_3D_H
