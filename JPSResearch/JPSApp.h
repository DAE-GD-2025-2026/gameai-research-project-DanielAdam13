#pragma once
#include "IGameApplication.h"
#include "Pathfinding/Grid.h"
#include "Pathfinding/AStar.h"

#include <memory>
#include "Pathfinding/Heuristics.h"

namespace ge
{
    class GameObject;
}

namespace jps
{
    class GridRendererComponent;

    // Pixel-space layout of Rendered Grid.
    // Exposed so mouse commands can map mouse coordinates
    struct GridLayout
    {
        float originX;
        float originY;
        float cellSize;
    };


    class JPSApp final : public ge::IGameApplication
    {
    public:
        JPSApp() = default;
        ~JPSApp() override = default;
        JPSApp(const JPSApp&) = delete;
        JPSApp(JPSApp&&) = delete;
        JPSApp& operator=(const JPSApp&) = delete;
        JPSApp& operator=(JPSApp&&) = delete;

        // ---- IGameApplication ----
        void Load() override;
        void Update(float deltaTime) override;
        void FixedUpdate(float fixedDelta) override;

        // ---- Read access ----
        const Grid& GetGrid() const noexcept { return *m_Grid; }
        Cell GetStart() const noexcept { return m_Start; }
        Cell GetGoal() const noexcept { return m_Goal; }
        GridLayout GetLayout() const noexcept { return m_Layout; }
        const SearchStats& GetLastStats() const noexcept { return m_LastStats; }
        bool LastSearchFound() const noexcept { return m_LastSearchFound; }

        // ---- Mutating operations (Comamands) ----
        void ToggleWall(Cell c);
        void SetStartCell(Cell c);
        void SetGoalCell(Cell c);

        // ---- Settings (ImGui) ----
        void SetHeuristic(heuristics::Fn h);
        void SetAllowCornerCutting(bool allow);
        void SetShowExpandedCells(bool show);

        // ---- Grid manipulation (ImGui) ----
        void GenerateRandomWalls(int densityPercent);
        void ClearWalls();

        // Public so the ImGui panel can trigger recomputes.
        // Used for mouse command as well.
        void RecomputePath();

    private:
        void BuildTestGrid();

        std::unique_ptr<Grid> m_Grid;
        Cell m_Start{ 10, 1 };
        Cell m_Goal{ 28, 5 };
        AStar m_AStar{};

        GridRendererComponent* m_GridRenderer{ nullptr };

        // Empty GO used as a "target" for the commands
        ge::GameObject* m_InputTarget{ nullptr };

        // For imgui:
        SearchStats m_LastStats{};
        bool m_LastSearchFound{ false };
        bool m_ShowExpandedCells{ true };

        // Cached layout
        GridLayout m_Layout{ 40.f, 40.f, 24.f };
    };
}
