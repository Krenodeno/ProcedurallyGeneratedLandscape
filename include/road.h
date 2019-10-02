#pragma once

#include "scal2D.h"

namespace Road {

	struct Neighbor {
		int target;
		float weight;

		Neighbor(int target, float weight) : target(target), weight(weight) {}
	};


};
