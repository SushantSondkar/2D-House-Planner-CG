#include "Staircase.h"
#include "Grid.h"
#include "Fonts.h"
#include <algorithm>
#include <sstream>
#include <vector>

#include <cmath>

Staircase::Staircase(float x, float y, float wMeters, float hMeters)
    : Shape(x, y), widthMeters(wMeters), heightMeters(hMeters)
{
    // One step roughly every 0.25 m of the LONGER side (a realistic
    // stair-tread depth), capped so the drawing never gets too busy.
    float longerSide = std::max(wMeters, hMeters);
    numSteps = std::max(3, std::min(16, static_cast<int>(longerSide / 0.25f)));
}

sf::Vector2f Staircase::getCenter() const
{
    float wPx = Grid::metersToPixels(widthMeters);
    float hPx = Grid::metersToPixels(heightMeters);
    return sf::Vector2f(x + wPx / 2.f, y + hPx / 2.f);
}

sf::Vector2f Staircase::getCornerWorld(int index) const
{
    // COMPUTER GRAPHICS: AFFINE ROTATION AROUND OBJECT CENTRE
    float wPx = Grid::metersToPixels(widthMeters);
    float hPx = Grid::metersToPixels(heightMeters);
    sf::Vector2f c = getCenter();

    sf::Vector2f localOffset;
    switch (index)
    {
    case HANDLE_TL: localOffset = {-wPx / 2.f, -hPx / 2.f}; break;
    case HANDLE_TR: localOffset = { wPx / 2.f, -hPx / 2.f}; break;
    case HANDLE_BR: localOffset = { wPx / 2.f,  hPx / 2.f}; break;
    case HANDLE_BL: localOffset = {-wPx / 2.f,  hPx / 2.f}; break;
    default:        localOffset = {0.f, 0.f}; break;
    }

    float rad = rotationDegrees * 3.14159265f / 180.f;
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);

    return sf::Vector2f(
        c.x + localOffset.x * cosA - localOffset.y * sinA,
        c.y + localOffset.x * sinA + localOffset.y * cosA
    );
}

sf::Vector2f Staircase::getTopCenterWorld() const
{
    float hPx = Grid::metersToPixels(heightMeters);
    sf::Vector2f c = getCenter();
    sf::Vector2f localOffset(0.f, -hPx / 2.f);

    float rad = rotationDegrees * 3.14159265f / 180.f;
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);

    return sf::Vector2f(
        c.x + localOffset.x * cosA - localOffset.y * sinA,
        c.y + localOffset.x * sinA + localOffset.y * cosA
    );
}

sf::Vector2f Staircase::getRotateHandleWorld() const
{
    float hPx = Grid::metersToPixels(heightMeters);
    sf::Vector2f c = getCenter();
    const float stemLength = 24.f;
    sf::Vector2f localOffset(0.f, -hPx / 2.f - stemLength);

    float rad = rotationDegrees * 3.14159265f / 180.f;
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);

    return sf::Vector2f(
        c.x + localOffset.x * cosA - localOffset.y * sinA,
        c.y + localOffset.x * sinA + localOffset.y * cosA
    );
}

void Staircase::draw(sf::RenderWindow& window)
{
    float wPx = Grid::metersToPixels(widthMeters);
    float hPx = Grid::metersToPixels(heightMeters);
    bool runsHorizontally = widthMeters >= heightMeters;
    sf::Vector2f c = getCenter();

    // COMPUTER GRAPHICS: ROTATION AROUND OBJECT CENTRE
    sf::RectangleShape outline({wPx, hPx});
    outline.setOrigin({wPx / 2.f, hPx / 2.f});
    outline.setPosition(c);
    outline.setRotation(sf::degrees(rotationDegrees));
    outline.setFillColor(sf::Color(210, 210, 235, 150));
    outline.setOutlineThickness(2.f);
    outline.setOutlineColor(selected ? sf::Color::Red : sf::Color::Black);
    window.draw(outline);

    // Apply the rotation transform to steps, UP arrow, and labels
    sf::RenderStates states;
    states.transform.translate(c).rotate(sf::degrees(rotationDegrees)).translate(-c);

    // Draw the individual steps as parallel lines across the flight's walking direction
    sf::Color stepColor(90, 90, 130);
    std::vector<sf::Vertex> steps;
    steps.reserve(static_cast<std::size_t>(numSteps) * 2);
    for (int i = 1; i < numSteps; ++i)
    {
        float t = static_cast<float>(i) / numSteps;
        if (runsHorizontally)
        {
            float sx = x + t * wPx;
            steps.push_back(sf::Vertex{{sx, y},       stepColor});
            steps.push_back(sf::Vertex{{sx, y + hPx}, stepColor});
        }
        else
        {
            float sy = y + t * hPx;
            steps.push_back(sf::Vertex{{x, sy},       stepColor});
            steps.push_back(sf::Vertex{{x + wPx, sy}, stepColor});
        }
    }
    if (!steps.empty())
        window.draw(steps.data(), steps.size(), sf::PrimitiveType::Lines, states);

    // "UP" arrow showing the ascending direction
    sf::ConvexShape arrow(3);
    if (runsHorizontally)
    {
        arrow.setPoint(0, {x + wPx - 14.f, y + hPx / 2.f - 6.f});
        arrow.setPoint(1, {x + wPx - 14.f, y + hPx / 2.f + 6.f});
        arrow.setPoint(2, {x + wPx - 4.f,  y + hPx / 2.f});
    }
    else
    {
        arrow.setPoint(0, {x + wPx / 2.f - 6.f, y + 14.f});
        arrow.setPoint(1, {x + wPx / 2.f + 6.f, y + 14.f});
        arrow.setPoint(2, {x + wPx / 2.f,       y + 4.f});
    }
    arrow.setFillColor(sf::Color::Black);
    window.draw(arrow, states);

    std::ostringstream ss;
    ss << "Staircase\n" << widthMeters << "m x " << heightMeters << "m\nSteps: " << numSteps;
    sf::Text label(Fonts::get(), ss.str(), 11);
    label.setFillColor(sf::Color::Black);
    label.setPosition({x + 4.f, y + 4.f});
    window.draw(label, states);
}

void Staircase::drawHandles(sf::RenderWindow& window) const
{
    // 1. Rotate handle: thin stem from top-centre to rotate handle circle
    sf::Vector2f tc = getTopCenterWorld();
    sf::Vector2f rh = getRotateHandleWorld();
    sf::Vertex stem[] = {
        sf::Vertex{tc, sf::Color(30, 100, 220)},
        sf::Vertex{rh, sf::Color(30, 100, 220)}
    };
    window.draw(stem, 2, sf::PrimitiveType::Lines);

    sf::CircleShape rotCircle(4.5f);
    rotCircle.setOrigin({4.5f, 4.5f});
    rotCircle.setPosition(rh);
    rotCircle.setFillColor(sf::Color(100, 220, 100));
    rotCircle.setOutlineThickness(1.5f);
    rotCircle.setOutlineColor(sf::Color(30, 100, 220));
    window.draw(rotCircle);

    // 2. Square resize handles at the 4 corners
    for (int i = 0; i < 4; ++i)
    {
        sf::RectangleShape box({8.f, 8.f});
        box.setOrigin({4.f, 4.f});
        box.setPosition(getCornerWorld(i));
        box.setRotation(sf::degrees(rotationDegrees));
        box.setFillColor(sf::Color::White);
        box.setOutlineThickness(1.5f);
        box.setOutlineColor(sf::Color(30, 100, 220));
        window.draw(box);
    }
}

int Staircase::getHandleAt(sf::Vector2f point) const
{
    const float rotRadius = 8.f;
    sf::Vector2f dr = point - getRotateHandleWorld();
    if (dr.x * dr.x + dr.y * dr.y <= rotRadius * rotRadius)
        return HANDLE_ROTATE;

    const float cornerRadius = 7.f;
    for (int i = 0; i < 4; ++i)
    {
        sf::Vector2f dc = point - getCornerWorld(i);
        if (dc.x * dc.x + dc.y * dc.y <= cornerRadius * cornerRadius)
            return i;
    }

    return HANDLE_NONE;
}

void Staircase::dragHandle(int handleIndex, sf::Vector2f newPos)
{
    // CASE A: ROTATION HANDLE
    if (handleIndex == HANDLE_ROTATE)
    {
        sf::Vector2f c = getCenter();
        sf::Vector2f v = newPos - c;
        float angleRad = std::atan2(v.y, v.x);
        float angleDeg = angleRad * 180.f / 3.14159265f;

        float currentAngle = angleDeg + 90.f;
        float snapped = std::round(currentAngle / 15.f) * 15.f;
        while (snapped < 0.f)   snapped += 360.f;
        while (snapped >= 360.f) snapped -= 360.f;
        rotationDegrees = snapped;
        return;
    }

    // CASE B: CORNER RESIZE HANDLE
    if (handleIndex >= HANDLE_TL && handleIndex <= HANDLE_BL)
    {
        int oppositeIndex = (handleIndex + 2) % 4;
        sf::Vector2f oppWorld = getCornerWorld(oppositeIndex);

        sf::Vector2f dWorld = newPos - oppWorld;

        float rad = -rotationDegrees * 3.14159265f / 180.f;
        float cosA = std::cos(rad);
        float sinA = std::sin(rad);
        sf::Vector2f dLocal(
            dWorld.x * cosA - dWorld.y * sinA,
            dWorld.x * sinA + dWorld.y * cosA
        );

        float wPxNew = 0.f;
        float hPxNew = 0.f;
        switch (handleIndex)
        {
        case HANDLE_TL:
            wPxNew = -dLocal.x;
            hPxNew = -dLocal.y;
            break;
        case HANDLE_TR:
            wPxNew = dLocal.x;
            hPxNew = -dLocal.y;
            break;
        case HANDLE_BR:
            wPxNew = dLocal.x;
            hPxNew = dLocal.y;
            break;
        case HANDLE_BL:
            wPxNew = -dLocal.x;
            hPxNew = dLocal.y;
            break;
        }

        // Minimum size of 1 grid cell (1 metre = 40 pixels)
        float minPx = Grid::PIXELS_PER_METER;
        wPxNew = std::max(minPx, wPxNew);
        hPxNew = std::max(minPx, hPxNew);

        // Recompute widthMeters and heightMeters cleanly (rounded to 0.1 m increments)
        widthMeters  = std::max(1.0f, std::round(Grid::pixelsToMeters(wPxNew) * 10.f) / 10.f);
        heightMeters = std::max(1.0f, std::round(Grid::pixelsToMeters(hPxNew) * 10.f) / 10.f);
        wPxNew = Grid::metersToPixels(widthMeters);
        hPxNew = Grid::metersToPixels(heightMeters);

        // Dynamic update of staircase step treads based on new dimensions
        float longerSide = std::max(widthMeters, heightMeters);
        numSteps = std::max(3, std::min(16, static_cast<int>(longerSide / 0.25f)));

        sf::Vector2f centerOffsetLocal;
        switch (handleIndex)
        {
        case HANDLE_TL: centerOffsetLocal = {-wPxNew / 2.f, -hPxNew / 2.f}; break;
        case HANDLE_TR: centerOffsetLocal = { wPxNew / 2.f, -hPxNew / 2.f}; break;
        case HANDLE_BR: centerOffsetLocal = { wPxNew / 2.f,  hPxNew / 2.f}; break;
        case HANDLE_BL: centerOffsetLocal = {-wPxNew / 2.f,  hPxNew / 2.f}; break;
        }

        float fwdRad = rotationDegrees * 3.14159265f / 180.f;
        float fwdCos = std::cos(fwdRad);
        float fwdSin = std::sin(fwdRad);
        sf::Vector2f newCenterWorld(
            oppWorld.x + centerOffsetLocal.x * fwdCos - centerOffsetLocal.y * fwdSin,
            oppWorld.y + centerOffsetLocal.x * fwdSin + centerOffsetLocal.y * fwdCos
        );

        x = newCenterWorld.x - wPxNew / 2.f;
        y = newCenterWorld.y - hPxNew / 2.f;
    }
}

bool Staircase::contains(sf::Vector2f point) const
{
    // COMPUTER GRAPHICS: INVERSE-ROTATE-THEN-TEST
    sf::Transform inv;
    inv.rotate(sf::degrees(-rotationDegrees), getCenter());
    sf::Vector2f localP = inv.transformPoint(point);

    float wPx = Grid::metersToPixels(widthMeters);
    float hPx = Grid::metersToPixels(heightMeters);
    return localP.x >= x && localP.x <= x + wPx &&
           localP.y >= y && localP.y <= y + hPx;
}

void Staircase::move(float dx, float dy)
{
    x += dx; y += dy;
}

double Staircase::getArea() const
{
    return static_cast<double>(widthMeters) * static_cast<double>(heightMeters);
}

std::string Staircase::getInfo() const
{
    std::ostringstream ss;
    ss << "Staircase  |  " << widthMeters << "m x " << heightMeters
       << "m  |  Steps: " << numSteps << "  |  Area: " << getArea() << " m2";
    if (std::abs(rotationDegrees) > 0.01f)
    {
        ss << "  |  Rot: " << static_cast<int>(std::round(rotationDegrees)) << " deg";
    }
    return ss.str();
}
