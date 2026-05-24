#pragma once
#include "IGameApplication.h"

namespace jps
{
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
    };
}
