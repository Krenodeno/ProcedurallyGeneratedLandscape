#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "multiLayers.h"

TEST_CASE( "Testing Box2D methods", "[Box2D]") {

	Box2D first({-2, -2}, {2, 2});

	CHECK( first.inside({0, 0}) );
	CHECK( !first.inside({-3, 5}) );

}
