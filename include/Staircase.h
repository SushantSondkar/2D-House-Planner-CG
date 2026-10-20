#pragma once
#include "Shape.h"
#include <string>

/*
 * ============================================================
 *  Staircase.h   (NEW: staircase requirement)
 * ------------------------------------------------------------
 *  OOP: INHERITANCE + POLYMORPHISM
 *    Staircase is another Shape subclass, placed with the same
 *    click-drag rectangle gesture as a Room. It DOES count
 *    toward the total house area (a staircase occupies real
 *    floor space), so getArea() mirrors Room's width*height -
 *    only HOW it draws itself differs (a zig-zag step pattern
 *    and an "UP" arrow instead of a plain rectangle), which is
 *    exactly what polymorphism is for.
 * ============================================================
 */
class Staircase : public Shape
{
private:
    float widthMeters;
    float heightMeters;
    int   numSteps;

public:
    static constexpr int HANDLE_TL     = 0;
    static constexpr int HANDLE_TR     = 1;
    static constexpr int HANDLE_BR     = 2;
    static constexpr int HANDLE_BL     = 3;
    static constexpr int HANDLE_ROTATE = 4;

    Staircase(float x, float y, float wMeters, float hMeters);

    void draw(sf::RenderWindow& window) override;
    bool contains(sf::Vector2f point) const override;
    void move(float dx, float dy) override;
    double getArea() const override;
    std::string getInfo() const override;

    float getWidthMeters() const { return widthMeters; }
    float getHeightMeters() const { return heightMeters; }

    // COMPUTER GRAPHICS: GEOMETRY & ROTATION HELPERS
    // Rotated around its own centre C by rotationDegrees.
    sf::Vector2f getCenter() const;
    sf::Vector2f getCornerWorld(int index) const;
    sf::Vector2f getTopCenterWorld() const;
    sf::Vector2f getRotateHandleWorld() const;

    // OOP: POLYMORPHIC HANDLE INTERFACE FOR RESIZE & ROTATE
    // - 4 corner resize handles: dragging one corner resizes the staircase,
    //   keeping the opposite corner fixed in world space (min 1 grid cell),
    //   and dynamically updates step count (numSteps).
    // - 1 rotate handle: rotates around center in 15-degree snapped increments.
    int  getHandleAt(sf::Vector2f point) const override;
    void dragHandle(int handleIndex, sf::Vector2f newPos) override;
    void drawHandles(sf::RenderWindow& window) const override;
};
