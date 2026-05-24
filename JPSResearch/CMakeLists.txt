add_executable(jpsResearch WIN32
    Main.cpp
    JPSApp.h
    JPSApp.cpp
 "Pathfinding/Grid.h" "Pathfinding/Grid.cpp" 
 "Pathfinding/Heuristics.h" 
 "Pathfinding/AStar.h" "Pathfinding/AStar.cpp" 
 "Pathfinding/PathFindingHelpers.h" "Pathfinding/PathFindingHelpers.cpp" 
 "Visualization/GridRendererComponent.h" "Visualization/GridRendererComponent.cpp")

target_include_directories(jpsResearch PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Linking the engine pulls in SDL3, SDL3_ttf, glm, ImGui transitively.
target_link_libraries(jpsResearch PRIVATE gameEngine_lib)

ge_apply_warnings(jpsResearch)

#----------------------------
# Platform-specific post-build steps. Copied verbatim from BombermanGame.
#----------------------------
if(WIN32)
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT jpsResearch)
    set_target_properties(jpsResearch PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:jpsResearch>")

    set(SDL_RUNTIME_DLLS
        SDL3::SDL3
        SDL3_ttf::SDL3_ttf
    )
    foreach(dllTarget IN LISTS SDL_RUNTIME_DLLS)
        add_custom_command(
            TARGET jpsResearch POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:${dllTarget}>"
                "$<TARGET_FILE_DIR:jpsResearch>"
            VERBATIM
        )
    endforeach()

    add_custom_command(TARGET jpsResearch POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${CMAKE_SOURCE_DIR}/resources"
            "$<TARGET_FILE_DIR:jpsResearch>/resources"
    )
endif()
