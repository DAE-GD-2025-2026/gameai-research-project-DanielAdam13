#pragma once
#include "Grid.h"
#include "Heuristics.h"

#include <vector>

namespace jps
{
	// Statistics gathered during a search. Will use for comparison JPS <-> Astar
	struct SearchStats
	{
		int nodesExpanded{ 0 }; // These nodes were at one point CHECKED and then POPPED from the open list
		int nodesGenerated{ 0 }; // Locked-in, sure nodes PUSHED on the open list
		int pathLength{ 0 };
		float pathCost{ 0.f };
	};

	struct SearchResult
	{
		std::vector<Cell> path; // All nodes, empty if path not found
		SearchStats stats{};

		// Used for visualization:
		std::vector<Cell> expandedCells;

		std::vector<Cell> jumpPoints; // JPS only

		// Returns true if path is not empty
		bool Found() const noexcept { return !path.empty(); }
	};


	class AStar final
	{
	public:
		AStar() = default;

		void SetHeuristic(heuristics::Fn h) noexcept { m_Heuristic = h; }

		void SetAllowCornerCutting(bool allow) noexcept { m_AllowCornerCutting = allow; }

		SearchResult FindPath(const Grid& grid, Cell start, Cell goal) const;

	private:
		heuristics::Fn m_Heuristic{ &heuristics::Octile };

		bool m_AllowCornerCutting{ false }; // Off by default

		bool m_TrackExpanded{ true }; // On by default
	};
}