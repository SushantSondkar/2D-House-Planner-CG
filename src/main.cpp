#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <optional>

#include "Grid.h"
#include "Fonts.h"
#include "Shape.h"
#include "Wall.h"
#include "CurvedWall.h"
#include "Door.h"
#include "Window.h"
#include "Room.h"
#include "Staircase.h"
#include "FloorPlan.h"
#include "Drafting.h"
#include "Export.h"

/*
 * ============================================================
 *  main.cpp   -   2D HOUSE FLOOR PLAN DESIGNER (OOPCG Project)
 * ------------------------------------------------------------
 *  LIBRARY USED: SFML (Simple and Fast Multimedia Library) 3.x
 *  ---------------------------------------------------------
 *   WHAT it is used for in this project:
 *     - sf::RenderWindow            -> the window we draw onto
 *     - sf::RectangleShape,
 *       sf::CircleShape,
 *       sf::ConvexShape,
 *       sf::Vertex/PrimitiveType     -> the 2D drawing PRIMITIVES
 *                                       (rectangles, circles, lines,
 *                                       triangles, curves) used to
 *                                       render every object
 *     - sf::Text / sf::Font          -> room labels, dimensions,
 *                                       the status bar, title block
 *     - sf::Event / sf::Mouse        -> mouse clicks, movement,
 *                                       window-close events
 *     - sf::Texture + sf::Image      -> capturing the window for
 *                                       the PNG export feature
 *
 *   WHY SFML (and not a bigger engine/GUI framework): SFML gives
 *   us a canvas, 2D primitives and input events, and nothing
 *   more - we still write our own coordinate maths, our own
 *   class hierarchy and our own hit-testing, exactly what an
 *   OOPCG project is meant to demonstrate.
 *
 *  ROLE OF THIS FILE: main.cpp is "glue code" - the window, the
 *  toolbar and the event loop. Every interesting OOP behaviour
 *  (drawing, area calculation, hit-testing) lives in the
 *  Shape/Wall/CurvedWall/Door/Window/Room/Staircase/FloorPlan
 *  classes defined elsewhere.
 * ============================================================
 */

enum class ToolMode
{
    SELECT, WALL, CURVE, DOOR, WINDOW,
    BEDROOM, LIVING, KITCHEN, BATHROOM, STAIRS,
    DELETE_TOOL, UNDO_TOOL, EXPORT_TOOL, CLEAR_TOOL
};

struct ToolButton
{
    sf::RectangleShape box;
    sf::Text label;
    ToolMode mode;
};

static ToolButton makeButton(float x, float y, float w, float h,
                              const std::string& text, ToolMode mode)
{
    ToolButton b{
        sf::RectangleShape({w, h}),
        sf::Text(Fonts::get(), text, 13),
        mode
    };
    b.box.setPosition({x, y});
    b.box.setFillColor(sf::Color(220, 220, 220));
    b.box.setOutlineThickness(1.f);
    b.box.setOutlineColor(sf::Color::Black);

    b.label.setFillColor(sf::Color::Black);
    b.label.setPosition({x + 6.f, y + 9.f});

    return b;
}

int main()
{
    const unsigned WIN_W = 1150, WIN_H = 760;
    const float STATUS_BAR_HEIGHT = 40.f;

    sf::RenderWindow window(sf::VideoMode({WIN_W, WIN_H}),
                             "2D House Floor Plan Designer - OOPCG Project");
    window.setFramerateLimit(60);

    // ---------------- build the toolbar buttons (2 rows x 7) ----------------
    const std::string names[] = {
        "Select", "Wall", "Curve", "Door", "Window", "Bedroom", "Living",
        "Kitchen", "Bathroom", "Stairs", "Delete", "Undo", "Export", "Clear"
    };
    const ToolMode modes[] = {
        ToolMode::SELECT, ToolMode::WALL, ToolMode::CURVE, ToolMode::DOOR, ToolMode::WINDOW,
        ToolMode::BEDROOM, ToolMode::LIVING,
        ToolMode::KITCHEN, ToolMode::BATHROOM, ToolMode::STAIRS,
        ToolMode::DELETE_TOOL, ToolMode::UNDO_TOOL, ToolMode::EXPORT_TOOL, ToolMode::CLEAR_TOOL
    };

    std::vector<ToolButton> buttons;
    const float btnW = 158.f, btnH = 40.f, gap = 6.f;
    for (int i = 0; i < 14; ++i)
    {
        int col = i % 7, row = i / 7;
        float bx = 8.f + col * (btnW + gap);
        float by = 8.f + row * (btnH + 8.f);
        buttons.push_back(makeButton(bx, by, btnW, btnH, names[i], modes[i]));
    }

    ToolMode currentTool = ToolMode::SELECT;

    // COMPOSITION: owns every shape on screen.
    FloorPlan floorPlan;

    // ---------------- interaction state ----------------
    bool wallStartSet = false;
    sf::Vector2f wallStart;

    int curveStage = 0;                 // 0=idle, 1=have start, 2=have end (awaiting control point)
    sf::Vector2f curveStart, curveEnd;

    bool roomDragging = false;
    sf::Vector2f roomStart;

    Shape* selectedShape = nullptr;
    bool draggingShape = false;
    int  activeHandle = Shape::HANDLE_NONE;
    bool draggingHandle = false;
    sf::Vector2f lastMousePos;
    sf::Vector2f currentMousePos;

    auto resetPendingTools = [&]()
    {
        wallStartSet = false;
        curveStage = 0;
        roomDragging = false;
        draggingShape = false;
        draggingHandle = false;
        activeHandle = Shape::HANDLE_NONE;
    };

    while (window.isOpen())
    {
        // ================= EVENT HANDLING (SFML 3 style) =================
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            // ---- mouse pressed ----
            if (const auto* mp = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mp->button == sf::Mouse::Button::Left)
                {
                    sf::Vector2f pos(static_cast<float>(mp->position.x),
                                      static_cast<float>(mp->position.y));

                    // (1) toolbar button clicked?
                    bool hitButton = false;
                    for (auto& b : buttons)
                    {
                        if (b.box.getGlobalBounds().contains(pos))
                        {
                            resetPendingTools();
                            currentTool = b.mode;
                            hitButton = true;

                            if (currentTool != ToolMode::SELECT && selectedShape)
                            {
                                selectedShape->setSelected(false);
                                selectedShape = nullptr;
                            }

                            if (currentTool == ToolMode::CLEAR_TOOL)
                            {
                                floorPlan.clear();
                                selectedShape = nullptr;
                                currentTool = ToolMode::SELECT;
                            }
                            else if (currentTool == ToolMode::UNDO_TOOL)
                            {
                                if (selectedShape) { selectedShape = nullptr; }
                                floorPlan.undoLast();
                                currentTool = ToolMode::SELECT;
                            }
                            else if (currentTool == ToolMode::EXPORT_TOOL)
                            {
                                Export::exportAll(window, floorPlan);
                                currentTool = ToolMode::SELECT;
                            }
                            break;
                        }
                    }
                    if (hitButton) continue;

                    // (2) ignore clicks outside the canvas area
                    if (pos.y < Grid::CANVAS_TOP) continue;
                    if (pos.y > WIN_H - STATUS_BAR_HEIGHT) continue;

                    // (3) act according to the current tool
                    switch (currentTool)
                    {
                    case ToolMode::WALL:
                    {
                        sf::Vector2f snapped(Grid::snap(pos.x), Grid::snap(pos.y));
                        if (!wallStartSet) { wallStart = snapped; wallStartSet = true; }
                        else
                        {
                            floorPlan.addShape(std::make_unique<Wall>(wallStart, snapped));
                            wallStartSet = false;
                        }
                        break;
                    }

                    case ToolMode::CURVE:
                    {
                        // 3-click gesture: start -> end -> control point (bulge)
                        sf::Vector2f snapped(Grid::snap(pos.x), Grid::snap(pos.y));
                        if (curveStage == 0) { curveStart = snapped; curveStage = 1; }
                        else if (curveStage == 1) { curveEnd = snapped; curveStage = 2; }
                        else
                        {
                            floorPlan.addShape(std::make_unique<CurvedWall>(curveStart, snapped, curveEnd));
                            curveStage = 0;
                        }
                        break;
                    }

                    case ToolMode::DOOR:
                    {
                        sf::Vector2f snapped(Grid::snap(pos.x), Grid::snap(pos.y));
                        floorPlan.addShape(std::make_unique<Door>(snapped.x, snapped.y));
                        break;
                    }

                    case ToolMode::WINDOW:
                    {
                        sf::Vector2f snapped(Grid::snap(pos.x), Grid::snap(pos.y));
                        floorPlan.addShape(std::make_unique<::Window>(snapped.x, snapped.y));
                        break;
                    }

                    case ToolMode::BEDROOM: case ToolMode::LIVING:
                    case ToolMode::KITCHEN: case ToolMode::BATHROOM:
                    case ToolMode::STAIRS:
                    {
                        roomStart = sf::Vector2f(Grid::snap(pos.x), Grid::snap(pos.y));
                        roomDragging = true;
                        break;
                    }

                    case ToolMode::DELETE_TOOL:
                    {
                        Shape* hit = floorPlan.findShapeAt(pos);
                        if (hit)
                        {
                            if (hit == selectedShape) selectedShape = nullptr;
                            floorPlan.removeShape(hit);
                        }
                        break;
                    }

                    case ToolMode::SELECT:
                    {
                        /*
                         * ----------------------------------------------------
                         *  OOP & INTERACTION MODEL: FIGMA/POWERPOINT STYLE
                         * ----------------------------------------------------
                         *  1) If a shape is ALREADY selected, we first hit-test
                         *     against its interactive handles (endpoints, corners,
                         *     rotate node).
                         *  2) If a handle was clicked: initiate handle dragging
                         *     (resizing or re-orienting) on that handle.
                         *  3) If not on a handle, check if the click was inside
                         *     the shape body: initiate whole-shape moving.
                         *  4) If click was outside: select whatever other shape
                         *     is clicked, or deselect if clicking empty space.
                         * ----------------------------------------------------
                         */
                        if (selectedShape)
                        {
                            int handle = selectedShape->getHandleAt(pos);
                            if (handle != Shape::HANDLE_NONE)
                            {
                                activeHandle = handle;
                                draggingHandle = true;
                                lastMousePos = pos;
                                selectedShape->onHandlePressed(activeHandle, pos);
                                break;
                            }
                        }

                        Shape* hit = floorPlan.findShapeAt(pos);
                        if (hit)
                        {
                            if (selectedShape && selectedShape != hit)
                                selectedShape->setSelected(false);

                            selectedShape = hit;
                            selectedShape->setSelected(true);
                            draggingShape = true;
                            lastMousePos = pos;
                        }
                        else
                        {
                            // Clicked empty canvas space -> clear selection
                            if (selectedShape)
                            {
                                selectedShape->setSelected(false);
                                selectedShape = nullptr;
                            }
                        }
                        break;
                    }

                    default: break;
                    }
                }
            }

            // ---- mouse released ----
            if (const auto* mr = event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (mr->button == sf::Mouse::Button::Left)
                {
                    draggingShape = false;
                    draggingHandle = false;
                    activeHandle = Shape::HANDLE_NONE;

                    if (roomDragging)
                    {
                        sf::Vector2f end(Grid::snap(currentMousePos.x), Grid::snap(currentMousePos.y));
                        float wPx = std::fabs(end.x - roomStart.x);
                        float hPx = std::fabs(end.y - roomStart.y);

                        if (wPx >= Grid::PIXELS_PER_METER && hPx >= Grid::PIXELS_PER_METER)
                        {
                            float topLeftX = std::min(roomStart.x, end.x);
                            float topLeftY = std::min(roomStart.y, end.y);
                            float wM = Grid::pixelsToMeters(wPx);
                            float hM = Grid::pixelsToMeters(hPx);

                            if (currentTool == ToolMode::STAIRS)
                            {
                                floorPlan.addShape(std::make_unique<Staircase>(topLeftX, topLeftY, wM, hM));
                            }
                            else
                            {
                                std::string type =
                                    (currentTool == ToolMode::BEDROOM)  ? "Bedroom"     :
                                    (currentTool == ToolMode::LIVING)   ? "Living Room" :
                                    (currentTool == ToolMode::KITCHEN)  ? "Kitchen"     : "Bathroom";
                                floorPlan.addShape(std::make_unique<Room>(topLeftX, topLeftY, wM, hM, type));
                            }
                        }
                        roomDragging = false;
                    }
                }
            }

            // ---- mouse moved ----
            if (const auto* mm = event->getIf<sf::Event::MouseMoved>())
            {
                currentMousePos = sf::Vector2f(static_cast<float>(mm->position.x),
                                                 static_cast<float>(mm->position.y));

                // POLYMORPHISM IN ACTION:
                // If dragging a handle, Shape::dragHandle polymorphically updates
                // the shape (resizing or rotating) without main.cpp knowing the
                // concrete class type!
                if (draggingHandle && selectedShape && activeHandle != Shape::HANDLE_NONE)
                {
                    selectedShape->dragHandle(activeHandle, currentMousePos);
                }
                else if (draggingShape && selectedShape)
                {
                    float dx = currentMousePos.x - lastMousePos.x;
                    float dy = currentMousePos.y - lastMousePos.y;
                    selectedShape->move(dx, dy);
                    lastMousePos = currentMousePos;
                }
            }
        }

        // ===================== DRAWING =====================
        window.clear(sf::Color::White);

        // ---- background GRID ----
        for (float gx = 0.f; gx <= WIN_W; gx += Grid::PIXELS_PER_METER)
        {
            sf::Vertex line[] = {
                sf::Vertex{{gx, Grid::CANVAS_TOP}, sf::Color(230, 230, 230)},
                sf::Vertex{{gx, WIN_H - STATUS_BAR_HEIGHT}, sf::Color(230, 230, 230)}
            };
            window.draw(line, 2, sf::PrimitiveType::Lines);
        }
        for (float gy = Grid::CANVAS_TOP; gy <= WIN_H - STATUS_BAR_HEIGHT; gy += Grid::PIXELS_PER_METER)
        {
            sf::Vertex line[] = {
                sf::Vertex{{0.f, gy}, sf::Color(230, 230, 230)},
                sf::Vertex{{static_cast<float>(WIN_W), gy}, sf::Color(230, 230, 230)}
            };
            window.draw(line, 2, sf::PrimitiveType::Lines);
        }

        // ---- every shape (POLYMORPHISM: one call fits every type) ----
        floorPlan.drawAll(window);

        // ---- selection handles (drawn ON TOP of shapes, only when Select tool active) ----
        if (currentTool == ToolMode::SELECT && selectedShape)
        {
            selectedShape->drawHandles(window);
        }

        // ---- live preview: dragging a new room/staircase rectangle ----
        if (roomDragging)
        {
            sf::Vector2f snapped(Grid::snap(currentMousePos.x), Grid::snap(currentMousePos.y));
            sf::RectangleShape preview({snapped.x - roomStart.x, snapped.y - roomStart.y});
            preview.setPosition(roomStart);
            preview.setFillColor(sf::Color(0, 150, 255, 60));
            preview.setOutlineColor(sf::Color::Blue);
            preview.setOutlineThickness(1.f);
            window.draw(preview);
        }

        // ---- live preview: curved wall being placed ----
        if (curveStage == 1)
        {
            sf::Vertex line[] = {
                sf::Vertex{curveStart, sf::Color(150, 150, 255)},
                sf::Vertex{currentMousePos, sf::Color(150, 150, 255)}
            };
            window.draw(line, 2, sf::PrimitiveType::Lines);
        }
        else if (curveStage == 2)
        {
            CurvedWall preview(curveStart, currentMousePos, curveEnd);
            preview.draw(window);
        }

        // ---- civil-engineer plan furniture (drawn ON TOP, sheet overlays) ----
        Drafting::drawCompass(window, {WIN_W - 55.f, Grid::CANVAS_TOP + 50.f}, 32.f);
        Drafting::drawScaleBar(window, {12.f, WIN_H - STATUS_BAR_HEIGHT - 26.f});
        Drafting::drawTitleBlock(window, {WIN_W - 230.f, WIN_H - STATUS_BAR_HEIGHT - 82.f},
                                  floorPlan.getTotalArea());
        Drafting::drawLegend(window, {8.f, Grid::CANVAS_TOP + 8.f});

        // ---- toolbar background + buttons ----
        sf::RectangleShape toolbarBg({static_cast<float>(WIN_W), Grid::CANVAS_TOP});
        toolbarBg.setFillColor(sf::Color(245, 245, 245));
        window.draw(toolbarBg);

        for (auto& b : buttons)
        {
            b.box.setFillColor(b.mode == currentTool ? sf::Color(180, 220, 255)
                                                       : sf::Color(220, 220, 220));
            window.draw(b.box);
            window.draw(b.label);
        }

        // ---- status bar ----
        sf::RectangleShape statusBg({static_cast<float>(WIN_W), STATUS_BAR_HEIGHT});
        statusBg.setPosition({0.f, WIN_H - STATUS_BAR_HEIGHT});
        statusBg.setFillColor(sf::Color(245, 245, 245));
        window.draw(statusBg);

        std::ostringstream ss;
        ss << (selectedShape ? selectedShape->getInfo() : std::string("No shape selected"));
        ss << "        |        Total House Area: " << floorPlan.getTotalArea() << " m^2";

        sf::Text statusText(Fonts::get(), ss.str(), 14);
        statusText.setFillColor(sf::Color::Black);
        statusText.setPosition({10.f, WIN_H - STATUS_BAR_HEIGHT + 10.f});
        window.draw(statusText);

        window.display();
    }

    return 0;
}
