# ============================================================
#  CMakeLists.txt
#  Build script for the 2D House Floor Plan Designer.
#  Requires SFML 3.x (Graphics, Window, System components).
# ============================================================
cmake_minimum_required(VERSION 3.16)
project(HouseFloorPlanner)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(HouseFloorPlanner
    src/main.cpp
    src/Wall.cpp
    src/CurvedWall.cpp
    src/Door.cpp
    src/Window.cpp
    src/Room.cpp
    src/Staircase.cpp
    src/FloorPlan.cpp
    src/Drafting.cpp
    src/Export.cpp
)

target_include_directories(HouseFloorPlanner PRIVATE include)
target_link_libraries(HouseFloorPlanner PRIVATE SFML::Graphics SFML::Window SFML::System)
