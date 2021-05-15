#include <ice/utility.hpp>
#include <doctest/doctest.h>
#include <thread>

TEST_CASE("on scope exit")
{
  auto always_set = false;
  {
    const auto se = ice::on_scope_exit([&]() {
      always_set = true;
    });
    CHECK(!always_set);
  }
  CHECK(always_set);
}

TEST_CASE("thread local storage")
{
  std::string test0 = "test 0";
  std::string test1 = "test 1";

  ice::thread_local_storage storage;
  storage.set(&test0);

  CHECK(*reinterpret_cast<std::string*>(storage.get()) == "test 0");

  auto thread = std::thread([&]() {
    storage.set(&test1);
    CHECK(*reinterpret_cast<std::string*>(storage.get()) == "test 1");
  });
  thread.join();

  CHECK(*reinterpret_cast<std::string*>(storage.get()) == "test 0");
}
