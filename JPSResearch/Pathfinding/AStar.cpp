#include "AStar.h"
#include "PathFindingHelpers.h"

#include <queue>
#include <functional>

using namespace jps;
using namespace helpers;

SearchResult jps::AStar::FindPath(const Grid& grid, Cell start, Cell goal) const
{
    SearchResult result{};

    // Trivial rejection cases:
    if (!grid.InBounds(start) || !grid.InBounds(goal)) 
        return result;
    if (!grid.IsWalkable(start) || !grid.IsWalkable(goal)) 
        return result;

    // Allocate one grid per cell
    std::vector<NodeRecord> records(grid.GetCellCount());

    // Priority queue keyed for the F-COST:
    std::priority_queue<OpenEntry, std::vector<OpenEntry>, std::greater<>> open;

    const int startIdx{ static_cast<int>(grid.Index(start)) };
    const int goalIdx{ static_cast<int>(grid.Index(goal)) };    

    // Start the open list with the START node:
    records[startIdx].gCost = 0.f;
    records[startIdx].fCost = m_Heuristic(std::abs(goal.x - start.x), std::abs(goal.y - start.y));
    records[startIdx].inOpen = true;
    open.push({ records[startIdx].fCost, startIdx });
    ++result.stats.nodesGenerated;

    while (!open.empty())
    {
        const OpenEntry top{ open.top() };
        open.pop();

        const int currentIdx{ top.cellIndex };
        NodeRecord& currentRecord{ records[static_cast<size_t>(currentIdx)] };

        // Process ONLY with the OpenEntry that matches the fcost, SKIP the others
        if (top.fCost != currentRecord.fCost)
            continue;
        if (currentRecord.inClosed)
            continue;


        // ----------------------------------------
        // EXPANDED
        // ----------------------------------------
        currentRecord.inOpen = false;
        currentRecord.inClosed = true;
        ++result.stats.nodesExpanded;

        // Goal check happens on expansion (not on generation), because the
        // first time we EXPAND the goal we have its optimal g-cost.
        if (currentIdx == goalIdx)
        {
            result.path = ReconstructPath(grid, records, goalIdx);
            result.stats.pathLength = static_cast<int>(result.path.size());
            result.stats.pathCost = currentRecord.gCost;
            return result;
        }

        const int currentX{ currentIdx % grid.GetWidth() };
        const int currentY{ currentIdx / grid.GetWidth() };

        for (const Offset& neigh : Neighbors)
        {
            const int neighX{ currentX + neigh.dx };
            const int neighY{ currentY + neigh.dy };

            // Skip if neighbour not traversable:
            if (!IsStepAllowed(grid, currentX, currentY, neighX, neighY, m_AllowCornerCutting))
                continue;

            const int neighborIdx{ static_cast<int>(grid.Index(neighX, neighY)) };
            NodeRecord& neighbourRecord{ records[static_cast<size_t>(neighborIdx)] };

            // Skip if already checked:
            if (neighbourRecord.inClosed)
                continue;

            const float finalGCost{ currentRecord.gCost + neigh.cost };

            // Skip if not an improvement:
            if (finalGCost >= neighbourRecord.gCost)
                continue;

            neighbourRecord.parentIndex = currentIdx;
            neighbourRecord.gCost = finalGCost;
            neighbourRecord.fCost = finalGCost
                + m_Heuristic(std::abs(goal.x - neighX), std::abs(goal.y - neighY));
            neighbourRecord.inOpen = true;

            open.push({ neighbourRecord.fCost, neighborIdx });

            // ----------------------------------------
            // GENERATED
            // ----------------------------------------
            ++result.stats.nodesGenerated;
        }
    }

    return result;
}


