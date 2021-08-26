#include "Math.h"

#include <random>

static std::random_device s_Dev;
static std::mt19937_64 s_Rng(s_Dev());

namespace feMath
{
	double RNG(double min, double max)
	{
		std::uniform_real_distribution dist(Min(min, max), Max(min, max));
		return dist(s_Rng);
	}

	int FastFloor(double x)
	{
		int xi = static_cast<int>(x);
		return x < xi ? xi - 1 : xi;
	}

	int FastFloor(float x)
	{
		int xi = static_cast<int>(x);
		return x < xi ? xi - 1 : xi;
	}
}