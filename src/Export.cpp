#include "Export.h"
#include <filesystem>
#include <fstream>
#include <ctime>
#include <iostream>
#include <string>

namespace Export
{

bool exportAll(sf::RenderWindow& window, const FloorPlan& floorPlan)
{
    std::filesystem::create_directories("exports");

    // Timestamp so repeated exports don't overwrite one another.
    std::time_t now = std::time(nullptr);
    char stamp[32];
    std::strftime(stamp, sizeof(stamp), "%Y%m%d_%H%M%S", std::localtime(&now));
    std::string stampStr(stamp);

    // ---- 1) PNG snapshot of the current window ----
    sf::Vector2u size = window.getSize();
    sf::Texture texture(size);
    texture.update(window);
    bool imageOk = texture.copyToImage().saveToFile("exports/floorplan_" + stampStr + ".png");

    // ---- 2) plain-text dimension report ----
    std::ofstream report("exports/floorplan_report_" + stampStr + ".txt");
    bool reportOk = static_cast<bool>(report);

    if (reportOk)
    {
        report << "2D HOUSE FLOOR PLAN - EXPORT REPORT\n";
        report << "Generated: " << stampStr << "\n";
        report << "Scale: 1 grid cell = 1 metre\n\n";

        for (auto& shape : floorPlan.getShapes())
            report << " - " << shape->getInfo() << "\n";   // POLYMORPHISM: each shape describes itself

        report << "\nTOTAL HOUSE AREA: " << floorPlan.getTotalArea() << " m2\n";
        report.close();
    }
    else
    {
        std::cerr << "[Export] Could not write the report file.\n";
    }

    return imageOk && reportOk;
}

} // namespace Export
