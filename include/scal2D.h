#pragma once

#include <vector>
#include "grid2D.h"


struct UnitScal2D {
	glm::ivec2 coord;
	float value;

	UnitScal2D() {};
	UnitScal2D(glm::ivec2 c, float v): coord(c), value(v) {};

	bool operator<(UnitScal2D b);
	bool operator>(UnitScal2D b);
	bool operator<=(UnitScal2D b);
	bool operator>=(UnitScal2D b);
	bool operator==(UnitScal2D b);
	bool operator!=(UnitScal2D b);
};

struct Scal2D: public Grid2D{
	struct Functor;

	Scal2D(const Scal2D& src);
	Scal2D(Scal2D&& src);

	Scal2D(const glm::vec2& min, const glm::vec2& max,
			int nbX, int nbY,
			float defaultValue = 0.f);
	Scal2D(const Box2D& input_box, int input_nx, int input_ny, float default_value = 0.f);
	Scal2D(const Grid2D& input_grid, float default_value = 0.f);

	Scal2D& operator=(const Scal2D& src);
	Scal2D& operator=(Scal2D&& src);

	Scal2D& operator+=(const Scal2D& src);
	Scal2D& operator+=(float scalar);

	Scal2D& operator-=(const Scal2D& src);
	Scal2D& operator-=(float scalar);

	Scal2D& operator*=(const Scal2D& src);
	Scal2D& operator*=(float scalar);

	// index of the element (i, j) in the vector of values
	unsigned int index(unsigned int i, unsigned int j) const;
	unsigned int index(const glm::ivec2& v) const;

	// integer coordinates of the element of index i in the vector of values
	glm::ivec2 inv_index(unsigned int i) const;

	// value of the element (i, j) in the scalar field
	float value(unsigned int i, unsigned int j) const;
	float value(const glm::ivec2& v) const;
	void setValue(const glm::ivec2& coord, float val);
	void addValue(const glm::ivec2& coord, float val);
	void suprValue(const glm::ivec2& coord, float val);

	float value(const glm::vec2& v) const;
	void setValue(const glm::vec2& coord, float val);

	void rescale(float minTarget, float maxTarget);
	Scal2D rescale(float minTarget, float maxTarget) const;

	// gradient of the scalar field at v
	float gradient(const glm::vec2& v) const;
	float gradient(const glm::ivec2& v) const;

	// returns the values of the neighbors [8-connexity]
	// /values/ is an array of size 8 such as
	// 	0 1 2
	// 	3 X 4
	// 	5 6 7
	void neighborhood(const glm::ivec2& v, float* values);

	// returns the number of neighbors [8-connexity] with value inferior to the value at v
	// /neighbor/ is an array of glm::ivec2 of size 8 storing neighbor positions relative to v
	// /delta_value/ is an array of float of size 8
	int lower_neighborhood(const glm::ivec2& v, glm::ivec2* neighbor, float* value, float* gradient) const;
	int threshold_neighborhood(const glm::ivec2& v, glm::ivec2* neighbor, float* values, float* gradients, float threshold_value) const;

	Scal2D drainage_area_field() const;

	bool importOBJ(const char* filename);

	std::vector<UnitScal2D> ScalToVector() const;

	// storing y as fast axis
	std::vector<float> values;
};

Scal2D operator+(const Scal2D& s1, const Scal2D& s2);
Scal2D operator+(const Scal2D& s, float scalar);
Scal2D operator*(const Scal2D& s1, const Scal2D& s2);
Scal2D operator*(const Scal2D& s, float scalar);
