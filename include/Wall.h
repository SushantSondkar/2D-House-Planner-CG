#pragma once
#include "Shape.h"

/*
 * ============================================================
 *  Wall.h
 * ------------------------------------------------------------
 *  OOP: INHERITANCE - Wall IS-A Shape; reuses x/y/selected.
 *  OOP: ENCAPSULATION - startPoint/endPoint are private; the
 *       length is never stored, always CALCULATED in getLength().
 *  CG: a wall is rendered as a ROTATED, TRANSLATED rectangle
 *      (the classic 2D transform pair) so it has a realistic
 *      wall THICKNESS, not just a 1-pixel line.
 * ============================================================
 */
class Wall : public Shape
{
private:
    sf::Vector2f startPoint;   // screen-pixel coordinates
    sf::Vector2f endPoint;

public:
    static constexpr int HANDLE_START = 0;
    static constexpr int HANDLE_END   = 1;

    Wall(sf::Vector2f start, sf::Vector2f end);

    void draw(sf::RenderWindow& window) override;
    bool contains(sf::Vector2f point) const override;
    void move(float dx, float dy) override;
    double getArea() const override { return 0.0; } // a wall itself has no floor area
    std::string getInfo() const override;

    float getLength() const;
    sf::Vector2f getStart() const { return startPoint; }
    sf::Vector2f getEnd()   const { return endPoint; }

    // OOP: POLYMORPHIC HANDLE INTERFACE IMPLEMENTATION
    // Clicking and dragging either endpoint moves ONLY that endpoint,
    // which dynamically resizes its length and re-orientates the wall
    // via atan2(dy, dx) in draw(). Clicking the wall body moves the whole shape.
    int  getHandleAt(sf::Vector2f point) const override;
    void dragHandle(int handleIndex, sf::Vector2f newPos) override;
    void drawHandles(sf::RenderWindow& window) const override;
};
