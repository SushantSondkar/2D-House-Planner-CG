#include "Room.h"
#include "Grid.h"
#include "Fonts.h"
#include "Drafting.h"
#include <sstream>

#include <cmath>

Room::Room(float x, float y, float wMeters, float hMeters, std::string type)
    : Shape(x, y), roomType(std::move(type)), widthMeters(wMeters), heightMeters(hMeters)
{
}

sf::Vector2f Room::getCenter() const
{
    float wPx = Grid::metersToPixels(widthMeters);
    float hPx = Grid::metersToPixels(heightMeters);
    return sf::Vector2f(x + wPx / 2.f, y + hPx / 2.f);
}

sf::Vector2f Room::getCornerWorld(int index) const
{
    // COMPUTER GRAPHICS: AFFINE ROTATION AROUND ARBITRARY POINT (CENTRE)
    // Formula for rotating local point (lx, ly) by angle theta:
    //   x' = c.x + lx * cos(theta) - ly * sin(theta)
    //   y' = c.y + lx * sin(theta) + ly * cos(theta)
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

sf::Vector2f Room::getTopCenterWorld() const
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

sf::Vector2f Room::getRotateHandleWorld() const
{
    float hPx = Grid::metersToPixels(heightMeters);
    sf::Vector2f c = getCenter();
    const float stemLength = 24.f; // distance above top edge in pixels
    sf::Vector2f localOffset(0.f, -hPx / 2.f - stemLength);

    float rad = rotationDegrees * 3.14159265f / 180.f;
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);

    return sf::Vector2f(
        c.x + localOffset.x * cosA - localOffset.y * sinA,
        c.y + localOffset.x * sinA + localOffset.y * cosA
    );
}

void Room::draw(sf::RenderWindow& window)
{
    float wPx = Grid::metersToPixels(widthMeters);
    float hPx = Grid::metersToPixels(heightMeters);
    sf::Vector2f c = getCenter();

    // COMPUTER GRAPHICS: ROTATION AROUND OBJECT CENTRE
    // setOrigin({wPx / 2.f, hPx / 2.f}) places the transformation anchor at the
    // centre of the rectangle, and setPosition(c) places that anchor at the centre
    // in world space. setRotation(sf::degrees(rotationDegrees)) rotates around this origin.
    sf::RectangleShape rect({wPx, hPx});
    rect.setOrigin({wPx / 2.f, hPx / 2.f});
    rect.setPosition(c);
    rect.setRotation(sf::degrees(rotationDegrees));
    rect.setFillColor(sf::Color(230, 240, 255, 120));
    rect.setOutlineThickness(2.f);
    rect.setOutlineColor(selected ? sf::Color::Red : sf::Color::Black);
    window.draw(rect);

    // Apply the exact same coordinate transformation to all child elements
    // (room name/area label and dimension lines) so the whole room rotates together.
    sf::RenderStates states;
    states.transform.translate(c).rotate(sf::degrees(rotationDegrees)).translate(-c);

    std::ostringstream ss;
    ss << roomType << "\n"
       << widthMeters << "m x " << heightMeters << "m\n"
       << "Area = " << getArea() << " m2";
    sf::Text label(Fonts::get(), ss.str(), 12);
    label.setFillColor(sf::Color::Black);
    label.setPosition({x + 4.f, y + 4.f});
    window.draw(label, states);

    // Architectural dimension lines along top and left, rendered via the rotated states
    std::ostringstream wStr, hStr;
    wStr << widthMeters << " m";
    hStr << heightMeters << " m";
    Drafting::drawDimensionLine(window, {x, y - 12.f}, {x + wPx, y - 12.f}, wStr.str(), false, states);
    Drafting::drawDimensionLine(window, {x - 12.f, y}, {x - 12.f, y + hPx}, hStr.str(), true, states);
}

void Room::drawHandles(sf::RenderWindow& window) const
{
    // 1. Rotate handle: thin stem from top-centre to rotate handle circle
    sf::Vector2f tc = getTopCenterWorld();
    sf::Vector2f rh = getRotateHandleWorld();
    sf::Vertex stem[] = {
        sf::Vertex{tc, sf::Color(30, 100, 220)},
        sf::Vertex{rh, sf::Color(30, 100, 220)}
    };
    window.draw(stem, 2, sf::PrimitiveType::Lines);

    // Circular rotate node (green highlight)
    sf::CircleShape rotCircle(4.5f);
    rotCircle.setOrigin({4.5f, 4.5f});
    rotCircle.setPosition(rh);
    rotCircle.setFillColor(sf::Color(100, 220, 100));
    rotCircle.setOutlineThickness(1.5f);
    rotCircle.setOutlineColor(sf::Color(30, 100, 220));
    window.draw(rotCircle);

    // 2. Square resize handles at all 4 corners
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

int Room::getHandleAt(sf::Vector2f point) const
{
    // Check rotate handle first
    const float rotRadius = 8.f;
    sf::Vector2f dr = point - getRotateHandleWorld();
    if (dr.x * dr.x + dr.y * dr.y <= rotRadius * rotRadius)
        return HANDLE_ROTATE;

    // Check 4 corner resize handles
    const float cornerRadius = 7.f;
    for (int i = 0; i < 4; ++i)
    {
        sf::Vector2f dc = point - getCornerWorld(i);
        if (dc.x * dc.x + dc.y * dc.y <= cornerRadius * cornerRadius)
            return i;
    }

    return HANDLE_NONE;
}

void Room::dragHandle(int handleIndex, sf::Vector2f newPos)
{
    // CASE A: ROTATION HANDLE
    if (handleIndex == HANDLE_ROTATE)
    {
        sf::Vector2f c = getCenter();
        sf::Vector2f v = newPos - c;
        float angleRad = std::atan2(v.y, v.x);
        float angleDeg = angleRad * 180.f / 3.14159265f;

        // Top-centre stem points upward (-90 degrees in screen Y-down coords),
        // so adding 90 degrees maps straight-up to 0 degrees.
        float currentAngle = angleDeg + 90.f;

        // Snapped 15-degree increments: 0, 15, 30, 45, ..., 345 deg
        float snapped = std::round(currentAngle / 15.f) * 15.f;
        while (snapped < 0.f)   snapped += 360.f;
        while (snapped >= 360.f) snapped -= 360.f;
        rotationDegrees = snapped;
        return;
    }

    // CASE B: CORNER RESIZE HANDLE
    // "Dragging a corner should resize the shape (recompute widthMeters/heightMeters),
    // keeping the OPPOSITE corner fixed, with a minimum size of 1 grid cell."
    if (handleIndex >= HANDLE_TL && handleIndex <= HANDLE_BL)
    {
        // Identify the opposite corner: TL (0) <-> BR (2), TR (1) <-> BL (3)
        int oppositeIndex = (handleIndex + 2) % 4;
        sf::Vector2f oppWorld = getCornerWorld(oppositeIndex);

        // Vector from the fixed opposite corner to the mouse in world space
        sf::Vector2f dWorld = newPos - oppWorld;

        // CG TRANSFORM: Inverse-rotate by -rotationDegrees into local unrotated axes
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
        case HANDLE_TL: // opp is BR (local vector points toward top-left, negative dx/dy)
            wPxNew = -dLocal.x;
            hPxNew = -dLocal.y;
            break;
        case HANDLE_TR: // opp is BL
            wPxNew = dLocal.x;
            hPxNew = -dLocal.y;
            break;
        case HANDLE_BR: // opp is TL
            wPxNew = dLocal.x;
            hPxNew = dLocal.y;
            break;
        case HANDLE_BL: // opp is TR
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

        // Center offset relative to the opposite corner in local space
        sf::Vector2f centerOffsetLocal;
        switch (handleIndex)
        {
        case HANDLE_TL: centerOffsetLocal = {-wPxNew / 2.f, -hPxNew / 2.f}; break;
        case HANDLE_TR: centerOffsetLocal = { wPxNew / 2.f, -hPxNew / 2.f}; break;
        case HANDLE_BR: centerOffsetLocal = { wPxNew / 2.f,  hPxNew / 2.f}; break;
        case HANDLE_BL: centerOffsetLocal = {-wPxNew / 2.f,  hPxNew / 2.f}; break;
        }

        // Transform center offset to world coordinates and place new center
        float fwdRad = rotationDegrees * 3.14159265f / 180.f;
        float fwdCos = std::cos(fwdRad);
        float fwdSin = std::sin(fwdRad);
        sf::Vector2f newCenterWorld(
            oppWorld.x + centerOffsetLocal.x * fwdCos - centerOffsetLocal.y * fwdSin,
            oppWorld.y + centerOffsetLocal.x * fwdSin + centerOffsetLocal.y * fwdCos
        );

        // Update stored top-left (x, y) so unrotated bounding box matches new center
        x = newCenterWorld.x - wPxNew / 2.f;
        y = newCenterWorld.y - hPxNew / 2.f;
    }
}

bool Room::contains(sf::Vector2f point) const
{
    // COMPUTER GRAPHICS: INVERSE-ROTATE-THEN-TEST
    // To hit-test against an arbitrarily rotated rectangle:
    // 1. Transform the screen click point by the inverse rotation matrix R(-theta)
    //    around the rectangle's centre.
    // 2. Perform an efficient Axis-Aligned Bounding Box (AABB) test in local model space!
    sf::Transform inv;
    inv.rotate(sf::degrees(-rotationDegrees), getCenter());
    sf::Vector2f localP = inv.transformPoint(point);

    float wPx = Grid::metersToPixels(widthMeters);
    float hPx = Grid::metersToPixels(heightMeters);
    return localP.x >= x && localP.x <= x + wPx &&
           localP.y >= y && localP.y <= y + hPx;
}

void Room::move(float dx, float dy)
{
    x += dx; y += dy;
}

double Room::getArea() const
{
    // ENCAPSULATION: the ONLY place area is ever calculated.
    return static_cast<double>(widthMeters) * static_cast<double>(heightMeters);
}

std::string Room::getInfo() const
{
    std::ostringstream ss;
    ss << roomType << "  |  Top: " << widthMeters << "m  Right: " << heightMeters
       << "m  Bottom: " << widthMeters << "m  Left: " << heightMeters
       << "m  |  Area: " << getArea() << " m2";
    if (std::abs(rotationDegrees) > 0.01f)
    {
        ss << "  |  Rot: " << static_cast<int>(std::round(rotationDegrees)) << " deg";
    }
    return ss.str();
}
