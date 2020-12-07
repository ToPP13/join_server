#include <string>
#include <vector>
#include <list>
#include <gtest/gtest.h>
#include "../include/lib.h"

TEST(gtest_version, gtest_version_basic)
{
    ASSERT_GT(version(), 0);
}
