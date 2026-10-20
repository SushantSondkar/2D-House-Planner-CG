#pragma once
#include "Shape.h"

/*
 * ============================================================
 *  Door.h
 * ------------------------------------------------------------
 *  OOP: INHERITANCE + POLYMORPHISM - Door overrides draw() with
 *  its own symbol (a small brown "leaf"), different from every
 *  other Shape, yet usable anywhere a plain Shape* is expected.
 * ============================================================
 */
class Door : public Shape
{
private:
    float width;   // metres (drag-adjustable between 0.6 m and 2.0 m)

public:
    static constexpr int HANDLE_RESIZE = 0;
    static constexpr int HANDLE_ROTATE = 1;

    Door(float x, float y);

    void draw(sf::RenderWindow& window) override;
    bool contains(sf::Vector2f point) const override;
    void move(float dx, float dy) override;
    double getArea() const override { return 0.0; }
    std::string getInfo() const override;

    float getWidth() const { return width; }
    void  setWidth(float w) { width = w; }

    // COMPUTER GRAPHICS: GEOMETRY & ROTATION HELPERS
    // Door rotates around its placement anchor (x, y) in 90-degree steps
    // (0 deg = Horizontal, 90 deg = Vertical).
    sf::Vector2f getAnchor() const { return {x, y}; }
    sf::Vector2f getEndWorld() const;
    sf::Vector2f getMidpointWorld() const;
    sf::Vector2f getRotateHandleWorld() const;

    // OOP: POLYMORPHIC HANDLE INTERFACE FOR RESIZE & ORIENT
    // - Resize handle at one end: drag-adjusts width (0.6 - 2.0 m).
    // - Rotate handle above midpoint: toggles horizontal/vertical in 90-degree steps.
    int  getHandleAt(sf::Vector2f point) const override;
    void onHandlePressed(int handleIndex, sf::Vector2f mousePos) override;
    void dragHandle(int handleIndex, sf::Vector2f newPos) override;
    void drawHandles(sf::RenderWindow& window) const override;
};
