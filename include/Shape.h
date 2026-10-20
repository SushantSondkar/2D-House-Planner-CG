#pragma once
#include <SFML/Graphics.hpp>
#include <string>

/*
 * ============================================================
 *  Shape.h  -  THE ABSTRACT BASE CLASS OF THE WHOLE PROJECT
 * ------------------------------------------------------------
 *  OOP CONCEPT: ABSTRACTION
 *    Shape declares WHAT every object on the floor plan must
 *    do (draw, move, report area, be selected...) WITHOUT
 *    saying HOW. The "how" belongs to each concrete subclass.
 *
 *  OOP CONCEPT: INHERITANCE (base of the hierarchy)
 *        Shape
 *        ├── Wall
 *        ├── CurvedWall
 *        ├── Door
 *        ├── Window
 *        ├── Room
 *        └── Staircase
 *
 *  OOP CONCEPT: POLYMORPHISM (enabled by "virtual")
 *    draw(), contains(), move(), getArea(), getInfo() are pure
 *    virtual ("=0"). You can never create a plain Shape object
 *    - only its concrete children - but you CAN store ALL of
 *    them together as Shape* / unique_ptr<Shape> and call
 *    shape->draw() on each; the CORRECT version runs
 *    automatically at runtime.
 *
 *  OOP CONCEPT: ENCAPSULATION
 *    x, y, selected are "protected", reachable only through
 *    the public getters/setters below.
 * ============================================================
 */
class Shape
{
protected:
    float x, y;                // position in SCREEN PIXELS (top-left reference point)
    bool  selected;             // is the user currently pointing at / holding this shape?
    float rotationDegrees;     // CG TRANSFORM: stored rotation angle around shape's anchor/centre in degrees

public:
    // Sentinel handle ID indicating no handle was clicked / selected
    static constexpr int HANDLE_NONE = -1;

    Shape(float x_, float y_)
        : x(x_), y(y_), selected(false), rotationDegrees(0.f) {}

    // virtual destructor - required because shapes are deleted
    // through base-class pointers (std::unique_ptr<Shape>).
    virtual ~Shape() = default;

    virtual void draw(sf::RenderWindow& window)        = 0; // POLYMORPHISM
    virtual bool contains(sf::Vector2f point) const    = 0; // mouse hit-testing
    virtual void move(float dx, float dy)              = 0; // TRANSLATION (CG transform)
    virtual double getArea() const                     = 0; // only Room/Staircase return > 0
    virtual std::string getInfo() const                 = 0; // text shown in the status bar

    virtual void setSelected(bool s) { selected = s; }
    virtual bool isSelected() const  { return selected; }

    float getX() const { return x; }
    float getY() const { return y; }

    float getRotation() const { return rotationDegrees; }
    virtual void setRotation(float deg) { rotationDegrees = deg; }

    /*
     * --------------------------------------------------------
     *  OOP CONCEPT: INTERFACE SEGREGATION & POLYMORPHIC HANDLES
     * --------------------------------------------------------
     *  To allow interactive RESIZING and RE-ORIENTING of shapes
     *  without breaking encapsulation or forcing main.cpp to
     *  perform unsafe downcasting (avoiding dynamic_cast / RTTI),
     *  Shape declares polymorphic handle operations:
     *
     *    1) getHandleAt(point): hit-tests the mouse click against
     *       shape-specific handles (corners, endpoints, rotate nodes).
     *       Returns a non-negative handle ID or HANDLE_NONE.
     *    2) onHandlePressed(handle, pos): hook invoked immediately
     *       when a handle is pressed (e.g. for toggle actions).
     *    3) dragHandle(handle, newPos): updates the shape's geometry
     *       as the mouse is dragged.
     *    4) drawHandles(window): renders the shape's handles when
     *       it is selected and the Select tool is active.
     * --------------------------------------------------------
     */
    virtual int getHandleAt(sf::Vector2f point) const { return HANDLE_NONE; }
    virtual void onHandlePressed(int handleIndex, sf::Vector2f mousePos) { dragHandle(handleIndex, mousePos); }
    virtual void dragHandle(int handleIndex, sf::Vector2f newPos) {}
    virtual void drawHandles(sf::RenderWindow& window) const {}
};
