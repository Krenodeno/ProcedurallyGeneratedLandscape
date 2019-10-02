#include "scal2D.h"
#include <cmath>
#include <fstream>
#include <string>

#include <iostream>


Scal2D::Scal2D(const Scal2D& src) : Grid2D(src.min, src.max, src.nx, src.ny){
	values.resize(src.nx * src.ny);

	for(int iv = 0; iv != values.size(); ++iv){
		values[iv] = src.values[iv];
	}
}

Scal2D::Scal2D(Scal2D&& src) : Grid2D(src.min, src.max, src.nx, src.ny){
	values = std::move(src.values);
}

Scal2D::Scal2D(const glm::vec2& input_min, const glm::vec2& input_max,
				int input_nx, int input_ny, float default_value)
	: Grid2D(input_min, input_max, input_nx, input_ny){

	values.resize(input_nx * input_ny, default_value);
}

Scal2D::Scal2D(const Box2D& input_box, int input_nx, int input_ny, float default_value)
: Grid2D(input_box, input_nx, input_ny){

	values.resize(input_nx * input_ny, default_value);
}

Scal2D::Scal2D(const Grid2D& input_grid, float default_value)
: Grid2D(input_grid){

	values.resize(input_grid.nx * input_grid.ny, default_value);
}

Scal2D& Scal2D::operator=(const Scal2D& src){
	min = src.min;
	max = src.max;

	nx = src.nx;
	ny = src.ny;

	values.resize(src.nx & src.ny);

	for(int iv = 0; iv != values.size(); ++iv){
		values[iv] = src.values[iv];
	}
}

Scal2D& Scal2D::operator=(Scal2D&& src){
	min = src.min;
	max = src.max;

	nx = src.nx;
	ny = src.ny;

	values = std::move(src.values);
}

Scal2D& Scal2D::operator+=(const Scal2D& src){
	assert((nx == src.nx) && (ny == src.ny));

	for(int ivalue = 0; ivalue != values.size(); ++ivalue){
		values[ivalue] += src.values[ivalue];
	}

	return *this;
}

Scal2D& Scal2D::operator+=(float scalar){
	for(int ivalue = 0; ivalue != values.size(); ++ivalue){
		values[ivalue] += scalar;
	}

	return *this;
}

Scal2D& Scal2D::operator-=(const Scal2D& src){
	assert((nx == src.nx) && (ny == src.ny));

	for(int ivalue = 0; ivalue != values.size(); ++ivalue){
		values[ivalue] -= src.values[ivalue];
	}

	return *this;
}

Scal2D& Scal2D::operator-=(float scalar){
	for(int ivalue = 0; ivalue != values.size(); ++ivalue){
		values[ivalue] -= scalar;
	}

	return *this;
}

Scal2D& Scal2D::operator*=(const Scal2D& src){
	assert((nx == src.nx) && (ny == src.ny));

	for(int ivalue = 0; ivalue != values.size(); ++ivalue){
		values[ivalue] *= src.values[ivalue];
	}

	return *this;
}

Scal2D& Scal2D::operator*=(float scalar){
	for(int ivalue = 0; ivalue != values.size(); ++ivalue){
		values[ivalue] *= scalar;
	}

	return *this;
}

unsigned int Scal2D::index(unsigned int i, unsigned int j) const{
	assert((i < nx) && (j < ny));

	return i * ny + j;
}

unsigned int Scal2D::index(const glm::ivec2& v) const{
	assert((v.x < nx) && (v.y < ny));

	return v.x * ny + v.y;
}

glm::ivec2 Scal2D::inv_index(unsigned int i) const{
	assert(i < values.size());

	glm::ivec2 coord = {i / ny, i % ny}; //@TODO: optimize

	return coord;
}

float Scal2D::value(unsigned int i, unsigned int j) const{
	assert((i < nx) && (j < ny));

	return values[i * ny + j];
}

float Scal2D::value(const glm::ivec2& v) const{
	assert((v.x < nx) && (v.y < ny));

	return values[v.x * ny + v.y];
}

float Scal2D::value(const glm::vec2& v) const{
	glm::vec2 grid_coord = locate(v);
	glm::ivec2 igrid_coord = {(int)grid_coord.x, (int)grid_coord.y};
	glm::vec2 rgrid_coord = {grid_coord.x - igrid_coord.x, grid_coord.y - igrid_coord.y};

	// C  D
	// A  B
	// order defined by fast axis
	float vC = value(igrid_coord.x, igrid_coord.y + 1);
	float vA = value(igrid_coord.x, igrid_coord.y);
	float vD = value(igrid_coord.x + 1, igrid_coord.y + 1);
	float vB = value(igrid_coord.x + 1, igrid_coord.y);

	float valueAB = (1. - rgrid_coord.x) * vA + rgrid_coord.x * vB;
	float valueCD = (1. - rgrid_coord.x) * vC + rgrid_coord.x * vD;

	return (1. - rgrid_coord.y) * valueAB + rgrid_coord.y * valueCD;
}

void Scal2D::setValue(const glm::ivec2& coord,const float val) {
	values[coord.x * ny + coord.y] = val;
}

void Scal2D::setValue(const glm::vec2& coord, const float val) {
	glm::ivec2 grid_coord = ilocate(coord);
	values[grid_coord.x * ny + grid_coord.y] = val;
}

void Scal2D::addValue(const glm::ivec2& coord,const float val) {
	assert((coord.x < nx) && (coord.y < ny));
	values[coord.x * ny + coord.y] += val;
}

void Scal2D::suprValue(const glm::ivec2& coord,const float val) {
	assert((coord.x < nx) && (coord.y < ny));
	assert(val <= values[coord.x * ny + coord.y]);
	values[coord.x * ny + coord.y] -= val;
}

/**
 * Rescale a Scalar field between minTarget and maxTarget values
 */
void Scal2D::rescale(float minTarget, float maxTarget) {
	assert(minTarget != maxTarget);

	float hMin = std::numeric_limits<float>::max();
	float hMax = std::numeric_limits<float>::min();

	for (const float& h : values) {
		hMin = h < hMin ? h : hMin;
		hMax = h > hMax ? h : hMax;
	}

	assert(hMin != hMax);

	for (float& h : values) {
		h = (((maxTarget - minTarget) * (h - hMin)) / (hMax - hMin)) + minTarget;
	}
}

Scal2D Scal2D::rescale(float minTarget, float maxTarget) const {
	Scal2D copy = *this;
	copy.rescale(minTarget, maxTarget);
	return copy;
}

// @TODO: Make sure this is good when v is not on the grid
float Scal2D::gradient(const glm::vec2& v) const{
	glm::ivec2 ilocal_v = ilocate(v);
	glm::vec2 grid_step = step();

	const float gradX = (value(std::min(ilocal_v.x + 1, nx - 1), ilocal_v.y)
							- value(std::max(ilocal_v.x - 1, 0), ilocal_v.y))
						/ (2*grid_step.x);
	const float gradY = (value(ilocal_v.x, std::min(ilocal_v.y + 1, ny - 1))
							- value(ilocal_v.x, std::max(ilocal_v.y - 1, 0)))
						/ (2*grid_step.y);

	return std::sqrt(gradX * gradX + gradY * gradY);
}


// @TODO: Make sure this is good when v is not on the grid
float Scal2D::gradient(const glm::ivec2& v) const{
	glm::vec2 grid_step = step();
	float gradX = (value(std::min(v.x + 1, nx - 1), v.y)
						- value(std::max(v.x - 1, 0), v.y))
						/ (2*grid_step.x);
	float gradY = (value(v.x, std::min(v.y + 1, ny - 1))
							- value(v.x, std::max(v.y - 1, 0)))
						/ (2*grid_step.y);

	return std::sqrt(gradX * gradX + gradY * gradY);
}

void Scal2D::neighborhood(const glm::ivec2& v, float* values){
	glm::ivec2 neighbor_relative_coord[8] = {{-1, -1}, {0, -1}, {1, -1},
											{-1, 0}, 		   {1, 0},
											{-1, 1},  {0, 1},  {1, 1}};

	for(int ineighbor = 0; ineighbor != 8; ++ineighbor){
		glm::ivec2 neighbor_coord = v + neighbor_relative_coord[ineighbor];

		values[ineighbor] = value(neighbor_coord);
	}

}

int Scal2D::lower_neighborhood(const glm::ivec2& v, glm::ivec2* neighbors, float* values, float* gradients) const {
	// todo: static ? elsewhere in the code ?
	static glm::ivec2 neighbor_relative_coord[8] = {{-1, -1}, {0, -1}, {1, -1},
													{-1, 0}, 		   {1, 0},
													{-1, 1},  {0, 1},  {1, 1}};
/*
	static float distance_scale[8] = {sqrt(2.), 1., sqrt(2.),
									  1.,			1.,
									  sqrt(2.), 1., sqrt(2.)};*/
	glm::vec2 grid_step = step();
	float diag = sqrt(grid_step.x*grid_step.x+grid_step.y*grid_step.y);
	static float distance_scale[8] = {diag, 	grid_step.y, diag,
									  grid_step.x,			 grid_step.x,
									  diag, 	grid_step.y, diag};

	int nneighbor = 0;
	const float& current_value = value(v);

	for(int ineighbor = 0; ineighbor != 8; ++ineighbor){
		glm::ivec2 neighbor_coord = v + neighbor_relative_coord[ineighbor];

		if((neighbor_coord.x >= 0) && (neighbor_coord.x < nx)
			&& (neighbor_coord.y >= 0) && (neighbor_coord.y < ny)){

			const float& neighbor_value = value(neighbor_coord);

			if(current_value > neighbor_value){
				neighbors[nneighbor] = neighbor_coord;
				values[nneighbor] = neighbor_value;
				gradients[nneighbor] = (current_value - neighbor_value) / distance_scale[ineighbor]; //@TODO: This assumes dx = dy = 1.
				++nneighbor;
			}
		}
	}

	return nneighbor;
}

int Scal2D::threshold_neighborhood(const glm::ivec2& v, glm::ivec2* neighbors, float* values, float* gradients, float threshold_value) const {
	//@TODO: static ? elsewhere in the code ?
	static glm::ivec2 neighbor_relative_coord[8] = {{-1, -1}, {0, -1}, {1, -1},
													{-1, 0}, 		   {1, 0},
													{-1, 1},  {0, 1},  {1, 1}};


	glm::vec2 grid_step = step();
	float diag = sqrt(grid_step.x*grid_step.x+grid_step.y*grid_step.y);
	static float distance_scale[8] = {diag, 	grid_step.y, diag,
									  grid_step.x,			 grid_step.x,
									  diag, 	grid_step.y, diag};

	int nneighbor = 0;
	const float& current_value = value(v);

	for(int ineighbor = 0; ineighbor != 8; ++ineighbor){
		glm::ivec2 neighbor_coord = v + neighbor_relative_coord[ineighbor];

		if((neighbor_coord.x >= 0) && (neighbor_coord.x < nx)
			&& (neighbor_coord.y >= 0) && (neighbor_coord.y < ny)){

			const float& neighbor_value = value(neighbor_coord);

			const float& neighbor_slope = (current_value - neighbor_value) / distance_scale[ineighbor];

			if(neighbor_slope > threshold_value){
				neighbors[nneighbor] = neighbor_coord;
				values[nneighbor] = neighbor_value;
				gradients[nneighbor] = neighbor_slope; //@TODO: This assumes dx = dy = 1.
				nneighbor++;
			}
		}
	}

	return nneighbor;
}

bool Scal2D::importOBJ(const char* filename) {
	std::ifstream file;
	file.open(filename);

	// checking successful opening
	if(!file.is_open()){
		return false;
	}

  	std::string line;
	std::string::size_type sz;
    while ( getline (file,line) )
    {
		if(line[0] == 'v') {
			float x = std::stof(line,&sz);
			float y = std::stof(line.substr(sz),&sz);
			float v = std::stof(line.substr(sz),&sz);
			int ix = x / ((max.x - min.x) / nx);
			int iy = y / ((max.y - min.y) / ny);
			values[index(ix, iy)] = v;
		} else if(line[0] == 'f')
			break;
    }
	file.close();

	return true;
}

Scal2D operator+(const Scal2D& s1, const Scal2D& s2){

	return Scal2D(s1) += s2;
}

Scal2D operator+(const Scal2D& s, float scalar){

	return Scal2D(s) += scalar;
}

Scal2D operator*(const Scal2D& s1, const Scal2D& s2){

	return Scal2D(s1) *= s2;
}

Scal2D operator*(const Scal2D& s, float scalar){

	return Scal2D(s) *= scalar;
}

std::vector<UnitScal2D> Scal2D::ScalToVector() const {
	std::vector<UnitScal2D> vectorUnitScal;
	for(int i = 0; i != values.size(); ++i)
		vectorUnitScal.push_back(UnitScal2D(inv_index(i),values[i]));

	return vectorUnitScal;
}

bool UnitScal2D::operator<(UnitScal2D b) {
	return this->value < b.value;
}
bool UnitScal2D::operator>(UnitScal2D b) {
	return this->value > b.value;
}
bool UnitScal2D::operator<=(UnitScal2D b) {
	return this->value <= b.value;
}
bool UnitScal2D::operator>=(UnitScal2D b) {
	return this->value >= b.value;
}
bool UnitScal2D::operator==(UnitScal2D b) {
	return this->value == b.value;
}
bool UnitScal2D::operator!=(UnitScal2D b) {
	return this->value != b.value;
}
