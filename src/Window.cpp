#include "Window.h"
#include "Grid.h"
#include "Fonts.h"
#include <sstream>

#include <cmath>
#include <algorithm>

Window::Window(float x, float y) : Shape(x, y), width(1.2f) {}

sf::Vector2f Window::getEndWorld() const
{
    float wPx = Grid::metersToPixels(width);
    float rad = rotationDegrees * 3.14159265f / 180.f;
    return sf::Vector2f(
        x + wPx * std::cos(rad),
        y + wPx * std::sin(rad)
    );
}

sf::Vector2f Window::getMidpointWorld() const
{
    float wPx = Grid::metersToPixels(width);
    float rad = rotationDegrees * 3.14159265f / 180.f;
    return sf::Vector2f(
        x + (wPx / 2.f) * std::cos(rad),
        y + (wPx / 2.f) * std::sin(rad) + 2.f
    );
}

sf::Vector2f Window::getRotateHandleWorld() const
{
    float wPx = Grid::metersToPixels(width);
    float rad = rotationDegrees * 3.14159265f / 180.f;
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);

    // In local unrotated frame: stem extends 18 px upward from midpoint
    sf::Vector2f localOffset(wPx / 2.f, -18.f);
    return sf::Vector2f(
        x + localOffset.x * cosA - localOffset.y * sinA,
        y + localOffset.x * sinA + localOffset.y * cosA
    );
}

void Window::draw(sf::RenderWindow& window)
{
    float w = Grid::metersToPixels(width);
    sf::Color c = selected ? sf::Color::Red : sf::Color::Blue;

    // COMPUTER GRAPHICS: AFFINE ROTATION AROUND ANCHOR POINT (x, y)
    sf::RenderStates states;
    states.transform.translate({x, y}).rotate(sf::degrees(rotationDegrees)).translate({-x, -y});

    // Two parallel lines = standard simple "glass" architectural symbol
    sf::Vertex line1[] = {
        sf::Vertex{{x, y},         c},
        sf::Vertex{{x + w, y},     c}
    };
    sf::Vertex line2[] = {
        sf::Vertex{{x, y + 4.f},     c},
        sf::Vertex{{x + w, y + 4.f}, c}
    };
    window.draw(line1, 2, sf::PrimitiveType::Lines, states);
    window.draw(line2, 2, sf::PrimitiveType::Lines, states);

    sf::Text label(Fonts::get(), "Window", 10);
    label.setFillColor(sf::Color::Black);
    label.setPosition({x + 4.f, y + 8.f});
    window.draw(label, states);
}

void Window::drawHandles(sf::RenderWindow& window) const
{
    // 1. Rotate handle above midpoint: thin stem line + circular handle
    sf::Vector2f mid = getMidpointWorld();
    sf::Vector2f rh  = getRotateHandleWorld();
    sf::Vertex stem[] = {
        sf::Vertex{mid, sf::Color(30, 100, 220)},
        sf::Vertex{rh,  sf::Color(30, 100, 220)}
    };
    window.draw(stem, 2, sf::PrimitiveType::Lines);

    sf::CircleShape rotCircle(4.5f);
    rotCircle.setOrigin({4.5f, 4.5f});
    rotCircle.setPosition(rh);
    rotCircle.setFillColor(sf::Color(100, 220, 100)); // green
    rotCircle.setOutlineThickness(1.5f);
    rotCircle.setOutlineColor(sf::Color(30, 100, 220));
    window.draw(rotCircle);

    // 2. Square resize handle at the far end
    sf::RectangleShape endBox({8.f, 8.f});
    endBox.setOrigin({4.f, 4.f});
    endBox.setPosition(getEndWorld());
    endBox.setRotation(sf::degrees(rotationDegrees));
    endBox.setFillColor(sf::Color::White);
    endBox.setOutlineThickness(1.5f);
    endBox.setOutlineColor(sf::Color(30, 100, 220));
    window.draw(endBox);
}

int Window::getHandleAt(sf::Vector2f point) const
{
    // Check rotate handle
    const float rotRadius = 8.f;
    sf::Vector2f dr = point - getRotateHandleWorld();
    if (dr.x * dr.x + dr.y * dr.y <= rotRadius * rotRadius)
        return HANDLE_ROTATE;

    // Check far-end resize handle
    const float resizeRadius = 8.f;
    sf::Vector2f de = point - getEndWorld();
    if (de.x * de.x + de.y * de.y <= resizeRadius * resizeRadius)
        return HANDLE_RESIZE;

    return HANDLE_NONE;
}

void Window::onHandlePressed(int handleIndex, sf::Vector2f mousePos)
{
    // "add a rotate handle that toggles between horizontal and vertical in 90-degree steps"
    if (handleIndex == HANDLE_ROTATE)
    {
        rotationDegrees = (rotationDegrees == 0.f) ? 90.f : 0.f;
        return;
    }

    if (handleIndex == HANDLE_RESIZE)
    {
        dragHandle(handleIndex, mousePos);
    }
}

void Window::dragHandle(int handleIndex, sf::Vector2f newPos)
{
    // "and a resize handle at one end to drag-adjust width (windows: 0.4-3.0 m)"
    if (handleIndex == HANDLE_RESIZE)
    {
        sf::Vector2f dWorld = newPos - sf::Vector2f(x, y);

        // Map mouse position into local unrotated coordinates along the window axis
        float rad = -rotationDegrees * 3.14159265f / 180.f;
        float dLocalX = dWorld.x * std::cos(rad) - dWorld.y * std::sin(rad);

        float newWidthM = Grid::pixelsToMeters(dLocalX);
        newWidthM = std::clamp(newWidthM, 0.4f, 3.0f);
        // Round cleanly to nearest 0.05 m (5 cm increments)
        width = std::round(newWidthM * 20.f) / 20.f;
    }
}

bool Window::contains(sf::Vector2f point) const
{
    // CG CONCEPT: INVERSE-TRANSFORM HIT TESTING
    sf::Transform inv;
    inv.rotate(sf::degrees(-rotationDegrees), {x, y});
    sf::Vector2f localP = inv.transformPoint(point);

    float w = Grid::metersToPixels(width);
    return localP.x >= x       && localP.x <= x + w &&
           localP.y >= y - 6.f && localP.y <= y + 10.f;
}

void Window::move(float dx, float dy)
{
    x += dx; y += dy;
}

std::string Window::getInfo() const
{
    std::ostringstream ss;
    ss << "Window  |  Width: " << width << " m  |  Orientation: "
       << (rotationDegrees == 0.f ? "Horizontal" : "Vertical");
    return ss.str();
}
