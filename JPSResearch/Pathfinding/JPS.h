#pragma once
#include "Heuristics.h"
#include "AStar.h" // for SearchResult / SearchStats

namespace jps
{
	class Grid;
	struct Cell;

	// JPS with 2012 - no corner cutting rules, see research README for more info
	class JPS final
	{
	public:
		JPS() = default;

		void SetHeuristic(heuristics::Fn h) noexcept { m_Heuristic = h; }
		void SetTrackExpanded(bool track) noexcept { m_TrackExpanded = track; }

		void SetShowInterpolatedPath(bool interp) noexcept { m_ShowDenseInterpolatedPath = interp; }

		SearchResult FindPath(const Grid& grid, Cell start, Cell goal) const;

	private:
		heuristics::Fn m_Heuristic{ &heuristics::Octile };

		bool m_TrackExpanded{ true };

		bool m_ShowDenseInterpolatedPath{ false };

	};
}