#include "PathFindingHelpers.h"
#include "Grid.h"

using namespace jps;

bool jps::helpers::IsStepAllowed(const Grid& grid, int fromX, int fromY, int toX, int toY, bool allowCornerCutting) noexcept
{
    if (!grid.IsWalkable(toX, toY))
        return false;

    const int distanceX{ toX - fromX };
    const int distanceY{ toY - fromY };

    if (distanceX == 0 || distanceY == 0)
        return true;

    // Diagonal Step:
    if (!allowCornerCutting)
    {
        if (!grid.IsWalkable(fromX + distanceX, fromY))
            return false;

        if (!grid.IsWalkable(fromX, fromY + distanceY))
            return false;
    }

    return true;
}

std::vector<Cell> jps::helpers::ReconstructPath(const Grid& grid, const std::vector<NodeRecord>& records, int goalIndex)
{
    std::vector<Cell> path;
    const int width{ grid.GetWidth() };

    int idx{ goalIndex };
    while (idx != -1)
    {
        const int x{ idx % width };
        const int y{ idx / width };
        path.push_back(Cell{ x,y });

        // Set new index as parent index - node reaches TO this point
        idx = records[static_cast<size_t>(idx)].parentIndex;
    }
    std::reverse(path.begin(), path.end());

    return path;
}