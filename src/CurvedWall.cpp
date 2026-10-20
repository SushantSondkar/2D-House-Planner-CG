#include "CurvedWall.h"
#include "Grid.h"
#include "Fonts.h"
#include <cmath>
#include <algorithm>
#include <sstream>

CurvedWall::CurvedWall(sf::Vector2f start, sf::Vector2f control, sf::Vector2f end)
    : Shape(start.x, start.y), startPoint(start), controlPoint(control), endPoint(end)
{
}

sf::Vector2f CurvedWall::pointAt(float t) const
{
    // Quadratic Bezier formula: B(t) = (1-t)^2 P0 + 2(1-t)t P1 + t^2 P2
    float u = 1.f - t;
    float a = u * u;
    float b = 2.f * u * t;
    float c = t * t;
    return sf::Vector2f(
        a * startPoint.x + b * controlPoint.x + c * endPoint.x,
        a * startPoint.y + b * controlPoint.y + c * endPoint.y
    );
}

std::vector<sf::Vector2f> CurvedWall::sampledPoints() const
{
    std::vector<sf::Vector2f> pts;
    pts.reserve(SEGMENTS + 1);
    for (int i = 0; i <= SEGMENTS; ++i)
        pts.push_back(pointAt(static_cast<float>(i) / SEGMENTS));
    return pts;
}

void CurvedWall::draw(sf::RenderWindow& window)
{
    auto pts = sampledPoints();
    sf::Color c = selected ? sf::Color(200, 60, 60) : sf::Color(60, 60, 60);

    // Build a VertexArray-style line strip and draw it in ONE call -
    // this is the standard way to render a sampled curve in 2D graphics.
    std::vector<sf::Vertex> verts;
    verts.reserve(pts.size());
    for (auto& p : pts)
        verts.push_back(sf::Vertex{p, c});
    window.draw(verts.data(), verts.size(), sf::PrimitiveType::LineStrip);

    sf::Vector2f mid = pointAt(0.5f);
    std::ostringstream ss;
    ss << getLength() << " m";
    sf::Text label(Fonts::get(), ss.str(), 11);
    label.setFillColor(sf::Color(20, 20, 20));
    label.setPosition({mid.x - 12.f, mid.y - 16.f});
    window.draw(label);
}

void CurvedWall::drawHandles(sf::RenderWindow& window) const
{
    // COMPUTER GRAPHICS: BEZIER CONTROL CAGE & HANDLES
    // When selected, we draw the Bezier "control polygon" (guide lines connecting
    // startPoint -> controlPoint and controlPoint -> endPoint) and three distinct
    // circular handles so the user can interactively manipulate the curve.
    sf::Vertex cage[] = {
        sf::Vertex{startPoint,   sf::Color(140, 140, 200, 180)},
        sf::Vertex{controlPoint, sf::Color(140, 140, 200, 180)},
        sf::Vertex{controlPoint, sf::Color(140, 140, 200, 180)},
        sf::Vertex{endPoint,     sf::Color(140, 140, 200, 180)}
    };
    window.draw(cage, 4, sf::PrimitiveType::Lines);

    // Control-point handle (distinct light-purple fill to signify curvature control)
    sf::CircleShape ctrlHandle(5.f);
    ctrlHandle.setFillColor(sf::Color(140, 140, 255));
    ctrlHandle.setOutlineThickness(1.5f);
    ctrlHandle.setOutlineColor(sf::Color::White);
    ctrlHandle.setOrigin({5.f, 5.f});
    ctrlHandle.setPosition(controlPoint);
    window.draw(ctrlHandle);

    // Start and End endpoint handles (primary blue)
    sf::CircleShape endHandle(5.f);
    endHandle.setFillColor(sf::Color(30, 100, 240));
    endHandle.setOutlineThickness(1.5f);
    endHandle.setOutlineColor(sf::Color::White);
    endHandle.setOrigin({5.f, 5.f});

    endHandle.setPosition(startPoint);
    window.draw(endHandle);
    endHandle.setPosition(endPoint);
    window.draw(endHandle);
}

int CurvedWall::getHandleAt(sf::Vector2f point) const
{
    const float radius = 8.f;
    const float rSq = radius * radius;

    auto hit = [&](sf::Vector2f target) {
        sf::Vector2f d = point - target;
        return (d.x * d.x + d.y * d.y) <= rSq;
    };

    // Check control point first (in case it lies near an endpoint)
    if (hit(controlPoint)) return HANDLE_CONTROL;
    if (hit(startPoint))   return HANDLE_START;
    if (hit(endPoint))     return HANDLE_END;

    return HANDLE_NONE;
}

void CurvedWall::dragHandle(int handleIndex, sf::Vector2f newPos)
{
    // CG CONCEPT: PARAMETRIC CURVE EDITING
    // - Dragging controlPoint reshapes the curvature without moving endpoints.
    // - Dragging startPoint or endPoint re-positions the curve's anchors.
    if (handleIndex == HANDLE_START)
    {
        startPoint = newPos;
        x = startPoint.x;
        y = startPoint.y;
    }
    else if (handleIndex == HANDLE_CONTROL)
    {
        controlPoint = newPos;
    }
    else if (handleIndex == HANDLE_END)
    {
        endPoint = newPos;
    }
}

bool CurvedWall::contains(sf::Vector2f point) const
{
    // Hit-test each straight segment of the sampled polyline and
    // keep the closest distance - the curve is "clicked" if the
    // point is near ANY segment of it.
    auto pts = sampledPoints();
    for (std::size_t i = 0; i + 1 < pts.size(); ++i)
    {
        sf::Vector2f A = pts[i], B = pts[i + 1];
        sf::Vector2f AB = B - A, AP = point - A;
        float lenSq = AB.x * AB.x + AB.y * AB.y;
        if (lenSq == 0.f) continue;
        float t = std::max(0.f, std::min(1.f, (AP.x * AB.x + AP.y * AB.y) / lenSq));
        sf::Vector2f closest = A + AB * t;
        sf::Vector2f diff = point - closest;
        if (std::sqrt(diff.x * diff.x + diff.y * diff.y) < 6.f)
            return true;
    }
    return false;
}

void CurvedWall::move(float dx, float dy)
{
    startPoint.x   += dx; startPoint.y   += dy;
    controlPoint.x += dx; controlPoint.y += dy;
    endPoint.x     += dx; endPoint.y     += dy;
    x = startPoint.x;
    y = startPoint.y;
}

float CurvedWall::getLength() const
{
    auto pts = sampledPoints();
    float total = 0.f;
    for (std::size_t i = 0; i + 1 < pts.size(); ++i)
    {
        sf::Vector2f d = pts[i + 1] - pts[i];
        total += std::sqrt(d.x * d.x + d.y * d.y);
    }
    return Grid::pixelsToMeters(total);
}

std::string CurvedWall::getInfo() const
{
    std::ostringstream ss;
    ss << "Curved Wall  |  Length: " << getLength() << " m";
    return ss.str();
}
