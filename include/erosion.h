#pragma once

#include "multiLayers.h"


namespace Erosion{

	namespace Thermal{
		void erode(MultiLayers& terrain);
		static void alteration(MultiLayers& terrain, const float conversion_rate);
		static float alteration_function(float slope);
		static void transport(MultiLayers& terrain, const float max_transport);
		static int transport(const glm::ivec2& terrain_cell,
							glm::ivec2* terrain_neighbor, float* terrain_data, float* terrain_delta_percent,
							Scal2D& updated_heightfield, Scal2D& sediments,
							float threshold_value);
		static void transport(Scal2D& bedrock, Scal2D& sediments);
		static int stabilize_pile(const glm::ivec2& v, MultiLayers& terrain,
					glm::ivec2* neighbor, float* value, float* gradient, const float stabilityAngle, const float max_transport);
		static void oneTransport(MultiLayers& terrain, const float stabilityAngle, const float max_transport);
	}

	namespace Hydraulic {
		void erode(MultiLayers& terrain);
	}

	void simulationLoop(MultiLayers& terrain, int iteration);
}