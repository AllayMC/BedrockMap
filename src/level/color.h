#pragma once

#include "level/data_3d.h"

namespace bl {

constexpr uint32_t color_hexstr_to_int(std::string_view hex) {
    if (hex.empty() || hex[0] != '#'
        || (hex.length() != 7 && hex.length() != 9)) {
        throw std::runtime_error("TODO");
    }

    uint32_t result{};
    if (std::from_chars(hex.data() + 1, hex.data() + hex.size(), result, 16).ec
        != std::errc()) {
        throw std::runtime_error("TODO");
    }

    return result;
}

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

    // 0xRRGGBBAA
    constexpr explicit Color(uint32_t hex) {
        m_red   = (hex >> 24) & 0xFF;
        m_green = (hex >> 16) & 0xFF;
        m_blue  = (hex >> 8) & 0xFF;
        m_alpha = hex & 0xFF;
    }

    constexpr explicit Color(std::string_view hex)
    : Color(color_hexstr_to_int(hex)) {}

    constexpr auto r() const { return m_red; }
    constexpr auto g() const { return m_green; }
    constexpr auto b() const { return m_blue; }
    constexpr auto a() const { return m_alpha; }

    [[nodiscard]] constexpr auto data() const {
        return (static_cast<uint32_t>(m_red) << 24)
             | (static_cast<uint32_t>(m_green) << 16)
             | (static_cast<uint32_t>(m_blue) << 8)
             | static_cast<uint32_t>(m_alpha);
    }

private:
    uint8_t m_red, m_green, m_blue, m_alpha;
};

[[maybe_unused]] Color get_biome_color(biome b);

std::string get_biome_name(biome b);

bool init_biome_color_palette_from_file(const std::string& filename);

[[maybe_unused]] void export_image(
    const std::vector<std::vector<Color>>& c,
    int                                    ppi,
    const std::string&                     name
);

} // namespace bl
