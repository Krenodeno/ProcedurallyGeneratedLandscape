#include "multiLayers.h"

#include <iostream> // @DEBUG

MultiLayers::MultiLayers(LayerScal2D bedrock, float sedSize) {
	Layers.push_back(bedrock);
	Layers.push_back(Scal2D( (Grid2D)bedrock, sedSize));
	Layers.push_back(Scal2D( (Grid2D)bedrock, 0.f));
	nx = bedrock.nx;
	ny = bedrock.ny;
	glm::ivec2 coord;
}

Scal2D MultiLayers::heightfield() const {
	Scal2D heightfield(Layers[0]);
	heightfield += Layers[1];

	return heightfield;
}

void MultiLayers::transferBedrockSed(const float transfer) {
	glm::ivec2 coord;
	for(int ix = 0; ix < nx; ++ix){
		coord.x = ix;
		for(int iy = 0; iy < ny; ++iy){
			coord.y = iy;
			transferBedrockSed(coord, transfer);
		}
	}
}

void MultiLayers::transferBedrockSed(const glm::ivec2& coord, const float transfer) {
	float tmp = std::min(transfer, Layers[0].value(coord));
	if(transfer < 0)
		std::cout << transfer << std::endl;
	Layers[0].suprValue(coord, tmp);
	Layers[1].addValue(coord, tmp);
}
