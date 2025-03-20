#include "level/color.h"
#include "level/palette.h"

#include <nlohmann/json.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

namespace bl {

namespace {

// Biome-related color whitelist
std::unordered_map<biome, Color> biome_water_map;
std::unordered_map<biome, Color> biome_leave_map;
std::unordered_map<biome, Color> biome_grass_map;

Color default_water_color{63, 118, 228};
Color default_leave_color{113, 167, 77};
Color default_grass_color{142, 185, 113};

// biome id -> name
std::unordered_map<biome, std::string> biome_id_map;
// biome id -> biome color
std::unordered_map<biome, Color> biome_color_map;

} // namespace

Color get_biome_color(bl::biome biome) {
    return biome_color_map.contains(biome) ? biome_color_map.at(biome)
                                           : Color{};
}

std::string get_biome_name(bl::biome biome) {
    return biome_id_map.contains(biome) ? biome_id_map.at(biome) : "unknown";
}

bool init_biome_color_palette_from_file(const std::string& filename) {
    try {
        std::ifstream f(filename);
        if (!f.is_open()) {
            BL_ERROR("Can not open biome color file %s", filename.c_str());
            return false;
        }
        nlohmann::json j;
        f >> j;
        for (auto& [key, value] : j.items()) {
            int id                               = value["id"].get<int>();
            biome_id_map[static_cast<biome>(id)] = key;

            if (value.contains("rgb")) {
                auto rgb = value["rgb"];
                assert(rgb.size() == 3);
                Color c{
                    static_cast<uint8_t>(rgb[0].get<int>()),
                    static_cast<uint8_t>(rgb[1].get<int>()),
                    static_cast<uint8_t>(rgb[2].get<int>())
                };
                biome_color_map[static_cast<biome>(id)] = c;
            }

            // water
            if (value.contains("water")) {
                auto water = value["water"];
                assert(water.size() == 3);
                Color c{
                    static_cast<uint8_t>(water[0].get<int>()),
                    static_cast<uint8_t>(water[1].get<int>()),
                    static_cast<uint8_t>(water[2].get<int>())
                };
                biome_water_map[static_cast<biome>(id)] = c;
                if (key == "default") default_water_color = c;
            }

            if (value.contains("grass")) {
                auto grass = value["grass"];
                assert(grass.size() == 3);
                Color c{
                    static_cast<uint8_t>(grass[0].get<int>()),
                    static_cast<uint8_t>(grass[1].get<int>()),
                    static_cast<uint8_t>(grass[2].get<int>())
                };
                biome_grass_map[static_cast<biome>(id)] = c;
                if (key == "default") default_grass_color = c;
            }

            if (value.contains("leaves")) {
                auto leaves = value["leaves"];
                assert(leaves.size() == 3);
                Color c{
                    static_cast<uint8_t>(leaves[0].get<int>()),
                    static_cast<uint8_t>(leaves[1].get<int>()),
                    static_cast<uint8_t>(leaves[2].get<int>())
                };
                biome_leave_map[static_cast<biome>(id)] = c;
                if (key == "default") default_leave_color = c;
            }
        }

    } catch (std::exception&) {
        return false;
    }

    BL_LOGGER("Water color Map: %zu", biome_water_map.size());
    BL_LOGGER("Leaves color Map: %zu", biome_leave_map.size());
    BL_LOGGER("Grass color Map: %zu", biome_grass_map.size());
    return true;
}

void export_image(
    const std::vector<std::vector<Color>>& b,
    int                                    ppi,
    const std::string&                     name
) {
    const int c = 3;
    const int h = (int)b.size() * ppi;
    const int w = (int)b[0].size() * ppi;

    std::vector<unsigned char> data(c * w * h, 0);

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            auto color                = b[i / ppi][j / ppi];
            data[3 * (j + i * w)]     = color.r();
            data[3 * (j + i * w) + 1] = color.g();
            data[3 * (j + i * w) + 2] = color.b();
        }
    }

    stbi_write_png(name.c_str(), w, h, c, data.data(), 0);
}

} // namespace bl
