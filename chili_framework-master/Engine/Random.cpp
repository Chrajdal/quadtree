#include "Random.h"

Random::Random(uint_least32_t seed)
{
	rng = std::mt19937(seed);
}

int32_t Random::next(int32_t minval, int32_t maxval)
{
	std::uniform_int_distribution<int32_t> dist(minval, maxval);
	return dist(rng);
}

double_t Random::next_double(double_t minval, double_t maxval)
{
	std::uniform_real_distribution<double_t> dist(minval, maxval);
	return dist(rng);
}


float_t Random::next_float(float_t minval, float_t maxval)
{
	std::uniform_real_distribution<float_t> dist(minval, maxval);
	return dist(rng);
}