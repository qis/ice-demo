#include "utility.hpp"

#ifdef _WIN32
#  include <windows.h>
#else
#  include <pthread.h>
#endif

namespace ice {

thread_local_storage::thread_local_storage() noexcept
{
#ifdef _WIN32
  handle_ = static_cast<std::uintptr_t>(::TlsAlloc());
  ICE_ASSERT(handle_ != invalid_handle_value);
#else
  auto key = static_cast<pthread_key_t>(invalid_handle_value);
  [[maybe_unused]] const auto ec = pthread_key_create(&key, nullptr);
  handle_ = static_cast<std::uintptr_t>(key);
  ICE_ASSERT(ec == 0);
#endif
}

thread_local_storage::~thread_local_storage()
{
#if _WIN32
  TlsFree(static_cast<DWORD>(handle_));
#else
  pthread_key_delete(static_cast<pthread_key_t>(handle_));
#endif
}

bool thread_local_storage::set(void* value) noexcept  // NOLINT(readability-make-member-function-const)
{
#if _WIN32
  return TlsSetValue(static_cast<DWORD>(handle_), value) != 0;
#else
  return pthread_setspecific(static_cast<pthread_key_t>(handle_), value) == 0;
#endif
}

void* thread_local_storage::get() const noexcept
{
#if _WIN32
  return TlsGetValue(static_cast<DWORD>(handle_));
#else
  return pthread_getspecific(static_cast<pthread_key_t>(handle_));
#endif
}

}  // namespace ice
