#pragma once

#include <glm/vec2.hpp>

struct Box2D{
	Box2D(const Box2D& src);
	Box2D(const glm::vec2& input_min, const glm::vec2& input_max);

	Box2D& operator=(const Box2D& src);

	bool inside(const glm::vec2& v) const;
	bool intersect(const Box2D& b) const;

	glm::vec2 min;
	glm::vec2 max;
};
