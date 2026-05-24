#pragma once

#include <vector>
#include <cstdint>

namespace jps
{
	struct Cell
	{
		int x{ 0 };
		int y{ 0 };

		bool operator==(const Cell& other) const noexcept { return x == other.x && y == other.y; }
		bool operator!=(const Cell& other) const noexcept { return !(*this == other); }
	};

	class Grid final
	{
	public:
		Grid(int width, int height);

		int GetWidth() const noexcept { return m_Width; }
		int GetHeight() const noexcept { return m_Height; }
			
		bool InBounds(int x, int y) const noexcept;
		bool InBounds(const Cell& c) const noexcept;

		bool IsWalkable(int x, int y) const noexcept;
		bool IsWalkable(const Cell& c) const noexcept;

		void SetBlocked(int x, int y, bool blocked) noexcept;
		void SetBlocked(const Cell& c, bool blocked) noexcept;

		// Convert x/y to flat index from the container
		size_t Index(int x, int y) const noexcept;
		size_t Index(const Cell& c) const noexcept;

		size_t GetCellCount() const noexcept;

	private:
		enum CellState : uint8_t
		{
			Walkable = 0,
			Blocked = 1
		};

		int m_Width;
		int m_Height;
		std::vector<uint8_t> m_Cells;
	};
}