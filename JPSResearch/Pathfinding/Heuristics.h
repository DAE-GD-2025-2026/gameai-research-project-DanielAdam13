#pragma once

#include <glm/glm.hpp>
#include <cmath>
#include <algorithm>

namespace jps::heuristics
{
	using Fn = float(*)(int, int); // Fn pointer used in Heuristic method parameter passing

	// Manhattan: optimal for 4-connected grids only. On 8-connected grids it
	// overestimates and produces suboptimal paths.
	inline float Manhattan(int dx, int dy) noexcept
	{
		return static_cast<float>(dx + dy);
	}

	// Euclidean: straight-line distance. Slower than Octile for grids since it 
	// underestimates more aggressively.
	inline float Euclidean(int dx, int dy) noexcept
	{
		const float fx{ static_cast<float>(dx) };
		const float fy{ static_cast<float>(dy) };
		return std::sqrtf(fx * fx + fy * fy);
	}

	// Octile: the standard heuristic for 8-connected grids with uniform costs.
	// Models a path that goes diagonally as much as possible then straight.
	// This is what JPS expects.
	inline float Octile(int dx, int dy) noexcept
    {
        constexpr float kSqrt2Minus1{ 0.41421356237f }; // sqrt(2) - 1
        const int lo{ std::min(dx, dy) };
        const int hi{ std::max(dx, dy) };
        return kSqrt2Minus1 * static_cast<float>(lo) + static_cast<float>(hi);
    }

	// Chebyshev: like Octile but assumes diagonal cost == straight cost.
    inline float Chebyshev(int dx, int dy) noexcept
    {
        return static_cast<float>(std::max(dx, dy));
    }


}