#pragma once
#include "Components/Component.h"

#include "Pathfinding/Grid.h"
#include "Pathfinding/AStar.h"

#include <SDL3/SDL.h>
#include <vector>
#include <utility>

namespace jps
{
	class GridRendererComponent final : public ge::Component
	{
	public:
		static constexpr ge::ComponentTypeID StaticTypeID{ 30 };

		GridRendererComponent(ge::GameObject* owner);
		~GridRendererComponent() override = default;
		GridRendererComponent(const GridRendererComponent&) = delete;
		GridRendererComponent(GridRendererComponent&&) = delete;
		GridRendererComponent& operator=(const GridRendererComponent&) = delete;
		GridRendererComponent& operator=(GridRendererComponent&&) = delete;

		void FixedUpdateComponent(float) override {}
		void UpdateComponent(float) override {}
		void RenderComponent() const override; // Only renders

		void SetGrid(const Grid* grid) noexcept { m_pGrid = grid; }
		void SetStart(Cell c) noexcept { m_Start = c; m_HasStart = true; }
		void SetGoal(Cell c) noexcept { m_Goal = c; m_HasGoal = true; }
		void ClearStart() noexcept { m_HasStart = false; }
		void ClearGoal() noexcept { m_HasGoal = false; }

		void SetPath(std::vector<Cell> path) { m_Path = std::move(path); }

		void SetExpandedCells(std::vector<Cell> expanded) { m_Expanded = std::move(expanded); }

		void SetLayout(float originX, float originY, float cellSize) noexcept
		{
			m_OriginX = originX;
			m_OriginY = originY;
			m_CellSize = cellSize;
		}

	private:
		const Grid* m_pGrid{ nullptr }; // Non-owning ref

		Cell m_Start{};
		Cell m_Goal{};
		bool m_HasStart{ false };
		bool m_HasGoal{ false };

		std::vector<Cell> m_Path;
		std::vector<Cell> m_Expanded;

		float m_OriginX{ 40.f };
		float m_OriginY{ 40.f };
		float m_CellSize{ 20.f };

		void DrawCell(SDL_Renderer* r, int x, int y, SDL_Color color, bool filled) const;
		void DrawPathLine(SDL_Renderer* r) const;
	};
}