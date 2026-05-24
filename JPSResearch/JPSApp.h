#pragma once
#include "IGameApplication.h"
#include "Pathfinding/Grid.h"
#include "Pathfinding/AStar.h"

#include <memory>

namespace jps
{
    class GridRendererComponent;

    class JPSApp final : public ge::IGameApplication
    {
    public:
        JPSApp() = default;
        ~JPSApp() override = default;
        JPSApp(const JPSApp&) = delete;
        JPSApp(JPSApp&&) = delete;
        JPSApp& operator=(const JPSApp&) = delete;
        JPSApp& operator=(JPSApp&&) = delete;

        void Load() override;
        void Update(float deltaTime) override;
        void FixedUpdate(float fixedDelta) override;

    private:
        void BuildTestGrid();

        void RecomputePath();

        std::unique_ptr<Grid> m_Grid;
        Cell m_Start{ 10, 1 };
        Cell m_Goal{ 28, 5 };
        AStar m_AStar{};

        GridRendererComponent* m_pRenderer{ nullptr };
    };
}
