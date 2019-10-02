#include "exporter.h"

#include "scal2D.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

void OBJExporter::save(const Scal2D& field, std::string filename) {
	std::ofstream file;
	file.open(filename);

	// checking successful opening
	if(!file.is_open()) {
		throw std::runtime_error("Cannot open file for exporting !");
	}

	file << "# VERTICES with (x, y, z) coordinates\n";

	for(int ix = 0; ix != field.nx; ++ix) {
		for(int iy = 0; iy != field.ny; ++iy) {

			const float x = (field.max.x - field.min.x) / field.nx * ix;
			const float y = (field.max.y - field.min.y) / field.ny * iy;

			file << "v " << x << " " << y << " " << field.values[field.index(ix, iy)] << "\n";
		}
	}

	file << "# TEXTURE coordinates\n";

	for(int ix = 0; ix != field.nx; ++ix) {
		for(int iy = 0; iy != field.ny; ++iy) {

			const float u = ix / (float)field.nx;
			const float v = iy / (float)field.ny;

			file << "vt " << u << " " << v << "\n";
		}
	}

	file << "# FACES as triangles\n";

	// two triangular face elements per grid square
	for (int ifacex = 0; ifacex != field.nx - 1; ++ifacex) {
		for (int ifacey = 0; ifacey != field.ny - 1; ++ifacey) {

			// A---C
			// | / |
			// B---D

			const int vertexA = field.index(ifacex, ifacey) + 1;
			const int vertexB = field.index(ifacex, ifacey + 1) + 1;
			const int vertexC = field.index(ifacex + 1, ifacey) + 1;
			const int vertexD = field.index(ifacex + 1, ifacey + 1) + 1;

			// faces ABC & BDC

			file << "f " << vertexC << "/" << vertexC << " " << vertexB << "/" << vertexB << " " << vertexA << "/" << vertexA << "\n";
			file << "f " << vertexB << "/" << vertexB << " " << vertexC << "/" << vertexC << " " << vertexD << "/" << vertexD << "\n";
		}
	}

	file.flush();
	file.close();
}

void TextureExporter::saveValue(const Scal2D& field, std::string filename) {

	//Scal2D f = field.rescale(0.f, 255.f);

	fipImage image(FIT_FLOAT, field.nx, field.ny, 32);

	for (int x = 0; x != field.nx; ++x) {
		for (int y = 0; y != field.ny; ++y) {
			float v = field.values[field.index(x, y)];
			//BYTE v = (BYTE)(value);
			RGBQUAD color = {v, v, v, 255};
			image.setPixelColor(x, y, &color);
		}
	}

	image.save(FIF_RAW, filename.c_str());

}

void TextureExporter::saveMask(const Scal2D& field, float maxheight, RGBQUAD color, std::string filename) {

	fipImage image(FIT_BITMAP, field.nx, field.ny, 32);

	for (int x = 0; x < field.nx; ++x) {
		for (int y = 0; y < field.ny; ++y) {
			float value = field.values[field.index(x, y)];
			RGBQUAD rgba;			
			if (value >= maxheight) {
				rgba = color;
			}
			else {
				rgba = color;
				rgba.rgbReserved = BYTE(value * 255.f / maxheight);
			}
			image.setPixelColor(x, y, &rgba);
		}
	}

	image.save(FIF_PNG, filename.c_str());

}
