#ifndef _TEXT_3D_H
#define _TEXT_3D_H
#include "model.h"
#include "core/graphics/text_builder_3d.h"

class Text3D final : public Model
{
public:
	void draw() override;

	void set_text(const std::string& text, const float scale = 1.0f);
private:
	void draw_internal(Shader* shader) override;
};

#endif // !_TEXT_3D_H
