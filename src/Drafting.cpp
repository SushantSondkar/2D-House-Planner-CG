#include "Drafting.h"
#include "Grid.h"
#include "Fonts.h"
#include <sstream>
#include <vector>

namespace Drafting
{

void drawDimensionLine(sf::RenderWindow& window, sf::Vector2f a, sf::Vector2f b,
                        const std::string& label, bool vertical,
                        const sf::RenderStates& states)
{
    sf::Color c(80, 80, 80);

    sf::Vertex main[] = { sf::Vertex{a, c}, sf::Vertex{b, c} };
    window.draw(main, 2, sf::PrimitiveType::Lines, states);

    // small perpendicular "tick" marks at both ends - the standard
    // dimension-line symbol on architectural/civil drawings.
    float tick = 5.f;
    if (vertical)
    {
        sf::Vertex t1[] = { sf::Vertex{{a.x - tick, a.y}, c}, sf::Vertex{{a.x + tick, a.y}, c} };
        sf::Vertex t2[] = { sf::Vertex{{b.x - tick, b.y}, c}, sf::Vertex{{b.x + tick, b.y}, c} };
        window.draw(t1, 2, sf::PrimitiveType::Lines, states);
        window.draw(t2, 2, sf::PrimitiveType::Lines, states);
    }
    else
    {
        sf::Vertex t1[] = { sf::Vertex{{a.x, a.y - tick}, c}, sf::Vertex{{a.x, a.y + tick}, c} };
        sf::Vertex t2[] = { sf::Vertex{{b.x, b.y - tick}, c}, sf::Vertex{{b.x, b.y + tick}, c} };
        window.draw(t1, 2, sf::PrimitiveType::Lines, states);
        window.draw(t2, 2, sf::PrimitiveType::Lines, states);
    }

    sf::Text text(Fonts::get(), label, 10);
    text.setFillColor(c);
    sf::Vector2f mid((a.x + b.x) / 2.f, (a.y + b.y) / 2.f);
    if (vertical)
        text.setPosition({mid.x - 28.f, mid.y - 6.f});
    else
        text.setPosition({mid.x - 10.f, mid.y - 14.f});
    window.draw(text, states);
}

void drawCompass(sf::RenderWindow& window, sf::Vector2f center, float radius)
{
    sf::CircleShape circle(radius);
    circle.setOrigin({radius, radius});
    circle.setPosition(center);
    circle.setFillColor(sf::Color(255, 255, 255, 220));
    circle.setOutlineThickness(1.5f);
    circle.setOutlineColor(sf::Color::Black);
    window.draw(circle);

    // the "needle" - a thin triangle pointing to screen-up = North
    sf::ConvexShape needle(3);
    needle.setPoint(0, {center.x, center.y - radius + 3.f});
    needle.setPoint(1, {center.x - 5.f, center.y + 6.f});
    needle.setPoint(2, {center.x + 5.f, center.y + 6.f});
    needle.setFillColor(sf::Color::Red);
    window.draw(needle);

    sf::Text n(Fonts::get(), "N", 12);
    n.setFillColor(sf::Color::Black);
    n.setPosition({center.x - 4.f, center.y - radius - 15.f});
    window.draw(n);
}

void drawScaleBar(sf::RenderWindow& window, sf::Vector2f position)
{
    // A classic printed-plan scale bar: alternating black/white
    // 1-metre blocks, so the plan's scale survives printing at any size.
    const int METRES = 5;
    float unit = Grid::PIXELS_PER_METER;

    sf::Text title(Fonts::get(), "SCALE (metres)", 9);
    title.setFillColor(sf::Color::Black);
    title.setPosition({position.x, position.y - 14.f});
    window.draw(title);

    for (int i = 0; i < METRES; ++i)
    {
        sf::RectangleShape block({unit, 8.f});
        block.setPosition({position.x + i * unit, position.y});
        block.setFillColor(i % 2 == 0 ? sf::Color::Black : sf::Color::White);
        block.setOutlineThickness(1.f);
        block.setOutlineColor(sf::Color::Black);
        window.draw(block);
    }

    for (int i = 0; i <= METRES; ++i)
    {
        std::ostringstream ss;
        ss << i;
        sf::Text t(Fonts::get(), ss.str(), 9);
        t.setFillColor(sf::Color::Black);
        t.setPosition({position.x + i * unit - 3.f, position.y + 10.f});
        window.draw(t);
    }
}

void drawTitleBlock(sf::RenderWindow& window, sf::Vector2f topLeft, double totalAreaM2)
{
    sf::RectangleShape box({220.f, 72.f});
    box.setPosition(topLeft);
    box.setFillColor(sf::Color(250, 250, 245));
    box.setOutlineThickness(1.5f);
    box.setOutlineColor(sf::Color::Black);
    window.draw(box);

    std::ostringstream ss;
    ss << "PROJECT: House Floor Plan\n"
       << "SCALE: 1 grid cell = 1 m\n"
       << "TOTAL AREA: " << totalAreaM2 << " m2\n"
       << "DRAWN BY: OOPCG Student";

    sf::Text text(Fonts::get(), ss.str(), 11);
    text.setFillColor(sf::Color::Black);
    text.setPosition({topLeft.x + 6.f, topLeft.y + 4.f});
    window.draw(text);
}

void drawLegend(sf::RenderWindow& window, sf::Vector2f topLeft)
{
    struct Item { std::string label; sf::Color color; };
    Item items[] = {
        {"Wall",        sf::Color(60, 60, 60)},
        {"Curved Wall", sf::Color(60, 60, 60)},
        {"Door",        sf::Color(139, 69, 19)},
        {"Window",      sf::Color::Blue},
        {"Room",        sf::Color(180, 210, 255)},
        {"Staircase",   sf::Color(120, 120, 180)}
    };

    sf::RectangleShape box({130.f, 128.f});
    box.setPosition(topLeft);
    box.setFillColor(sf::Color(250, 250, 245, 230));
    box.setOutlineThickness(1.f);
    box.setOutlineColor(sf::Color::Black);
    window.draw(box);

    sf::Text title(Fonts::get(), "LEGEND", 11);
    title.setFillColor(sf::Color::Black);
    title.setPosition({topLeft.x + 6.f, topLeft.y + 2.f});
    window.draw(title);

    float yPos = topLeft.y + 20.f;
    for (auto& it : items)
    {
        sf::RectangleShape swatch({12.f, 12.f});
        swatch.setPosition({topLeft.x + 6.f, yPos});
        swatch.setFillColor(it.color);
        window.draw(swatch);

        sf::Text t(Fonts::get(), it.label, 10);
        t.setFillColor(sf::Color::Black);
        t.setPosition({topLeft.x + 24.f, yPos - 2.f});
        window.draw(t);

        yPos += 17.f;
    }
}

} // namespace Drafting
