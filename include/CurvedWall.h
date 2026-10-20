#pragma once
#include "Shape.h"
#include <vector>

/*
 * ============================================================
 *  CurvedWall.h   (NEW: "curvy structures" requirement)
 * ------------------------------------------------------------
 *  OOP: INHERITANCE + POLYMORPHISM
 *    CurvedWall is ANOTHER Shape subclass, used for bay
 *    windows, rounded porches or curved boundary walls. It
 *    reuses the exact same Shape interface as every other
 *    object, so FloorPlan and main.cpp need ZERO extra code
 *    to support it - that is the whole point of coding to an
 *    abstraction instead of to concrete classes.
 *
 *  CG CONCEPT: PARAMETRIC CURVES (quadratic Bezier)
 *    The wall is defined by 3 points: start, control, end.
 *    A quadratic Bezier curve is evaluated at parameter
 *    t in [0,1] as:
 *        B(t) = (1-t)^2 * start + 2(1-t)t * control + t^2 * end
 *    We sample this formula at many small steps of t and join
 *    the samples with straight LINE SEGMENTS - the standard
 *    way curves are rendered in real-time 2D graphics (there
 *    is no literal "draw a curve" primitive on the GPU).
 * ============================================================
 */
class CurvedWall : public Shape
{
private:
    sf::Vector2f startPoint;
    sf::Vector2f controlPoint;
    sf::Vector2f endPoint;
    static const int SEGMENTS = 24;   // how many straight segments approximate the curve

    sf::Vector2f pointAt(float t) const;              // the Bezier formula itself
    std::vector<sf::Vector2f> sampledPoints() const;   // the curve, sampled into a polyline

public:
    static constexpr int HANDLE_START   = 0;
    static constexpr int HANDLE_CONTROL = 1;
    static constexpr int HANDLE_END     = 2;

    CurvedWall(sf::Vector2f start, sf::Vector2f control, sf::Vector2f end);

    void draw(sf::RenderWindow& window) override;
    bool contains(sf::Vector2f point) const override;
    void move(float dx, float dy) override;
    double getArea() const override { return 0.0; }
    std::string getInfo() const override;

    float getLength() const;   // sum of all the sampled segment lengths
    sf::Vector2f getStart()   const { return startPoint; }
    sf::Vector2f getControl() const { return controlPoint; }
    sf::Vector2f getEnd()     const { return endPoint; }

    // OOP: POLYMORPHIC HANDLE INTERFACE FOR PARAMETRIC BEZIER CURVE
    // startPoint, controlPoint, and endPoint can all be dragged independently.
    // Dragging controlPoint reshapes the curvature/bulge; dragging start or end
    // moves the boundary and resizes the span.
    int  getHandleAt(sf::Vector2f point) const override;
    void dragHandle(int handleIndex, sf::Vector2f newPos) override;
    void drawHandles(sf::RenderWindow& window) const override;
};
