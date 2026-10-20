#include "FloorPlan.h"
#include <algorithm>

void FloorPlan::addShape(std::unique_ptr<Shape> shape)
{
    shapes.push_back(std::move(shape));
}

void FloorPlan::removeShape(Shape* shape)
{
    shapes.erase(
        std::remove_if(shapes.begin(), shapes.end(),
            [shape](const std::unique_ptr<Shape>& s) { return s.get() == shape; }),
        shapes.end());
}

void FloorPlan::clear()
{
    shapes.clear();   // every unique_ptr is destroyed -> every Shape is freed
}

void FloorPlan::undoLast()
{
    if (!shapes.empty())
        shapes.pop_back();   // unique_ptr automatically frees the removed shape
}

void FloorPlan::drawAll(sf::RenderWindow& window)
{
    // POLYMORPHIC CALL: we never ask "are you a Wall? a Room?".
    // We just say s->draw(window) and C++ runs the RIGHT version
    // automatically, based on what the object actually is.
    for (auto& s : shapes)
        s->draw(window);
}

Shape* FloorPlan::findShapeAt(sf::Vector2f point)
{
    for (auto it = shapes.rbegin(); it != shapes.rend(); ++it)
    {
        if ((*it)->contains(point))
            return it->get();
    }
    return nullptr;
}

double FloorPlan::getTotalArea() const
{
    double total = 0.0;
    for (auto& s : shapes)
        total += s->getArea();   // Wall/CurvedWall/Door/Window automatically contribute 0
    return total;
}
