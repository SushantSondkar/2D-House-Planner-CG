#pragma once
#include <SFML/Graphics.hpp>
#include <string>

/*
 * ============================================================
 *  Drafting.h   (NEW: compass, scale bar, title block, legend,
 *                dimension lines)
 * ------------------------------------------------------------
 *  A set of small FREE FUNCTIONS (not a class) that draw the
 *  extra "civil engineer / professional plan" furniture that
 *  a real architectural drawing always carries:
 *    - a NORTH COMPASS, so the plan's orientation is unambiguous
 *    - a printed SCALE BAR, so the drawing's scale survives
 *      being printed or zoomed at any size
 *    - a TITLE BLOCK, the standard project-name/scale/area/
 *      drawn-by information box every civil drawing sheet has
 *    - a LEGEND explaining each symbol used on the plan
 *    - DIMENSION LINES, the small tick-marked lines with a
 *      length label used to mark a wall's/room's measurements
 *
 *  These are kept OUTSIDE the Shape hierarchy on purpose: they
 *  are fixed drawing-SHEET decorations, not house objects the
 *  user places/selects/moves, so they don't belong in the
 *  Shape/FloorPlan "parts of the house" model. Keeping them
 *  as free functions instead of (mis)using inheritance for
 *  them is itself good OOP judgement, worth mentioning in viva.
 * ============================================================
 */
namespace Drafting
{
    // Draws a dimension line between two points that share either the
    // same y (a horizontal dimension, vertical=false) or the same x
    // (a vertical dimension, vertical=true), with small perpendicular
    // tick marks at both ends and a text label at the midpoint -
    // exactly how lengths are annotated on a real architectural plan.
    void drawDimensionLine(sf::RenderWindow& window, sf::Vector2f a, sf::Vector2f b,
                            const std::string& label, bool vertical,
                            const sf::RenderStates& states = sf::RenderStates::Default);

    void drawCompass(sf::RenderWindow& window, sf::Vector2f center, float radius);
    void drawScaleBar(sf::RenderWindow& window, sf::Vector2f position);
    void drawTitleBlock(sf::RenderWindow& window, sf::Vector2f topLeft, double totalAreaM2);
    void drawLegend(sf::RenderWindow& window, sf::Vector2f topLeft);
}
