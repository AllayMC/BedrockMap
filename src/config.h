#pragma once

#include "level/bedrock_key.h"

#include <QColor>
#include <QImage>

typedef bl::chunk_pos region_pos;

struct cfg {
    const static std::string SOFTWARE_NAME;
    const static std::string SOFTWARE_VERSION;

    // Not configurable
    // Configuration file
    const static std::string CONFIG_FILE_PATH;
    // Color table file path
    const static std::string BLOCK_FILE_PATH;
    const static std::string BIOME_FILE_PATH;
    static constexpr uint8_t RW =
        8u; //(1<<w) //The size of region d. A region consists of RW * RW
            //blocks, and the
            // block coordinates are aligned to multiples of 8.

    const static int GRID_WIDTH; // Map area grid width (unit: block)
    // Configurable
    static int   SHADOW_LEVEL; // Shading levels for terrain maps.
    static float ZOOM_SPEED;   // Scroll wheel zoom speed.
    static int   THREAD_NUM;   // Number of background threads.
    static int   REGION_CACHE_SIZE;
    static int   EMPTY_REGION_CACHE_SIZE;
    static int   MINIMUM_SCALE_LEVEL;
    static int   MAXIMUM_SCALE_LEVEL;
    static int   FONT_SIZE;
    static bool  FANCY_TERRAIN_RENDER; // Whether to render shadows
    static bool
        LOAD_GLOBAL_DATA; // Whether to load global data such as villages
    static bool        OPEN_NBT_EDITOR_ONLY; // Open the nbt editor directly
    static std::string COLOR_THEME;

    // Runtime configuration.
    static bool transparent_void;

    static region_pos c2r(const bl::chunk_pos& ch);

    static void initColorTable();

    static void initConfig();

    static QImage CREATE_REGION_IMG(const std::bitset<cfg::RW * cfg::RW>& bitmap
    );

    static QImage* UNLOADED_REGION_IMAGE();

    static QImage* NULL_REGION_IMAGE();

    //    static QImage *EMPTY_REGION_IMAGE();

    static QString VERSION_STRING();
};
