#pragma once

#include <functional>
#include <map>
#include <optional>
#include <string>
#include <tuple>

#include "level/bedrock_key.h"
#include "level/chunk.h"
#include "level/global.h"
#include "level/level_dat.h"
#include "leveldb/db.h"

namespace bl {

class bedrock_level {
public:
    bedrock_level();

    /*
     * Returns whether the level has been opened normally
     */
    [[nodiscard]] bool is_open() const { return this->is_open_; }

    /**
     * Open a level
     * @param root level root, that is, the directory where level.dat is located
     * @return
     */
    bool open(const std::string& root);

    // Returns a reference to db
    leveldb::DB*& db() { return this->db_; }

    void close();

    chunk*           get_chunk(const chunk_pos& cp, bool fast_load = false);
    general_kv_nbts& player_data() { return this->player_data_; }

    bl::village_data& village_data() { return this->village_data_; }

    bl::general_kv_nbts& map_item_data() { return this->map_item_data_; }

    bl::general_kv_nbts& other_item_data() { return this->other_data_; }

    /**
     * Get the number of cached blocks
     * @return
     */
    [[nodiscard]] inline size_t cached_chunk_size() const { return this->chunk_data_cache_.size(); };

    /**
     * Get the object wrapper of the level.dat file
     * @return
     */
    level_dat& dat() { return this->dat_; }

    /**
     * When enabled, Level will cache the read block data, with no capacity limit (can be changed to cache later)
     * When closed, the content will be cleared
     * @param enable
     */
    void set_cache(bool enable);

    actor* load_actor(const std::string& raw_uid);

    ~bedrock_level();

    void foreach_global_keys(const std::function<void(const std::string&, const std::string&)>& f);

    void load_global_data();
    // write
    bool remove_chunk(const chunk_pos& cp);

    [[nodiscard]] std::string root_path() const { return this->root_name_; }
    static const std::string  LEVEL_DATA;
    static const std::string  LEVEL_DB;

private:
    /**
     * Read all data of a chunk from the database
     * @param cp ChunkPos
     * @return
     */
    chunk* read_chunk_from_db(const bl::chunk_pos& cp, bool fast_load);

    void clear_cache();

private:
    bool is_open_{false};

    level_dat dat_;

    leveldb::DB* db_{nullptr};

    std::string root_name_;

    std::map<chunk_pos, chunk*> chunk_data_cache_;

private:
    bool read_db();

    bool             enable_cache_{false};
    leveldb::Options options_{};
    // global data
    bl::village_data    village_data_;
    bl::general_kv_nbts player_data_;
    bl::general_kv_nbts map_item_data_;
    bl::general_kv_nbts other_data_;
};

} // namespace bl
