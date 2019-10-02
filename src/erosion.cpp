#include "erosion.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cassert>
#include <cmath>
#include <fstream>
#include <unordered_set>
#include <algorithm>

#include <iostream> // @DEBUG
#include <random> // @DEBUG
#include <thread>
#include <chrono>

using namespace Erosion;

void Erosion::simulationLoop(MultiLayers& terrain, int iteration) {
	const float atan5 = tan(M_PI/36);
	const float atan10 = tan(M_PI/18);
	for(int i=0; i<iteration; i++) {
		std::cout << i+1 << "/" << iteration << std::endl;
		Thermal::erode(terrain);
		std::cout << ".";
		Hydraulic::erode(terrain);
		std::cout << ".";
		Thermal::oneTransport(terrain, atan10, 1.f);
		std::cout << "." << std::endl;
		Thermal::oneTransport(terrain, atan5, .5f);
	}
}

//
//  Thermal Part
//

void Thermal::erode(MultiLayers& terrain){

	alteration(terrain, .1f);

	transport(terrain, 1.f);
}

// slope based alteration
void Thermal::alteration(MultiLayers& terrain, const float conversion_rate){

	for(int ivalue = 0; ivalue != terrain.bedrock().values.size(); ++ivalue){
		const glm::ivec2 coord = terrain.sediment().inv_index(ivalue);

		float slope = terrain.bedrock().gradient(coord);
		if(slope>0)
			terrain.transferBedrockSed(coord, conversion_rate * alteration_function(slope));
	}
}

// determines the alteration intensity using a slope value
float Thermal::alteration_function(const float slope){
	return slope/sqrt(1+(slope*slope));
}

void Thermal::transport(MultiLayers& terrain, const float max_transport) {
	const double stability_angle = tan(M_PI/4.5);

	std::vector<glm::ivec2> potentially_unstable;
	Scal2D heightfield = terrain.heightfield();
	glm::ivec2 terrain_cell;
	glm::ivec2 terrain_neighbor[8] = {{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}};
	float terrain_values[8];
	float terrain_slopes[8];

float tests = 0.f;
float maxS = 0.f;
float minS = 500.f;
float testb = 0.f;
float maxB = 0.f;
float minB = 500.f;
	// evaluating first unstabilities
	for(int ix = 0; ix != heightfield.nx; ++ix){
		terrain_cell.x = ix;

		for(int iy = 0; iy != heightfield.ny; ++iy){
			terrain_cell.y = iy;

			// on pourrait tester dès maintenant l'instabilité
			potentially_unstable.push_back(terrain_cell);
			float s = terrain.sediment().value(terrain_cell);
			tests += s;
			float b = terrain.bedrock().value(terrain_cell);
			testb += b;
			if(s > maxS) maxS = s;
			if(s < minS) minS = s;
			if(b > maxB) maxB = b;
			if(b < minB) minB = b;
		}
	}
	std::cout << minS << " - " << tests/(heightfield.nx*heightfield.ny) << " - " << maxS << std::endl;
	std::cout << minB << " - " << testb/(heightfield.nx*heightfield.ny) << " - " << maxB << std::endl;

	// while not stable
	while(!potentially_unstable.empty()){
		//std::cout << potentially_unstable.size() << std::endl;
		terrain_cell = potentially_unstable.back();

		int neighbor_count = stabilize_pile(terrain_cell, terrain,
											terrain_neighbor, terrain_values, terrain_slopes, stability_angle, max_transport);

		// pick one & erase it from the list of unstable coord
		potentially_unstable.pop_back();

		// add neighbors where material fell to
		for(int ineighbor = 0; ineighbor != neighbor_count; ++ineighbor){
			//if ( std::find(potentially_unstable.begin(), potentially_unstable.end(), terrain_neighbor[ineighbor]) == potentially_unstable.end() )
				potentially_unstable.insert(potentially_unstable.begin(), terrain_neighbor[ineighbor]);
		}
	}
}

void Thermal::oneTransport(MultiLayers& terrain, const float stabilityAngle, const float max_transport) {
	glm::ivec2 terrain_cell;
	glm::ivec2 terrain_neighbor[8] = {{0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}};
	float terrain_values[8];
	float terrain_slopes[8];

	// evaluating first unstabilities
	for(int ix = 0; ix != terrain.nx; ++ix){
		terrain_cell.x = ix;
		for(int iy = 0; iy != terrain.ny; ++iy){
			terrain_cell.y = iy;
			stabilize_pile(terrain_cell, terrain, terrain_neighbor, terrain_values, terrain_slopes, stabilityAngle, max_transport);
		}
	}
}

int Thermal::stabilize_pile(const glm::ivec2& v, MultiLayers& terrain,
					glm::ivec2* neighbor, float* value, float* gradient, const float stabilityAngle, const float max_transport){

	float sed = terrain.sediment().value(v);
	if(sed == 0) return 0;

	int neighbor_count = terrain.heightfield().threshold_neighborhood(v, neighbor, value, gradient, stabilityAngle);
	if(neighbor_count == 0) return 0;

	// find the repartition of the matter
	float max_gradient = 0;
	float gradient_sum = 0.;

	for(int ineighbor = 0; ineighbor != neighbor_count; ++ineighbor){
		gradient_sum += gradient[ineighbor];

		if(gradient[ineighbor] > max_gradient){
			max_gradient = gradient[ineighbor];
		}
	}
	if(max_gradient <= stabilityAngle) return 0;

	// quantity to transport ie until stable with the max gradient
	float height_to_transport = std::min(sed, std::min(max_transport, max_gradient - stabilityAngle));
	terrain.sediment().suprValue(v, height_to_transport);

	for(int ineighbor = 0; ineighbor != neighbor_count; ++ineighbor){
		const float to_neighbor = (gradient[ineighbor] / gradient_sum) * height_to_transport;
		terrain.sediment().addValue(neighbor[ineighbor], to_neighbor);
	}

	// add the position if it's not stable
	if(height_to_transport == max_transport && sed != height_to_transport) {
		neighbor[neighbor_count] = v;
		neighbor_count++;
	}

	return neighbor_count;
}

//
//  Hydraulic Part
//

void Hydraulic::erode(MultiLayers& terrain) {
	Scal2D heightfield = terrain.heightfield();
	Scal2D drainage_area_field((Grid2D)heightfield, 1.0);
	// trier drainage_area_field en fonction de la hauteur heightfield
	std::vector<UnitScal2D> vectorHeight = heightfield.ScalToVector();
	std::sort(vectorHeight.begin(), vectorHeight.end());

	glm::ivec2 neighborsCoords[8];
	float neighborsValues[8];
	float neighborsGradients[8];
	// pour chaque point écouler son aire sur ses voisins en fonction de la pente
	for(int i=vectorHeight.size()-1; i>=0; i--) {
		glm::ivec2 coord = vectorHeight[i].coord;
		int nb = heightfield.lower_neighborhood(coord, neighborsCoords, neighborsValues, neighborsGradients);

		float sumGradients = 0;
		for(int j=0; j<nb; j++) {
			sumGradients += neighborsGradients[j];
		}

		// drainage_area += voisins * pente
		for(int j=0; j<nb; j++) {
			drainage_area_field.setValue(neighborsCoords[j], drainage_area_field.value(neighborsCoords[j]) + drainage_area_field.value(coord) * neighborsGradients[j]/sumGradients);
		}

		// hauteur -= cst * sqrt(drainage_area) * pente
		float delta = .01 * sqrt(drainage_area_field.value(coord)) * heightfield.gradient(coord);
		terrain.transferBedrockSed(coord, delta);
	}
}
