#pragma once
#include "Shape.h"
#include <string>

/*
 * ============================================================
 *  Room.h
 * ------------------------------------------------------------
 *  OOP: INHERITANCE + POLYMORPHISM (the clearest example in the
 *  whole project). Room is the only shape whose getArea() is
 *  non-zero; FloorPlan::getTotalArea() never checks "is this a
 *  Room?" - it just sums shape->getArea() over everything, and
 *  Wall/Door/Window/CurvedWall contribute 0 automatically.
 *
 *  SIMPLIFICATION: rooms are always RECTANGLES. For a rectangle,
 *  the top/bottom sides are always equal and the left/right
 *  sides are always equal - so "length of every side" is fully
 *  represented by just two numbers (width, height), which is
 *  what is stored AND what is shown (all 4 side names, mam's
 *  requested dimension lines included - see Room.cpp).
 * ============================================================
 */
class Room : public Shape
{
private:
    std::string roomType;    // "Bedroom", "Kitchen", "Living Room", "Bathroom"
    float widthMeters;
    float heightMeters;

public:
    static constexpr int HANDLE_TL     = 0;
    static constexpr int HANDLE_TR     = 1;
    static constexpr int HANDLE_BR     = 2;
    static constexpr int HANDLE_BL     = 3;
    static constexpr int HANDLE_ROTATE = 4;

    Room(float x, float y, float wMeters, float hMeters, std::string type);

    void draw(sf::RenderWindow& window) override;
    bool contains(sf::Vector2f point) const override;
    void move(float dx, float dy) override;
    double getArea() const override;      // width * height
    std::string getInfo() const override;

    std::string getType() const { return roomType; }
    float getWidthMeters() const { return widthMeters; }
    float getHeightMeters() const { return heightMeters; }

    // COMPUTER GRAPHICS: GEOMETRY & ROTATION HELPERS
    // Room is rotated around its own centre C by rotationDegrees.
    // Corners are transformed from local model space to screen space.
    sf::Vector2f getCenter() const;
    sf::Vector2f getCornerWorld(int index) const;
    sf::Vector2f getTopCenterWorld() const;
    sf::Vector2f getRotateHandleWorld() const;

    // OOP: POLYMORPHIC HANDLE INTERFACE FOR RESIZE & ROTATE
    // - 4 corner resize handles: dragging one corner resizes the room,
    //   keeping the opposite corner fixed in world space (min 1 grid cell).
    // - 1 rotate handle: small circle above top-centre with connecting stem,
    //   rotating the room around its centre in 15-degree snapped increments.
    int  getHandleAt(sf::Vector2f point) const override;
    void dragHandle(int handleIndex, sf::Vector2f newPos) override;
    void drawHandles(sf::RenderWindow& window) const override;
};
