#include "Wall.h"
#include "Grid.h"
#include "Fonts.h"
#include <cmath>
#include <algorithm>
#include <sstream>

Wall::Wall(sf::Vector2f start, sf::Vector2f end)
    : Shape(start.x, start.y), startPoint(start), endPoint(end)
{
}

void Wall::draw(sf::RenderWindow& window)
{
    sf::Vector2f d = endPoint - startPoint;
    float lengthPx  = std::sqrt(d.x * d.x + d.y * d.y);
    float angleDeg  = std::atan2(d.y, d.x) * 180.f / 3.14159265f;
    float thickness = Grid::metersToPixels(Grid::WALL_THICKNESS_METERS);

    // COMPUTER GRAPHICS: a thick wall is just a plain RECTANGLE that has
    // been ROTATED to match the wall's direction and then TRANSLATED to
    // the wall's start point - the standard rotate+translate transform
    // pair, done here through SFML's built-in Transformable interface.
    sf::RectangleShape body({lengthPx, thickness});
    body.setOrigin({0.f, thickness / 2.f});
    body.setPosition(startPoint);
    body.setRotation(sf::degrees(angleDeg));
    body.setFillColor(selected ? sf::Color(200, 60, 60) : sf::Color(60, 60, 60));
    window.draw(body);

    // Permanent length label at the wall's midpoint - always visible,
    // like a real architectural drawing, not only when selected.
    sf::Vector2f mid = startPoint + d * 0.5f;
    std::ostringstream ss;
    ss << getLength() << " m";
    sf::Text label(Fonts::get(), ss.str(), 11);
    label.setFillColor(sf::Color(20, 20, 20));
    label.setPosition({mid.x - 12.f, mid.y - 16.f});
    window.draw(label);
}

void Wall::drawHandles(sf::RenderWindow& window) const
{
    // COMPUTER GRAPHICS: interactive endpoint handles are drawn ONLY when
    // the shape is selected and the Select tool is active.
    // Circular handles at startPoint and endPoint allow the user to drag
    // either point independently, which alters both length and orientation.
    sf::CircleShape handle(5.f);
    handle.setFillColor(sf::Color(30, 100, 240));
    handle.setOutlineThickness(1.5f);
    handle.setOutlineColor(sf::Color::White);
    handle.setOrigin({5.f, 5.f});

    handle.setPosition(startPoint);
    window.draw(handle);
    handle.setPosition(endPoint);
    window.draw(handle);
}

int Wall::getHandleAt(sf::Vector2f point) const
{
    // Radius of hit-testing circle around each endpoint (in screen pixels)
    const float radius = 8.f;
    const float rSq = radius * radius;

    sf::Vector2f ds = point - startPoint;
    if (ds.x * ds.x + ds.y * ds.y <= rSq) return HANDLE_START;

    sf::Vector2f de = point - endPoint;
    if (de.x * de.x + de.y * de.y <= rSq) return HANDLE_END;

    return HANDLE_NONE;
}

void Wall::dragHandle(int handleIndex, sf::Vector2f newPos)
{
    // OOP & CG CONCEPT: POINT MANIPULATION
    // Moving only one endpoint recalculates the vector d = (endPoint - startPoint).
    // In draw(), length is computed via Euclidean norm std::sqrt(dx^2 + dy^2)
    // and orientation is derived via std::atan2(dy, dx) - resizing and
    // re-orienting simultaneously in a single intuitive gesture!
    if (handleIndex == HANDLE_START)
    {
        startPoint = newPos;
        x = startPoint.x;
        y = startPoint.y;
    }
    else if (handleIndex == HANDLE_END)
    {
        endPoint = newPos;
    }
}

bool Wall::contains(sf::Vector2f point) const
{
    sf::Vector2f AB = endPoint - startPoint;
    sf::Vector2f AP = point - startPoint;
    float lenSq = AB.x * AB.x + AB.y * AB.y;
    if (lenSq == 0.f) return false;

    float t = (AP.x * AB.x + AP.y * AB.y) / lenSq;
    t = std::max(0.f, std::min(1.f, t));

    sf::Vector2f closestPoint = startPoint + AB * t;
    sf::Vector2f diff = point - closestPoint;
    float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    // click tolerance now matches the wall's drawn thickness
    return distance < (Grid::metersToPixels(Grid::WALL_THICKNESS_METERS) / 2.f + 4.f);
}

void Wall::move(float dx, float dy)
{
    startPoint.x += dx; startPoint.y += dy;
    endPoint.x   += dx; endPoint.y   += dy;
    x = startPoint.x;
    y = startPoint.y;
}

float Wall::getLength() const
{
    sf::Vector2f d = endPoint - startPoint;
    float pixelLength = std::sqrt(d.x * d.x + d.y * d.y);
    return Grid::pixelsToMeters(pixelLength);
}

std::string Wall::getInfo() const
{
    std::ostringstream ss;
    ss << "Wall  |  Length: " << getLength() << " m  |  Thickness: "
       << Grid::WALL_THICKNESS_METERS << " m";
    return ss.str();
}
