#pragma once

#include "scal2D.h"


struct LayerScal2D: public Scal2D{
	LayerScal2D(const Scal2D& src) : Scal2D(src) {}
	LayerScal2D(Scal2D&& src) : Scal2D(src) {}
	LayerScal2D(const glm::vec2& input_min, const glm::vec2& input_max,
			int input_nx, int input_ny, float default_value = 0.f) : 
			Scal2D(input_min, input_max, input_nx, input_ny, default_value) {}
	LayerScal2D(const Box2D& input_box, int input_nx, int input_ny, float default_value = 0.f) :
			Scal2D(input_box, input_nx, input_ny, default_value) {}
	LayerScal2D(const Grid2D& input_grid, float default_value = 0.f) :
			Scal2D(input_grid, default_value) {}
};
