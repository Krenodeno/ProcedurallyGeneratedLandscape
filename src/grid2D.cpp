#include "grid2D.h"


Grid2D::Grid2D(const Grid2D& src) : Box2D(src.min, src.max), nx(src.nx), ny(src.ny) { }

Grid2D::Grid2D(const glm::vec2& input_min, const glm::vec2& input_max, const int& input_nx, const int& input_ny)
	: Box2D(input_min, input_max), nx(input_nx), ny(input_ny) { }

Grid2D::Grid2D(const Box2D& input_box, const int& input_nx, const int& input_ny)
	: Box2D(input_box), nx(input_nx), ny(input_ny){ }

Grid2D& Grid2D::operator=(const Grid2D& src){
	min = src.min;
	max = src.max;

	nx = src.nx;
	ny = src.ny;

	return *this;
}

glm::vec2 Grid2D::diagonal() const{
	return max - min;
}

glm::vec2 Grid2D::step() const{
	return {(max.x - min.x) / (nx - 1), (max.y - min.y) / (ny - 1)};
}

glm::vec2 Grid2D::point(unsigned int i, unsigned int j) const{
	assert((i < nx) && (j < ny));

	glm::vec2 grid_coord = {i, j};

	return diagonal() * step() * grid_coord;
}

glm::vec2 Grid2D::locate(const glm::vec2& v) const{
	assert(inside(v));

	return (v - min) / step();
}

glm::ivec2 Grid2D::ilocate(const glm::vec2& v) const{
	assert(inside(v));

	glm::vec2 local_v = (v - min) / step();
	glm::ivec2 ilocal_v = {(int)local_v.x, (int)local_v.y};

	return ilocal_v;
}
