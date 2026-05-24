add_executable(jpsResearch WIN32
    Main.cpp
    JPSApp.h
    JPSApp.cpp
 "Pathfinding/Grid.h" "Pathfinding/Grid.cpp" "Pathfinding/Heuristics.h" "Pathfinding/AStar.h" "Pathfinding/AStar.cpp" "Pathfinding/PathFindingHelpers.h" "Pathfinding/PathFindingHelpers.cpp")

target_include_directories(jpsResearch PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Linking the engine pulls in SDL3, SDL3_ttf, glm, ImGui transitively.
target_link_libraries(jpsResearch PRIVATE gameEngine_lib)

ge_apply_warnings(jpsResearch)

# Post-build glue: copies SDL DLLs + resources/ next to the exe and sets
# the Visual Studio startup project + working directory.
ge_install_runtime(jpsResearch)
