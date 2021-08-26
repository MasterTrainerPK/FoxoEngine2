#pragma once

namespace feMath
{
	double RNG(double min, double max);

	int FastFloor(double x);
	int FastFloor(float x);

	template<typename t_Type>
	t_Type Min(const t_Type& x, const t_Type& y)
	{
		return (y < x) ? y : x;
	}

	template<typename t_Type>
	t_Type Max(const t_Type& x, const t_Type& y)
	{
		return (y < x) ? x : y;
	}

	template<typename t_Type>
	t_Type Map(const t_Type& value, const t_Type& min1, const t_Type& max1, const t_Type& min2, const t_Type& max2)
	{
		return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
	}

	template<typename t_Type>
	t_Type Clamp(const t_Type& value, const t_Type& min, const t_Type& max)
	{
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}
}