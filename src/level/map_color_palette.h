#pragma once

#include "level/color.h"
#include "level/palette.h"

namespace bl {

class MapColorPalette {
public:
    using hash_t = uint64_t;

    void init_biome_color_data(const std::filesystem::path& path);
    void init_block_color_data(const std::filesystem::path& path);

    Color const& get_color(palette::compound_tag& tag) const;

private:
    std::unordered_map<hash_t, Color> m_block_to_color_map;

    static Color UNKNOWN_BLOCK_COLOR;
};

} // namespace bl

// todo: remove this singleton.
inline bl::MapColorPalette bl_map_color_palette;
