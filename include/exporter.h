#pragma once

#include "FreeImagePlus.h"

#include <string>

// Forward Declaration
struct Scal2D;

class OBJExporter {
public:
	static void save(const Scal2D& field, std::string filename);
};

class TextureExporter {
public:
	/**
	 * Export scalar values in image
	 * A rescale is needed for PNG export
	 */
	static void saveValue(const Scal2D& field, std::string filename);
	/**
	 * Export the color parameter for each value above 0, transparancy for the others
	 */
	static void saveMask(const Scal2D& field, float maxheight, RGBQUAD color, std::string filename);
};
