#include "GameEngine.h"
#include "JPSApp.h"

#include <filesystem>

#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

int main(int, char* [])
{
#if __EMSCRIPTEN__
    std::filesystem::path data_location = "";
#else
    std::filesystem::path data_location = "./resources/";
    if (!std::filesystem::exists(data_location))
        data_location = "../resources/";
#endif

    ge::GameEngine::GetInstance().InitializeEngine(data_location, "JPS Research");

    jps::JPSApp app{};
    ge::GameEngine::GetInstance().Run(app);
    return 0;
}
