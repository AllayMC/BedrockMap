//
// Created by xhy on 2023/7/11.
//
#include "config.h"

#include <qglobal.h>
#include <qnumeric.h>

#include <QDir>
#include <QtDebug>
#include <fstream>

#include "level/color.h"
#include "nlohmann/json.hpp"

namespace {

//    QImage *bg_{nullptr};
    QImage *unloaded_region_image_{nullptr};  // Unloaded regions.
    QImage *null_region_image_{nullptr};      // Empty areas with no valid chunks.
    //    QImage *transparent_region_img_{nullptr};
}  // namespace

const std::string cfg::SOFTWARE_NAME = "BedrockMap";
const std::string cfg::SOFTWARE_VERSION = "v0.4.0";
// Not configurable
const int cfg::GRID_WIDTH = 32;
// Configurable (all have default values)
int cfg::SHADOW_LEVEL = 128;
float cfg::ZOOM_SPEED = 1.2;
int cfg::THREAD_NUM = 8;
int cfg::REGION_CACHE_SIZE = 4096;
int cfg::EMPTY_REGION_CACHE_SIZE = 16384;
int cfg::MINIMUM_SCALE_LEVEL = 4;
int cfg::MAXIMUM_SCALE_LEVEL = 1024;
bool cfg::FANCY_TERRAIN_RENDER = true;
bool cfg::LOAD_GLOBAL_DATA = true;
bool cfg::OPEN_NBT_EDITOR_ONLY = false;
std::string cfg::COLOR_THEME = "developing";
int cfg::FONT_SIZE = 10;
//Runtime mutable
bool cfg::transparent_void = false;

//The paths to three important files are directly built-in.
#ifdef QT_DEBUG
const std::string cfg::CONFIG_FILE_PATH = R"(../config.json)";
const std::string cfg::BLOCK_FILE_PATH = R"(../bedrock-level/data/colors/block_color.json)";
const std::string cfg::BIOME_FILE_PATH = R"(../bedrock-level/data/colors/biome_color.json)";
#else
const std::string cfg::CONFIG_FILE_PATH = "config.json";
const std::string cfg::BLOCK_FILE_PATH = "block_color.json";
const std::string cfg::BIOME_FILE_PATH = "biome_color.json";
#endif

region_pos cfg::c2r(const bl::chunk_pos &ch) {
    auto cx = ch.x < 0 ? ch.x - cfg::RW + 1 : ch.x;
    auto cz = ch.z < 0 ? ch.z - cfg::RW + 1 : ch.z;
    return region_pos{cx / cfg::RW * cfg::RW, cz / cfg::RW * cfg::RW, ch.dim};
}

void cfg::initColorTable() {
    if (!bl::init_biome_color_palette_from_file(cfg::BIOME_FILE_PATH)) {
        qWarning() << "Can not load biome color file in path: " << BIOME_FILE_PATH.c_str();
    }

    if (!bl::init_block_color_palette_from_file(cfg::BLOCK_FILE_PATH)) {
        qWarning() << "Can not load block color file in path: " << BLOCK_FILE_PATH.c_str();
    }

    // init image

    unloaded_region_image_ = new QImage(cfg::RW << 4, cfg::RW << 4, QImage::Format_RGB888);
    null_region_image_ = new QImage(cfg::RW << 4, cfg::RW << 4, QImage::Format_RGBA8888);
//    default_region_image_ = new QImage(cfg::RW << 4, cfg::RW << 4, QImage::Format_RGB888);
    const int BW = cfg::RW << 4;
    for (int i = 0; i < BW; i++) {
        for (int j = 0; j < BW; j++) {
            const int arr1[2]{128, 148};
            const int arr2[2]{20, 40};

            const int idx = (i / (cfg::RW * 8) + j / (cfg::RW * 8)) % 2;
            assert(unloaded_region_image_);
            unloaded_region_image_->setPixelColor(i, j, QColor(arr1[idx], arr1[idx], arr1[idx]));
            null_region_image_->setPixelColor(i, j, QColor(arr2[idx], arr2[idx], arr2[idx]));

            //            default_region_image_->setPixelColor(i, j, QColor(255 - arr2[idx], 255 - arr2[idx], 255 - arr2[idx]));
        }
    }

    //    bg_ = new QImage(8, 8, QImage::Format_RGBA8888);
    //    bg_->fill(QColor(0, 0, 0, 0));
    //    std::vector<std::string> fills{
    //            "XXXXXXXX",  //
    //            "XXXXXXXX",  //
    //            "X  XX  X",  //
    //            "X  XX  X",  //
    //            "XXX  XXX",  //
    //            "XX    XX",  //
    //            "XX    XX",  //
    //            "XX XX XX",  //
    //    };
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = 0; j < 8; j++) {
    //            if (fills[i][j] == ' ') bg_->setPixelColor(i, j, QColor(31, 138, 112, 160));
    //        }
    //    }
}

#include <bitset>


void cfg::initConfig() {
    qInfo() << "Current working directory: " << QDir::currentPath();
    qInfo() << "Configuration file path: " << CONFIG_FILE_PATH.c_str();
    try {
        nlohmann::json j;
        std::ifstream f(CONFIG_FILE_PATH);
        if (!f.is_open()) {
            qCritical() << "Can not find config file.";
        } else {
            f >> j;
            cfg::SHADOW_LEVEL = j["terrain_shadow_level"].get<int>();
            cfg::COLOR_THEME = j["theme"].get<std::string>();
            REGION_CACHE_SIZE = j["region_cache_size"].get<int>();
            EMPTY_REGION_CACHE_SIZE = j["empty_region_cache_size"].get<int>();
            THREAD_NUM = j["background_thread_number"].get<int>();
            cfg::MINIMUM_SCALE_LEVEL = j["minimum_scale_level"].get<int>();
            cfg::MAXIMUM_SCALE_LEVEL = j["maximum_scale_level"].get<int>();
            cfg::ZOOM_SPEED = j["zoom_speed"].get<float>();
            cfg::FONT_SIZE = j["font_size"].get<int>();
            cfg::FANCY_TERRAIN_RENDER = j["fancy_terrain_render"].get<bool>();
            cfg::LOAD_GLOBAL_DATA = j["load_global_data"].get<bool>();
            cfg::OPEN_NBT_EDITOR_ONLY = j["nbt_editor_mode"].get<bool>();
        }

    } catch (std::exception &e) {
        qCritical() << "Invalid config file format" << e.what();
    }
    if (THREAD_NUM < 1) {
        THREAD_NUM = 2;
        qWarning() << "Invalid background thread number, reset it to default(2)";
    }

    qInfo() << "Read config finished, here are the details";
    qInfo() << "- Shadow level: " << cfg::SHADOW_LEVEL;
    qInfo() << "- Theme: " << COLOR_THEME.c_str();
    qInfo() << "- Region cache size: " << REGION_CACHE_SIZE;
    qInfo() << "- Empty region cache size: " << EMPTY_REGION_CACHE_SIZE;
    qInfo() << "- Background thread number: " << THREAD_NUM;
    qInfo() << "- Minimum scale level: " << MINIMUM_SCALE_LEVEL;
    qInfo() << "- Maximum thread number: " << MAXIMUM_SCALE_LEVEL;
    qInfo() << "- Font size: " << FONT_SIZE;
    qInfo() << "- Zoom speed: " << ZOOM_SPEED;
    qInfo() << "- Load global data: " << cfg::LOAD_GLOBAL_DATA;
    qInfo() << "- Render region width: " << cfg::RW;
    qInfo() << "- NBT editor mode:" << cfg::OPEN_NBT_EDITOR_ONLY;
    qInfo() << "Reading biome and block color table...";
    initColorTable();
}

QString cfg::VERSION_STRING() {
    return QString(cfg::SOFTWARE_NAME.c_str()) + " " + QString(cfg::SOFTWARE_VERSION.c_str());
}

//QImage *cfg::EMPTY_REGION_IMAGE() { return transparent_region_img_; }


QImage *cfg::NULL_REGION_IMAGE() { return null_region_image_; }

QImage cfg::CREATE_REGION_IMG(const std::bitset<cfg::RW * cfg::RW> &bitmap) {
    auto res = QImage(cfg::RW << 4, cfg::RW << 4, QImage::Format_RGB888);
    const int BW = cfg::RW << 4;
    for (int i = 0; i < BW; i++) {
        for (int j = 0; j < BW; j++) {
            const int arr[2]{20, 40};
            const int idx = (i / (cfg::RW << 3) + j / (cfg::RW << 3)) % 2;
            if (bitmap[(i >> 4) * cfg::RW + (j >> 4)] && (!cfg::transparent_void)) {
                res.setPixelColor(i, j, QColor(255 - arr[idx], 255 - arr[idx], 255 - arr[idx]));
            } else {
                res.setPixelColor(i, j, QColor(arr[idx], arr[idx], arr[idx]));
            }
        }
    }
    return res;
}

QImage *cfg::UNLOADED_REGION_IMAGE() { return unloaded_region_image_; }
