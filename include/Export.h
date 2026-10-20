#pragma once
#include <SFML/Graphics.hpp>
#include "FloorPlan.h"

/*
 * ============================================================
 *  Export.h   (NEW: export option requirement)
 * ------------------------------------------------------------
 *  Produces two files inside an "exports/" folder next to the
 *  executable, each stamped with the current date/time so
 *  repeated exports never overwrite each other:
 *
 *   1) a PNG image - a snapshot of the whole drawing window,
 *      captured with sf::Texture::update(window), the standard
 *      SFML technique for turning what's currently on screen
 *      into a saveable sf::Image.
 *
 *   2) a plain-text report listing every wall/curved wall,
 *      room, staircase, door and window with its measurements
 *      (reusing each Shape's own getInfo() - POLYMORPHISM again:
 *      one loop, every shape type reports itself correctly),
 *      plus the total house area - a ready deliverable you can
 *      attach to your project report or submission.
 * ============================================================
 */
namespace Export
{
    // Returns true if both files were written successfully.
    bool exportAll(sf::RenderWindow& window, const FloorPlan& floorPlan);
}
