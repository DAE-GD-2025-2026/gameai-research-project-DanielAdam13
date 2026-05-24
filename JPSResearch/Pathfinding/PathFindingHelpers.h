#pragma once

#include <vector>
#include <limits>

namespace jps
{
	class Grid;
	struct Cell;

	namespace helpers
	{
		// =================================================
		// Helpers:
		// =================================================
		static inline constexpr float StraightCost{ 1.f };
		static inline constexpr float DiagonalCost{ 1.41421356237f }; // sqrt(2)

		// 8 neighbour offsets
		struct Offset
		{
			int dx;
			int dy;
			float cost;
		};
		static inline constexpr Offset Neighbors[8]{
			{ -1, 0, StraightCost }, // W
			{ 1, 0, StraightCost }, // E
			{ 0, -1, StraightCost }, // N
			{ 0, 1, StraightCost }, // S
			{ -1, -1, DiagonalCost }, // NW
			{ 1, -1, DiagonalCost }, // NE
			{ -1, 1, DiagonalCost }, // SW
			{ 1, 1, DiagonalCost }, // SE
		};

		// One entry per cell in the grid.
		struct NodeRecord
		{
			float gCost{ std::numeric_limits<float>::infinity() }; // cost from start
			float fCost{ std::numeric_limits<float>::infinity() }; // g + h
			int parentIndex{ -1 }; // index into grid; -1 == no parent
			bool inOpen{ false };
			bool inClosed{ false };
		};

		// Item we put on the priority queue. 
		// When a better g-cost for a cell already in the grid is found -> just push a new entry.
		struct OpenEntry
		{
			float fCost;
			int cellIndex;

			// std::priority_queue is a max-heap by default => flip compare
			bool operator>(const OpenEntry& other) const noexcept { return fCost > other.fCost; }
		};


		// ----------------------------------------------------------------

		bool IsStepAllowed(const Grid& grid, int fromX, int fromY, int toX, int toY,
			bool allowCornerCutting = false) noexcept;

		// Walk the parent pointers from goal back to start.
		std::vector<Cell> ReconstructPath(const Grid& grid, const std::vector<NodeRecord>& records, int goalIndex);
	}
}