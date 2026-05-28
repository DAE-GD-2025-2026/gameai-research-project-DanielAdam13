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

	namespace jpsHelpers
	{
		struct Dir {
			int dx;
			int dy;
		};

		// Clamps values for dirX and dirY to { -1, 0, +1} because
		// the start node does not have a parent and a Jump Point might be many cells away...
		int Sign(int v) noexcept;
		// Direction from parent we arrived FROM
		Dir DirectionFromParent(int px, int py, int cx, int cy) noexcept;

		inline bool IsDiagonal(Dir d) noexcept { return d.dx != 0 && d.dy != 0; }
		inline bool IsHorOrVert(Dir d) noexcept { return (d.dx == 0) != (d.dy == 0); }

		float StepCost(Dir d) noexcept;

		// -------------------------------------------
		// Forced Neighbour method
		// -------------------------------------------
		bool HasForcedNeighbourHorOrVert(const Grid& grid, int x, int y, Dir d) noexcept;

		bool HasForcedNeighbourDiagonal(const Grid& grid, int x, int y, Dir d) noexcept;

		// -------------------------------------------
		// JUMP
		// -------------------------------------------
		// From(x, y), step once in direction d, then recursively jump in
		// the same direction. Return the first jump point encountered, or -1 if the search runs off the grid / into a wall.
		// A jump point is:
        //   1) the goal
        //   2) a cell with a forced neighbour (hor/vert or diagonal)
        //   3) (diagonal only) a cell from which a cardinal jump in either
        //      of d's component directions finds its own jump point
		int Jump(const Grid& grid, int x, int y, Dir d, Cell goal, int& cellsScanned);

		// -------------------------------------------
		// Pruned successor set
		// -------------------------------------------
		// We input the direction of arrival and return the directions we should try to jump in. (to the Natural Neighbours)
		void GetPrunedDirections(const Grid& grid, int x, int y, Dir parentDir,
			Dir(&out)[5], int& outCount);

		// -------------------------------------------
		// Path reconstruction
		// -------------------------------------------
		// Returns the jump points - the SPARSE path
		std::vector<Cell> ReconstructJumpPoints(const Grid& gird,
			const std::vector<helpers::NodeRecord>& records, int goalIdx);

		// Returns every single node checked from jump point to point - the DENSE path
		std::vector<Cell> InterpolatePath(const std::vector<Cell>& jumpPoints);

		// Total cost of the SPARSE path (sums distances between jump points)
		float SumPathCost(const std::vector<Cell>& jumpPoints) noexcept;
	}
}