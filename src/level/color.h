#pragma once

#include "level/data_3d.h"

namespace bl {

class Color {
public:
    constexpr Color() : m_red(0), m_green(0), m_blue(0), m_alpha(255) {}
    constexpr Color(
        uint8_t red,
        uint8_t green,
        uint8_t blue,
        uint8_t alpha = 255
    )
    : m_red(red),
      m_green(green),
      m_blue(blue),
      m_alpha(alpha) {}

    constexpr explicit Color(uint32_t hex) {
        if (hex <= 0xFFFFFF) {
            // 0xRRGGBB
            m_red   = (hex >> 16) & 0xFF;
            m_green = (hex >> 8) & 0xFF;
            m_blue  = hex & 0xFF;
            m_alpha = 255;
        } else {
            // 0xRRGGBBAA
            m_red   = (hex >> 24) & 0xFF;
            m_green = (hex >> 16) & 0xFF;
            m_blue  = (hex >> 8) & 0xFF;
            m_alpha = hex & 0xFF;
        }
    }

    constexpr auto r() const { return m_red; }
    constexpr auto g() const { return m_green; }
    constexpr auto b() const { return m_blue; }
    constexpr auto a() const { return m_alpha; }

    [[nodiscard]] constexpr auto data() const {
        return (static_cast<int32_t>(m_red) << 24)
             | (static_cast<int32_t>(m_green) << 16)
             | (static_cast<int32_t>(m_blue) << 6)
             | static_cast<int32_t>(m_alpha);
    }

private:
    uint8_t m_red, m_green, m_blue, m_alpha;
};

[[maybe_unused]] Color get_biome_color(biome b);

std::string get_biome_name(biome b);

Color get_block_color_from_SNBT(const std::string& name);

// [[maybe_unused]] Color get_water_color(Color gray, bl::biome b);

// Color get_leave_color(Color gray, bl::biome b);
// Color get_grass_color(Color gray, bl::biome b);

Color blend_color_with_biome(const std::string& name, Color color, bl::biome b);

[[maybe_unused]] std::unordered_map<std::string, Color>&
     get_block_color_table();
bool init_biome_color_palette_from_file(const std::string& filename);

bool init_block_color_palette_from_file(const std::string& filename);

[[maybe_unused]] void export_image(
    const std::vector<std::vector<Color>>& c,
    int                                    ppi,
    const std::string&                     name
);

} // namespace bl
