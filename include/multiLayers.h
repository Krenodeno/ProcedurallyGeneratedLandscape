#pragma once

#include "layer.h"

struct MultiLayers {
	private:
		std::vector<LayerScal2D> Layers;

	public:
		MultiLayers() {};
		MultiLayers(LayerScal2D bedrock, float sedSize = 1.f);
		LayerScal2D& bedrock() { return Layers[0]; }
		LayerScal2D& sediment() { return Layers[1]; }
		LayerScal2D& veget() { return Layers[2]; }
		Scal2D heightfield() const;
		void transferBedrockSed(const float transfer);
		void transferBedrockSed(const glm::ivec2& coord,const float transfer);

		// number of points on each axis of the grid
		int nx;
		int ny;
};
