
#include "terrain.h"
#include "image.h"

#include <iostream>
#include <cassert>

Terrain::Terrain() {

}

Terrain::~Terrain() {

}

Scal2D Terrain::fromFile(const char* path, const Box2D& box, int nx, int ny,
						float minZ, float maxZ) {
	// file loading
	Image img(path);

	// initializing
	float vertices_by_x = (float)img.width();
	float vertices_by_y = (float)img.height();
	glm::ivec2 coord;
	float scale = maxZ - minZ;

	Scal2D heightfield(box, nx, ny);

	for(int ivalue = 0; ivalue != heightfield.values.size(); ++ivalue){
		coord = heightfield.inv_index(ivalue);
		coord.x = coord.x*vertices_by_x / nx;
		coord.y = coord.y*vertices_by_y / ny;
		// vertical scaling
		heightfield.values[ivalue] = minZ + img(coord.x, coord.y)[0] * scale;
	}
	return heightfield;
}

TerrainMusgrave::TerrainMusgrave(float baseWaveLength, int octaveCount, int seed) :
	baseWaveLength(baseWaveLength), octaveCount(octaveCount), seed(seed) {

}

TerrainMusgrave::~TerrainMusgrave(){
}

/**
 * Generate a heightfield with a Perlin Noise (of which parameters are in TerrainMusgrave)
 * with its values between minZ and maxZ
 */
Scal2D TerrainMusgrave::heightfield(const Box2D& box,
								int nbCellsX, int nbCellsY,
								float minZ, float maxZ) {

	float cellSizeX = ((box.max.x - box.min.x) / nbCellsX);
	float cellSizeY = ((box.max.y - box.min.y) / nbCellsY);
	float cellSize = (cellSizeX + cellSizeY) / 2.f;
	float frequency = cellSize / baseWaveLength;
	float half_vertical_scale = (maxZ - minZ) / 2.f;

	// Initializing & filling heightfield
	Scal2D heightfield(box, nbCellsX, nbCellsY);

	// Do the base wavelentgh first
	{
		std::unique_ptr<FastNoiseSIMD> noise1(initGenerator(frequency, seed));
		std::unique_ptr<FastNoiseSIMD> noise2(initGenerator(frequency, seed+1));

		// generating noise; generated buffers need to be deleted later
		// GetNoiseSet(int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f)
		float* altitude_noise = noise1->GetNoiseSet(0, 0, 0, nbCellsX, nbCellsY, 1);
		float* ridge_noise = noise2->GetNoiseSet(0, 0, 0, nbCellsX, nbCellsY, 1);

		for(int ivalue = 0; ivalue != heightfield.values.size(); ++ivalue) {
			heightfield.values[ivalue] =
				std::min(
					((altitude_noise[ivalue] + 1) // [-1; 1] -> [0; 2]
					* half_vertical_scale) // [0; 2] -> [0; vertical_scale]
					+ minZ, // [0; vertical_scale] -> [minZ; maxZ]
					((ridge_noise[ivalue] + 1) // [-1; 1] -> [0; 2]
					* half_vertical_scale) // [0; 2] -> [0; vertical_scale]
					+ minZ // [0; vertical_scale] -> [minZ; maxZ]
				);
		}

		// deleting noise buffer
		FastNoiseSIMD::FreeNoiseSet(altitude_noise);
		FastNoiseSIMD::FreeNoiseSet(ridge_noise);
	}

	// Then add higher frequency noises
	for (int o = 1; o < octaveCount; o++) {

		std::unique_ptr<FastNoiseSIMD> noise1(initGenerator(frequency, seed));
		std::unique_ptr<FastNoiseSIMD> noise2(initGenerator(frequency, seed+1));

		// generating noise; generated buffers need to be deleted later
		// GetNoiseSet(int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f)
		float* altitude_noise = noise1->GetNoiseSet(0, 0, 0, nbCellsX, nbCellsY, 1);
		float* ridge_noise = noise2->GetNoiseSet(0, 0, 0, nbCellsX, nbCellsY, 1);

		for(int ivalue = 0; ivalue != heightfield.values.size(); ++ivalue) {
			// adjust the noise depending on last value
			/// possible enhancement: compute diference between carrier wave value and actual value
			///   and compute a more accurate modulation
			float mod = 1.f;//float mod = heightfield.values[ivalue] / maxZ;
			// [-1; 1] -> [-vertical_scale; vertical_scale]
			heightfield.values[ivalue] +=
				std::min(
					altitude_noise[ivalue] * half_vertical_scale * mod,
					ridge_noise[ivalue] * half_vertical_scale * mod
				);
		}

		// deleting noise buffer
		FastNoiseSIMD::FreeNoiseSet(altitude_noise);
		FastNoiseSIMD::FreeNoiseSet(ridge_noise);

		// divide wavelength per 2
		frequency *= 2.0f;
		half_vertical_scale *= 0.5f;
	}

	// Rescale heightfield
	heightfield.rescale(minZ, maxZ);

	return heightfield;
}

FastNoiseSIMD* TerrainMusgrave::initGenerator(float frequency, int seed){

	// noise & fractal type
	FastNoiseSIMD* generator = FastNoiseSIMD::NewFastNoiseSIMD();
	generator->SetNoiseType(FastNoiseSIMD::PerlinFractal);
	generator->SetFractalType(FastNoiseSIMD::FBM);
	generator->SetPerturbType(FastNoiseSIMD::None);

	// noise parameters
	generator->SetFrequency(frequency);
	generator->SetSeed(seed);

	// fractal parameters
	generator->SetFractalOctaves(1);
	generator->SetFractalLacunarity(2.f);
	generator->SetFractalGain(0.5f);

	return generator;
}
