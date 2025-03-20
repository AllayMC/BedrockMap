#include "level/map_color_palette.h"

#include <nlohmann/json.hpp>
#include <print>

namespace bl {

namespace {

constexpr uint32_t fnv1a_32(char* data, size_t len) {
    uint32_t hash  = 0x811c9dc5;
    uint32_t prime = 0x1000193;

    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint8_t>(data[i]);
        hash *= prime;
    }

    return hash;
}

} // namespace

Color MapColorPalette::UNKNOWN_BLOCK_COLOR{173, 8, 172};

void MapColorPalette::init_biome_color_data(const std::filesystem::path& path) {
}

void MapColorPalette::init_block_color_data(const std::filesystem::path& path) {
    std::ifstream ifs(path);
    if (!ifs) {
        throw std::runtime_error("TODO");
    }

    auto data = nlohmann::json::parse(ifs);
    for (const auto& block_state : data) {
        m_block_to_color_map.emplace(
            block_state["blockStateHash"].get<hash_t>(),
            Color{block_state["mapColor"].get<std::string>()}
        );
    }
}

Color const& MapColorPalette::get_color(palette::compound_tag& tag) const {
    auto raw  = tag.to_raw();
    auto hash = fnv1a_32(raw.data(), raw.size());
    if (m_block_to_color_map.contains(hash)) {
        std::println(
            "[{}] color => #{:08X}",
            tag.get("name")->value_string(),
            m_block_to_color_map.at(hash).data()
        );
        return m_block_to_color_map.at(hash);
    } else {
        std::println("[[MISMATCH: {}]]", tag.to_readable_string());
        return UNKNOWN_BLOCK_COLOR;
    }
}

} // namespace bl
