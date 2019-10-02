#pragma once

#include <memory>

#include "multiLayers.h"
#include <FastNoiseSIMD.h>

/** Abstract class used to generate heightfield
 */
class Terrain {
public:
	Terrain();
	virtual ~Terrain();

	virtual Scal2D heightfield(const Box2D& box, int nbCellsX, int nbCellsY, float minZ, float maxZ) = 0;
	Scal2D fromFile(const char* path, const Box2D& box, int nx, int ny, float minZ, float maxZ);
};

/** Perlin Noise based heightfield generator
 */
class TerrainMusgrave : public Terrain {
public:

	// We want to set :
	// - the heighfield size (in x, y, z units)
	// - the number of cells / the size of each cells
	// - the base frequency and the number of octaves / the size of mountains and the number of halfing
	TerrainMusgrave(float baseWaveLength, int octaveCount, int seed = 1337);


	TerrainMusgrave(const TerrainMusgrave& src);
	~TerrainMusgrave();

	Scal2D heightfield(const Box2D& box,
					int nbCellsX, int nbCellsY,
					float minZ, float maxZ);

	void setMountainSize(float newSize);
	void setOctaveCount(int count);
	void setSeed(int newSeed);

private:

	FastNoiseSIMD* initGenerator(float frequency, int seed);

	/** FastNoiseSIMD documentation : https://github.com/Auburns/FastNoiseSIMD/wiki/Noise-Settings
	 */

	float baseWaveLength;
	int octaveCount;
	int seed;
};
