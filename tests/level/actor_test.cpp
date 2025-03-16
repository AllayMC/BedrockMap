#include "level/actor.h"

#include <gtest/gtest.h>

#include "level/utils.h"

TEST(Actor, BaicRead) {
    auto data = bl::utils::read_file(R"(C:\Users\xhy\dev\bedrock-level\data\dumps\actors\144115188092633088.palette)");
    bl::actor actor;
    actor.load(data.data(), data.size());
    actor.dump();
}
