#pragma once

#include "config.h"
#include "level/bedrock_key.h"
#include "level/bedrock_level.h"
#include "level/palette.h"
#include "ui/renderfilterdialog.h"

#include <QCache>
#include <QFuture>
#include <QRunnable>
#include <QThreadPool>

class AsyncLevelLoader;
namespace bl {

inline uint qHash(const bl::chunk_pos& key, uint seed) {
    uint hash = 3241;
    hash      = 3457689L * hash + key.x;
    hash      = 8734625L * hash + key.z;
    hash      = 2873465L * hash + key.dim;
    return hash;
}
} // namespace bl

struct BlockTipsInfo {
    std::string block_name{"?"};
    bl::biome   biome{bl::none};
    int16_t     height{-128};
};

struct ChunkRegion {
    ~ChunkRegion();

    std::array<std::array<BlockTipsInfo, cfg::RW << 4>, cfg::RW << 4>
                                                       tips_info_{};
    std::bitset<cfg::RW * cfg::RW>                     chunk_bit_map_;
    QImage                                             terrain_bake_image_;
    QImage                                             biome_bake_image_;
    QImage                                             height_bake_image_;
    bool                                               valid{false};
    std::unordered_map<QImage*, std::vector<bl::vec3>> actors_;
    std::vector<bl::hardcoded_spawn_area>              HSAs_;
};

struct RegionTimer {
    std::deque<int64_t> values;

    [[nodiscard]] int64_t mean() const;

    void push(int64_t value);
};

template <typename T>
class TaskBuffer {
public:
    bool contains(const T& t) {
        bool exist{false};
        {
            std::lock_guard<std::mutex> lk(this->mu_);
            exist = this->buffer_.count(t) > 0;
        }
        return exist;
    }

    size_t size() {
        size_t sz = 0;
        {
            std::lock_guard<std::mutex> kl(this->mu_);
            sz = this->buffer_.size();
        }
        return sz;
    }

    void clear() {
        std::lock_guard<std::mutex> kl(this->mu_);
        this->buffer_.clear();
    }

    void add(const T& t) {
        std::lock_guard<std::mutex> kl(this->mu_);
        this->buffer_.insert(t);
    }

    void remove(const T& t) {
        std::lock_guard<std::mutex> kl(this->mu_);
        this->buffer_.erase(t);
    }

    // bl::bedrock_level *level() { return this->level_; }

    std::mutex            mu_;
    std::unordered_set<T> buffer_;
};

class LoadRegionTask : public QObject, public QRunnable {
    Q_OBJECT

public:
    LoadRegionTask(
        bl::bedrock_level*   level,
        const bl::chunk_pos& pos,
        const MapFilter*     filter
    )
    : QRunnable(),
      level_(level),
      pos_(pos),
      filter_(filter) {}

    void run() override;

signals:

    void finish(
        int          x,
        int          z,
        int          dim,
        ChunkRegion* region,
        long long    load_time,
        long long    render_time,
        bl::chunk**  chunks
    ); // NO_LINT

private:
    bl::bedrock_level* level_;
    region_pos         pos_;
    const MapFilter*   filter_;
};

// class FreeMemoryTask : public QObject, public QRunnable {
//
// public:
//
//     explicit FreeMemoryTask(bl::chunk **chunks) : chunks_(chunks) {}
//
//     void run() override;
//
// private:
//     bl::chunk **chunks_;
// };

class AsyncLevelLoader : public QObject {
    Q_OBJECT

public:
    AsyncLevelLoader();

    void clearAllCache();

    bool open(const std::string& path);

    void close();

    bl::bedrock_level& level() { return this->level_; }

    inline bool isOpen() const { return this->loaded_; }

    void setFilter(const MapFilter& f) { this->map_filter_ = f; }

public:
    /*region cache*/
    QImage* bakedBiomeImage(const region_pos& rp);

    QImage* bakedTerrainImage(const region_pos& rp);

    QImage* bakedHeightImage(const region_pos& rp);

    QImage* bakedSlimeChunkImage(const region_pos& rp);

    BlockTipsInfo getBlockTips(const bl::block_pos& p, int dim);

    std::unordered_map<QImage*, std::vector<bl::vec3>>
    getActorList(const region_pos& rp);

    std::vector<bl::hardcoded_spawn_area> getHSAs(const region_pos& rp);

    /*Modify*/
    bl::chunk* getChunkDirect(const bl::chunk_pos& p);

    // modify
    QFuture<bool>
    dropChunk(const bl::chunk_pos& min, const ::bl::chunk_pos& max);

    /**
     * Batch modify database
     * For each key and value in @modifies
     * If the value is empty, delete the key from the db
     * If the value is not empty, update the key
     * Note that the bedrock_level object in memory will not be updated
     * @param modifies
     * @return
     */
    bool
    modifyDBGlobal(const std::unordered_map<std::string, std::string>& modifies
    );

    bool modifyLeveldat(bl::palette::compound_tag* nbt);

    //    bool
    //    modifyPlayerList(const std::unordered_map<std::string,
    //    bl::palette::compound_tag *> &player_list);
    //
    //    bool modifyOtherItemList(const std::unordered_map<std::string,
    //    bl::palette::compound_tag *> &item_list);
    //
    //    bool modifyVillageList(
    //            const std::unordered_map<std::string,
    //            std::array<bl::palette::compound_tag *, 4>> &village_list);

    bool
    modifyChunkBlockEntities(const bl::chunk_pos& cp, const std::string& raw);

    bool
    modifyChunkPendingTicks(const bl::chunk_pos& cp, const std::string& raw);

    bool modifyChunkActors(
        const bl::chunk_pos&           cp,
        bl::ChunkVersion               v,
        const std::vector<bl::actor*>& actors
    );

public:
    ~AsyncLevelLoader() override;

    std::vector<QString> debugInfo();

private:
    ChunkRegion* tryGetRegion(const region_pos& p, bool& empty);

private:
    std::atomic_bool                              loaded_{false};
    bl::bedrock_level                             level_{};
    TaskBuffer<region_pos>                        processing_;
    std::vector<QCache<region_pos, ChunkRegion>*> region_cache_;
    std::vector<QCache<region_pos, char>*>        invalid_cache_;
    // Mainly cache images, calculation is not the focus.
    QCache<region_pos, QImage>* slime_chunk_cache_;
    QThreadPool                 pool_;
    MapFilter                   map_filter_;
    RegionTimer                 region_load_timer_;
    RegionTimer                 region_render_timer_;
};
