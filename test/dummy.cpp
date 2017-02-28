#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../external/doctest.h"

TEST_CASE("Dummy test case") {
  CHECK(true);
  WARN(true);
  REQUIRE(true);
}
