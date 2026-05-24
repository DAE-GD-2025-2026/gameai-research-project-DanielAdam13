cmake_minimum_required(VERSION 3.26)
project(JPSResearch
        VERSION 0.1.0
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#----------------------------------------------------------------------
# Pull in the engine.
#----------------------------------------------------------------------
add_subdirectory(GameEngine)

#----------------------------------------------------------------------
# Game executable
#----------------------------------------------------------------------
add_subdirectory(JPSResearch)
