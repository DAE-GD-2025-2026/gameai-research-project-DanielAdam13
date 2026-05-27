#include "JPS.h"
#include "Grid.h"
#include "PathFindingHelpers.h"

#include <vector>
#include <queue>
#include <functional>
#include <cmath>
#include <algorithm>

jps::SearchResult jps::JPS::FindPath(const Grid& grid, Cell start, Cell goal) const
{
	SearchResult result{};

	if (!grid.InBounds(start) || !grid.InBounds(goal))
		return result;
	if (!grid.IsWalkable(start) || !grid.IsWalkable(goal))
		return result;

	std::vector<helpers::NodeRecord> records(grid.GetCellCount());
	std::priority_queue<helpers::OpenEntry, std::vector<helpers::OpenEntry>, std::greater<>> open;
	
	const int startIdx{ static_cast<int>(grid.Index(start)) };
	const int goalIdx{ static_cast<int>(grid.Index(goal)) };

	// Push start to open list/queue - same as A*
	records[startIdx].gCost = 0.f;
	records[startIdx].fCost = m_Heuristic(std::abs(goal.x - start.x), std::abs(goal.y - start.y));
	records[startIdx].inOpen = true;
	open.push({ records[startIdx].fCost, startIdx });
	++result.stats.nodesGenerated;

	const int W{ grid.GetWidth() };

	while (!open.empty())
	{
		const helpers::OpenEntry top{ open.top() };
		open.pop();

		const int currentIdx{ top.cellIndex };
		helpers::NodeRecord& current = records[static_cast<size_t>(currentIdx)];

		// Stale entry skip - same as A*
		if (top.fCost != current.fCost) 
			continue;
		if (current.inClosed)
			continue;

		// ----------------------------------------
		// EXPANDED - same as A*
		// ----------------------------------------
		current.inClosed = false;
		current.inOpen = true;
		++result.stats.nodesExpanded;

		const int cx{ currentIdx % W };
		const int cy{ currentIdx / W };

		if (m_TrackExpanded)
			result.expandedCells.push_back(Cell{ cx, cy });

		// Goal check on expainsion
		if (currentIdx == goalIdx)
		{
			// Reconstruct all jump points + path from the NodeRecord
			std::vector<Cell> jumpPoints{ jpsHelpers::ReconstructJumpPoints(grid, records, goalIdx) };
			result.path = m_InterpolatePath ? jpsHelpers::InterpolatePath(jumpPoints) : jumpPoints;
			result.stats.pathLength = static_cast<int>(result.path.size());
			result.stats.pathCost = jpsHelpers::SumPathCost(jumpPoints);
			return result;
		}

		// If expanded node is NOT the goal (most of the time):

		// -- Determine direction-of-arrival from parent --
		jpsHelpers::Dir parentDir{ 0, 0 };
		if (current.parentIndex != -1)
		{
			const int px{ current.parentIndex % W };
			const int py{ current.parentIndex / W };
			parentDir = jpsHelpers::DirectionFromParent(px, py, cx, cy);
		}

		// -- Get pruned successor directions --
		jpsHelpers::Dir dirs[5];
		int dirCount{ 0 };
		if (parentDir.dx == 0 && parentDir.dy == 0)
		{
			// Start node: all 8 directions
			jpsHelpers::Dir all8[8];
			int n{ 0 };
			for (int dy{ -1 }; dy <= 1; ++dy)
			{
				for (int dx{ -1 }; dx <= 1; ++dx)
				{
					if (dx != 0 || dy != 0)
					{
						all8[n++] = jpsHelpers::Dir{ dx, dy };
					}
				}
			}

			for (int i{ 0 }; i < n; ++i)
			{
				// =======================================
				// JUMP
				// =======================================
				const int jpIdx{ jpsHelpers::Jump(grid, cx, cy, all8[i], goal) };
				// If jump is invalid, continue to next direction
				if (jpIdx == -1)
					continue;

				const int jx{ jpIdx % W };
				const int jy{ jpIdx / W };
				helpers::NodeRecord& jp{ records[static_cast<size_t>(jpIdx)] };
				if (jp.inClosed)
					continue;

				// Distance from current to jump point along this run
				const int dx{ std::abs(jx - cx) };
				const int dy{ std::abs(jy - cy) };
				const int diag{ std::min(dx, dy) };
				const int straight{ std::max(dx, dy) - diag };

				const float finalGCost{ current.gCost +
					static_cast<float>(diag) * helpers::DiagonalCost +
					static_cast<float>(straight) * helpers::StraightCost };

				// If the current checked G Cost along the run is bigger than the current one,
				// it means this jump/run was NOT BETTER
				if (finalGCost >= jp.gCost)
					continue;

				jp.parentIndex = currentIdx;
				jp.gCost = finalGCost;
				jp.fCost = finalGCost + m_Heuristic(std::abs(goal.x - jx), std::abs(goal.y - jy));
				jp.inOpen = true;
				open.push({ jp.fCost, jpIdx });
				++result.stats.nodesGenerated;
			}
			continue;
		}

		// After we've found a jump point, get directions TO THE Native Neighbours
		jpsHelpers::GetPrunedDirections(grid, cx, cy, parentDir, dirs, dirCount);

		// -- For each pruned direction, find the jump point and relax it --
		for (int i{ 0 }; i < dirCount; ++i)
		{
			// =======================================
			// JUMP
			// =======================================
			const int jpIdx{ jpsHelpers::Jump(grid, cx, cy, dirs[i], goal) };
			if (jpIdx == -1) 
				continue;

			const int jx{ jpIdx % W };
			const int jy{ jpIdx / W };
			helpers::NodeRecord& jp{ records[static_cast<size_t>(jpIdx)] };
			if (jp.inClosed)
				continue;

			// Distance from current to jump point along this run
			const int dx{ std::abs(jx - cx) };
			const int dy{ std::abs(jy - cy) };
			const int diag{ std::min(dx, dy) };
			const int straight{ std::max(dx, dy) - diag };

			const float finalGCost{ current.gCost +
				static_cast<float>(diag) * helpers::DiagonalCost +
				static_cast<float>(straight) * helpers::StraightCost };

			// If the current checked G Cost along the run is bigger than the current one,
			// it means this jump/run was NOT BETTER
			if (finalGCost >= jp.gCost)
				continue;

			jp.parentIndex = currentIdx;
			jp.gCost = finalGCost;
			jp.fCost = finalGCost + m_Heuristic(std::abs(goal.x - jx), std::abs(goal.y - jy));
			jp.inOpen = true;
			open.push({ jp.fCost, jpIdx });
			++result.stats.nodesGenerated;
		}
	}

	return result;
}
