#include "PathFindingHelpers.h"
#include "Grid.h"
#include <cmath>

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

int jps::jpsHelpers::Sign(int v) noexcept
{
    return (v > 0) - (v < 0);
}

jpsHelpers::Dir jps::jpsHelpers::DirectionFromParent(int px, int py, int cx, int cy) noexcept
{
    return Dir{ Sign(cx - px), Sign(cy - py) };
}

float jps::jpsHelpers::StepCost(Dir d) noexcept
{
    return IsDiagonal(d) ? helpers::DiagonalCost : helpers::StraightCost;
}

bool jps::jpsHelpers::HasForcedNeighbourHorOrVert(const Grid& grid, int x, int y, Dir d) noexcept
{
    if (d.dx != 0) // Moving horizontally
    {
        if (!grid.IsWalkable(x - d.dx, y - d.dy) && grid.IsWalkable(x, y - 1))
            return true;

        if (!grid.IsWalkable(x - d.dx, y + 1) && grid.IsWalkable(x, y + 1))
            return true;
    }
    else // Vertically
    {
        if (!grid.IsWalkable(x - 1, y - d.dy) && grid.IsWalkable(x - 1, y)) 
            return true;

        if (!grid.IsWalkable(x + 1, y - d.dy) && grid.IsWalkable(x + 1, y)) 
            return true;

    }
    return false;
}

bool jps::jpsHelpers::HasForcedNeighbourDiagonal(const Grid& grid, int x, int y, Dir d) noexcept
{
    if (!grid.IsWalkable(x - d.dx, y) && grid.IsWalkable(x - d.dx, y + d.dy)) 
        return true;

    if (!grid.IsWalkable(x, y - d.dy) && grid.IsWalkable(x + d.dx, y - d.dy)) 
        return true;

    return false;

}

int jps::jpsHelpers::Jump(const Grid& grid, int x, int y, Dir d, Cell goal)
{
    const int nx{ x + d.dx };
    const int ny{ y + d.dy };

    // EXCEPTION WHEN REACHING Wall/End of map when going right
    if (!grid.IsWalkable(nx, ny)) 
        return -1;

    // Same logic but for diagonal step
    if (IsDiagonal(d))
    {
        if (!grid.IsWalkable(x + d.dx, y) || !grid.IsWalkable(x, y + d.dy))
            return -1;
    }

    // (1) Goal reached !!!
    if (nx == goal.x && ny == goal.y)
    {
        return static_cast<int>(grid.Index(nx, ny)); // Jump
    }

    // (2) Forced neighbour check at new cell
    if (IsHorOrVert(d))
    {
        if (HasForcedNeighbourHorOrVert(grid, nx, ny, d))
        {
            return static_cast<int>(grid.Index(nx, ny)); // Jump
        }
    }
    else // Diagonal
    {
        if (HasForcedNeighbourDiagonal(grid, nx, ny, d))
        {
            return static_cast<int>(grid.Index(nx, ny)); // Jump
        }

        // (3) Recurse along the hor and vert BEFORE continuing diagonally
        if (Jump(grid, nx, ny, Dir{d.dx, 0}, goal) != -1)
        {
            return static_cast<int>(grid.Index(nx, ny)); // Jump
        }
        if(Jump(grid, nx, ny, Dir{0, d.dy}, goal) != -1)
        {
            return static_cast<int>(grid.Index(nx, ny)); // Jump
        }
    }

    return Jump(grid, nx, ny, d, goal); // Else, keep jumping recursively
}

void jps::jpsHelpers::GetPrunedDirections(const Grid& grid, int x, int y, Dir parentDir, Dir(&out)[5], int& outCount)
{
    outCount = 0;

    // No parent means starting => All 8 directions
    if (parentDir.dx == 0 && parentDir.dy == 0)
    {
        for (int dy{ -1 }; dy <= 1; ++dy)
        {
            for (int dx{ -1 }; dx <= 1; ++dx)
            {
                if (((dx != 0) || (dy != 0)) || outCount < 5)
                {
                    out[outCount++] = Dir{ dx, dy };
                }
            }
        }

        return;
    }

    if (IsDiagonal(parentDir))
    {
        // 1. Diagonal itself
        // 2. Direction of hor and vert
        out[outCount++] = Dir{ parentDir.dx, parentDir.dy };
        out[outCount++] = Dir{ parentDir.dx, 0 };
        out[outCount++] = Dir{ 0, parentDir.dy };
        
        // FORCED NEIGHBOURS:
        // Top left
        if (!grid.IsWalkable(x - parentDir.dx, y) && grid.IsWalkable(x - parentDir.dx, y + parentDir.dy))
        {
            out[outCount++] = Dir{ -parentDir.dx, parentDir.dy };
        }
        // Bottom right
        if (!grid.IsWalkable(x, y - parentDir.dy) && grid.IsWalkable(x + parentDir.dx, y - parentDir.dy))
        {
            out[outCount++] = Dir{ parentDir.dx, -parentDir.dy };
        }
    }
    else // Horizontal or Vertical
    {
        // Keep going forward
        out[outCount++] = parentDir;

        if (parentDir.dx != 0) // Horizontal arrival
        {
            // When a wall sits at (x - dx, y - 1) and (x, y - 1) is
            // open, two cells become forced neighbours: 
            // 1. perpendicular cardinal (x, y - 1) and 
            // and
            // 2. the diagonal(x + dx, y - 1).
            if (!grid.IsWalkable(x - parentDir.dx, y - 1) && grid.IsWalkable(x, y - 1))
            {
                out[outCount++] = Dir{ 0, -1 };
                out[outCount++] = Dir{ parentDir.dx, -1 };
            }
            // Same for (y = +1 side)
            if (!grid.IsWalkable(x - parentDir.dx, y + 1) && grid.IsWalkable(x, y + 1))
            {
                out[outCount++] = Dir{ 0, 1 };
                out[outCount++] = Dir{ parentDir.dx, 1 };
            }
        }
        else // Vertical
        {
            // - || - same logic as horizontal
            if (!grid.IsWalkable(x - 1, y - parentDir.dy) && grid.IsWalkable(x - 1, y))
            {
                out[outCount++] = Dir{ -1, 0 };
                out[outCount++] = Dir{ -1, parentDir.dy };
            }
            if (!grid.IsWalkable(x + 1, y - parentDir.dy) && grid.IsWalkable(x + 1, y))
            {
                out[outCount++] = Dir{ 1, 0 };
                out[outCount++] = Dir{ 1, parentDir.dy };
            }
        }
    }
}

std::vector<Cell> jps::jpsHelpers::ReconstructJumpPoints(const Grid& grid, 
    const std::vector<helpers::NodeRecord>& records, int goalIdx)
{
    std::vector<Cell> jumpPoints;
    const int W{ grid.GetWidth() };
    int idx{ goalIdx };

    while (idx != -1)
    {
        jumpPoints.push_back(Cell{ idx % W, idx / W });
        idx = records[static_cast<size_t>(idx)].parentIndex;
    }

    std::reverse(jumpPoints.begin(), jumpPoints.end());
    return jumpPoints;
}

std::vector<Cell> jps::jpsHelpers::InterpolatePath(const std::vector<Cell>& jumpPoints)
{
    if (jumpPoints.size() < 2) 
        return jumpPoints;

    std::vector<Cell> dense;
    dense.push_back(jumpPoints.front());

    // For every jump point (besides the end goal)
    for (size_t i{ 1 }; i < jumpPoints.size(); ++i)
    {
        const Cell& a{ jumpPoints[i - 1] };
        const Cell& b{ jumpPoints[i] };
        const int sx{ Sign(b.x - a.x) };
        const int sy{ Sign(b.y - a.y) };

        int x{ a.x };
        int y{ a.y };

        // Just build and push the path for each jump point to point path
        while (x != b.x || y != b.y)
        {
            x += sx;
            y += sy;
            dense.push_back(Cell{ x, y });
        }
    }

    return dense;
}

float jps::jpsHelpers::SumPathCost(const std::vector<Cell>& jumpPoints) noexcept
{
    float cost{ 0.f };

    for (size_t i{ 1 }; i < jumpPoints.size(); ++i)
    {
        const int dx{ std::abs(jumpPoints[i].x - jumpPoints[i - 1].x) };
        const int dy{ std::abs(jumpPoints[i].y - jumpPoints[i - 1].y) };
        const int diag{ std::min(dx, dy) };

        const int straight{ std::max(dx,dy) - diag };

        cost += static_cast<float>(diag) * helpers::DiagonalCost +
            static_cast<float>(straight) * helpers::StraightCost;

    }

    return cost;
}
