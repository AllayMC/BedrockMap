#include <gtest/gtest.h>

#include "level/color.h"
#include "level/data_3d.h"

TEST(Color, readColorPalette) {
    bl::init_biome_color_palette_from_file(
        R"(C:\Users\xhy\dev\bedrock-level\data\colors\biome.json)"
    );
}

TEST(Color, readBlockPalette) {
    bl::init_block_color_palette_from_file(
        R"(C:\Users\xhy\dev\bedrock-level\data\colors\block.json)"
    );
}

TEST(Color, exportImage) {
    bl::init_biome_color_palette_from_file(
        R"(C:\Users\xhy\dev\bedrock-level\data\colors\biome.json)"
    );
    std::vector<std::vector<bl::biome>> b(
        40,
        std::vector<bl::biome>(60, bl::biome::cherry_groves)
    );
    b[12][32] = bl::biome::ocean;
    std::vector<std::vector<bl::Color>> c(
        40,
        std::vector<bl::Color>(60, bl::Color())
    );
    for (auto i = 0u; i < b.size(); i++) {
        for (auto j = 0u; j < b[0].size(); j++) {
            c[i][j] = bl::get_biome_color(b[i][j]);
        }
    }
    bl::export_image(c, 10, "a.png");
}