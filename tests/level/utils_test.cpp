#include <gtest/gtest.h>

#include "level/utils.h"

TEST(Utils, Logger) {
    int a = 1;
    BL_ERROR("This is a error message with a  = %d", a);
    BL_LOGGER("This is a logger message with a  = %d", a);
}