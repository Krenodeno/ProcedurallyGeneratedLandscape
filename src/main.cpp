#include "terrain.h"
#include "erosion.h"
#include "veget.h"
#include "exporter.h"

#include "FreeImagePlus.h"

#include <glm/vec2.hpp>

#include <cstring>
#include <iostream>
#include <string>

void help() {
	std::cout << "Generate terrain based on Perlin noise.\n";
	std::cout << "Usage : ANewWorld -h\n";
	std::cout << "        ANewWorld TERRAIN-PARAMETERS [FILENAME] [-e STEPS [-v STEPS]]\n";
	std::cout << "\n-TERRAIN-PARAMETERS:\n";
	std::cout << "  -Base wave length of Perlin Noise in meters\n";
	std::cout << "  -Size of the output square terrain in meters\n";
	std::cout << "  -Maximum altitude of the altitude (min is 0) in meters\n";
	std::cout << "  -Number of octaves for Perlin Noise sum\n";
	std::cout << "  -Number of cells (aka.number of vertices in 3D model)\n";
	std::cout << "\nParameters :\n";
	std::cout << "-h : print this and exit\n";
	std::cout << "-e STEPS : errode terrain STEPS times\n";
	std::cout << "-v STEPS : generate vegetation and simulate it STEPS times\n";
	std::cout << std::endl;
}

int main(int argc, char* argv[]) {
/*
	float baseWaveLength = 10000.f;
	float size = 16000.f;
	float maxZ = 4000.f;
	int   octaveCount = 8;
	int   cellCount = 100;
	std::string filename = "out.obj";

	int erosionStepCount = 500;
	int vegeSimuStepCount = 0;*/
	FreeImage_Initialise();

	float baseWaveLength = 70.f;
	float size = 100.f;
	float maxZ = 40.f;
	int   octaveCount = 7;
	int   cellCount = 100;
	std::string filename = "out.obj";

	int erosionStepCount = 10;
	int vegeSimuStepCount = 1;

	switch (argc) {
	case 11:
		vegeSimuStepCount = std::stoi(argv[10]);
	case 9:
		erosionStepCount = std::stoi(argv[8]);
	case 7:
		filename = argv[6];
	case 6:
		baseWaveLength	= std::stof(argv[1]);
		size			= std::stof(argv[2]);
		maxZ			= std::stof(argv[3]);
		octaveCount		= std::stoi(argv[4]);
		cellCount		= std::stoi(argv[5]);
		break;
	case 1:
		std::cout << "Using default paramters:\n";
		break;
	default:
		help();
		if (argc == 2 && strcmp(argv[1], "-h") == 0)
			return 0;
		else
			return -1;
	}

	// Unit is what you want, 1 meter, 1 kilometer, 1 yard ...
	// Just remember to be consistent

	// We want to set :
	// - the heighfield size (in x, y, z units)
	// - the number of cells / the size of each cells
	// - the base frequency and the number of octaves / the size of mountains

	std::cout << "Creating a Terrain with:\n";
	std::cout << "- Base wave length of " << baseWaveLength << " meters\n";
	std::cout << "- Size of " << size << "x" << size << " meters\n";
	std::cout << "- A maximum altitude of " << maxZ << " meters\n";
	std::cout << "- " << octaveCount << " octaves of noise\n";
	std::cout << "- " << cellCount << " vertices in X and Y directions\n";
	std::cout << "- Filename : '" << filename << "'" << std::endl;

	// Initialize the terrain with parameters for Perlin Noise generation
	TerrainMusgrave example_terrain(baseWaveLength, octaveCount);

	glm::vec2 terrain_box_min(0.f, 0.f);
	glm::vec2 terrain_box_max(size, size);
	Box2D terrain_box(terrain_box_min, terrain_box_max);

	// Initialize multi-layers with bedrock
	MultiLayers layers( example_terrain.heightfield(terrain_box, cellCount, cellCount, 0.f, maxZ));
	OBJExporter::save(layers.heightfield(), filename.c_str());	// Z is up
	TextureExporter::saveValue(layers.heightfield(), "value.png");

	// Erode bedrock into sediment
	Erosion::simulationLoop(layers, erosionStepCount);
	OBJExporter::save(layers.heightfield(), "eroded_terrain.obj");
	OBJExporter::save(layers.bedrock(), "bedrock_eroded_terrain.obj");
	OBJExporter::save(layers.sediment(), "sediment_eroded_terrain.obj");
	TextureExporter::saveMask(layers.sediment(), 1.f, {90, 150, 240, 255}, "sed.png");

	// Generate and simulate vegetation
	if (vegeSimuStepCount != 0) {
		Veget::SimuleAll(layers);
		TextureExporter::saveMask(layers.veget(), .1f, {50, 200, 50, 255}, "veget.png");
	}

	FreeImage_DeInitialise();

	return 0;
}
