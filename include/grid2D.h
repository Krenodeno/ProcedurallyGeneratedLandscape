#pragma once

#include "box2D.h"


struct Grid2D: public Box2D{
	Grid2D(const Grid2D& src);
	Grid2D(const glm::vec2& input_min, const glm::vec2& input_max, const int& input_nx, const int& input_ny);
	Grid2D(const Box2D& input_box, const int& input_nx, const int& input_ny);

	Grid2D& operator=(const Grid2D& src);

	glm::vec2 diagonal() const;
	glm::vec2 step() const;
	glm::vec2 point(unsigned int i, unsigned int j) const;
	glm::vec2 locate(const glm::vec2& v) const;
	glm::ivec2 ilocate(const glm::vec2& v) const;

	// number of points on each axis of the grid
	int nx;
	int ny;
};
