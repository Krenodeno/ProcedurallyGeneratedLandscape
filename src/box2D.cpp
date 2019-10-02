#include "box2D.h"


Box2D::Box2D(const Box2D& src)
: min(src.min), max(src.max){

}

Box2D::Box2D(const glm::vec2& input_min, const glm::vec2& input_max)
: min(input_min), max(input_max){

}

Box2D& Box2D::operator=(const Box2D& src){
	min = src.min;
	max = src.max;

	return *this;
}

bool Box2D::inside(const glm::vec2& v) const{
	if(v.x < min.x || v.x > max.x
		|| v.y < min.y || v.y > max.y){

		return false;
	}

	return true;
}

// using the separating axis theorem
bool Box2D::intersect(const Box2D& b) const{
	if((b.min.x > max.x || b.max.x < min.x)
		&& (b.min.y > max.y || b.max.y < min.y)){
		return false;
	}

	return true;
}