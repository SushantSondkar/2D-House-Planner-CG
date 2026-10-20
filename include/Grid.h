#pragma once
#include <cmath>

/*
 * ============================================================
 *  Grid.h
 * ------------------------------------------------------------
 *  COMPUTER GRAPHICS CONCEPT: COORDINATE SYSTEMS + SCALING
 *
 *  Two coordinate systems exist in this project:
 *    1) SCREEN / PIXEL coordinates  -> what SFML actually draws
 *    2) WORLD / METRE coordinates   -> what the HUMAN cares
 *       about ("this wall is 5 metres long")
 *
 *  PIXELS_PER_METER is the SCALE FACTOR that converts between
 *  the two - the same idea as "world-to-screen" / "screen-to-
 *  world" transforms used in every 2D/3D graphics engine.
 *
 *      screen_pixels = world_metres * PIXELS_PER_METER
 *      world_metres  = screen_pixels / PIXELS_PER_METER
 *
 *  GRID SNAPPING rounds a pixel coordinate to the nearest
 *  multiple of PIXELS_PER_METER, so objects line up with the
 *  visible grid, like "snap to grid" in real CAD tools.
 * ============================================================
 */
namespace Grid
{
    // 1 grid cell = 1 metre = 40 pixels on screen.
    constexpr float PIXELS_PER_METER = 40.f;

    // Toolbar is now TWO rows of buttons -> reserve more height.
    constexpr float CANVAS_TOP = 108.f;

    // Standard real-world wall thickness (a 150 mm / 0.15 m brick
    // wall is a realistic, commonly used value on civil drawings).
    constexpr float WALL_THICKNESS_METERS = 0.15f;

    inline float snap(float pixelValue)
    {
        return std::round(pixelValue / PIXELS_PER_METER) * PIXELS_PER_METER;
    }

    inline float metersToPixels(float meters) { return meters * PIXELS_PER_METER; }
    inline float pixelsToMeters(float pixels)  { return pixels / PIXELS_PER_METER; }
}
