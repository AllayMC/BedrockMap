--- origin:   https://github.com/xmake-io/xmake-repo/blob/master/packages/l/leveldb/xmake.lua
--- comments: using pmmp/leveldb

package('pmmp-leveldb')
    set_description('A minimalist fork of LevelDB compatible with ZLIB- and ZSTD-compressed DBs produced by Minecraft: Bedrock Edition.')
    set_license('BSD-3-Clause')

    add_urls('https://github.com/pmmp/leveldb.git')
    add_versions('2021.10.9', '1c7564468b41610da4f498430e795ca4de0931ff')

    add_deps('snappy')
    add_deps('zstd')
    add_deps('zlib')

    add_deps('cmake')
    on_install('macosx', 'linux', 'windows', 'mingw', function (package)
        local configs = {'-DLEVELDB_BUILD_TESTS=OFF', '-DLEVELDB_BUILD_BENCHMARKS=OFF'}
        table.insert(configs, '-DCMAKE_BUILD_TYPE=' .. (package:debug() and 'Debug' or 'Release'))
        table.insert(configs, '-DBUILD_SHARED_LIBS=' .. (package:config('shared') and 'ON' or 'OFF'))
        import('package.tools.cmake').install(package, configs)
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            void test() {
                leveldb::DB* db;
                leveldb::Options options;
                options.create_if_missing = true;
                leveldb::Status status = leveldb::DB::Open(options, "./test", &db);
            }
        ]]}, {configs = {languages = package:is_plat('windows') and 'c++14' or 'c++11'}, includes = 'leveldb/db.h'}))
    end)
