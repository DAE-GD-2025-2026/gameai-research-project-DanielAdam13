#include "Grid.h"

using namespace jps;

Grid::Grid(int width, int height)
	:m_Width{ width },
	m_Height{ height },
	m_Cells(static_cast<size_t>(width)* static_cast<size_t>(height), CellState::Walkable) // Initialize everything empty
{
}

bool jps::Grid::InBounds(int x, int y) const noexcept
{
	return x >= 0 && x < m_Width && y >= 0 && y < m_Height;
}

bool jps::Grid::InBounds(const Cell& c) const noexcept
{
	return InBounds(c.x, c.y);
}

bool jps::Grid::IsWalkable(int x, int y) const noexcept
{
	if (!InBounds(x, y))
		return false;

	return m_Cells[Index(x, y)] == CellState::Walkable;
}

bool jps::Grid::IsWalkable(const Cell& c) const noexcept
{
	return IsWalkable(c.x, c.y);
}

void jps::Grid::SetBlocked(int x, int y, bool blocked) noexcept
{
	if (!InBounds(x, y))
		return;

	m_Cells[Index(x, y)] = blocked ? CellState::Blocked : CellState::Walkable;
}

void jps::Grid::SetBlocked(const Cell& c, bool blocked) noexcept
{
	SetBlocked(c.x, c.y, blocked);
}

size_t jps::Grid::Index(int x, int y) const noexcept
{
	return static_cast<size_t>(y) * static_cast<size_t>(m_Width) + static_cast<size_t>(x);
}

size_t jps::Grid::Index(const Cell& c) const noexcept
{
	return Index(c.x, c.y);
}

size_t jps::Grid::GetCellCount() const noexcept
{
	return m_Cells.size();
}
