#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

/*
 * ============================================================
 *  Fonts.h
 * ------------------------------------------------------------
 *  Loads ONE font file ONE time (lazy singleton) and hands
 *  every class a reference to the same sf::Font, avoiding
 *  repeated slow disk reads every frame.
 *
 *  NOTE (SFML 3 API): the loading function is openFromFile()
 *  in SFML 3 - it was called loadFromFile() in SFML 2.
 * ============================================================
 */
namespace Fonts
{
    inline sf::Font& get()
    {
        static sf::Font font;
        static bool loaded = false;

        if (!loaded)
        {
            const char* candidates[] = {
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
                "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
                "C:/Windows/Fonts/arial.ttf",
                "/Library/Fonts/Arial.ttf",
                "assets/font.ttf"
            };

            for (const char* path : candidates)
            {
                if (font.openFromFile(path))
                {
                    loaded = true;
                    break;
                }
            }

            if (!loaded)
                std::cerr << "[Warning] No font found - text labels will not be visible.\n"
                             "Copy any .ttf file to 'assets/font.ttf' next to the executable.\n";
        }

        return font;
    }
}
