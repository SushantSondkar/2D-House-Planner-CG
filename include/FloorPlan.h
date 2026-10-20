#pragma once
#include "Shape.h"
#include <vector>
#include <memory>

/*
 * ============================================================
 *  FloorPlan.h
 * ------------------------------------------------------------
 *  OOP: COMPOSITION ("HAS-A") - FloorPlan represents the whole
 *  house. It does NOT inherit from Shape; it simply OWNS a
 *  collection of Shape objects (walls, curved walls, rooms,
 *  staircases, doors, windows).
 *
 *      FloorPlan
 *          HAS-A  vector<unique_ptr<Shape>>
 *                   ├── Wall / CurvedWall
 *                   ├── Door / Window
 *                   └── Room / Staircase
 *
 *  std::unique_ptr<Shape> means FloorPlan fully OWNS every
 *  shape's memory - removing one or clearing the vector
 *  automatically deletes it (RAII, no manual "delete").
 *
 *  OOP: POLYMORPHISM - drawAll() and getTotalArea() loop over
 *  every shape and call shape->draw() / shape->getArea(); the
 *  correct override for each concrete type runs automatically.
 * ============================================================
 */
class FloorPlan
{
private:
    std::vector<std::unique_ptr<Shape>> shapes;

public:
    void addShape(std::unique_ptr<Shape> shape);
    void removeShape(Shape* shape);
    void clear();                                  // "Clear" toolbar button
    void undoLast();                                // "Undo" toolbar button - removes most recent shape

    void drawAll(sf::RenderWindow& window);         // POLYMORPHIC rendering loop

    Shape* findShapeAt(sf::Vector2f point);          // topmost shape under a mouse click
    double getTotalArea() const;                     // sum of every Room's/Staircase's area

    const std::vector<std::unique_ptr<Shape>>& getShapes() const { return shapes; }
};
