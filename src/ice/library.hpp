#pragma once
#include <ice/path.hpp>
#include <ice/result.hpp>

namespace ice {

// ================================================================================================
// library
// ================================================================================================

class ICE_API library {
public:
  library() noexcept = default;
  library(library&& other) noexcept;
  library(const library& other) = delete;
  library& operator=(library&& other) noexcept;
  library& operator=(const library& other) = delete;

  library(const std::string& name) noexcept;
  library(const std::filesystem::path& path, const std::string& name) noexcept;

  virtual ~library();

  ice::error open() noexcept;
  ice::error close() noexcept;

  void* get(const char* symbol) noexcept;

  template <typename T>
  constexpr auto get(const char* symbol) noexcept requires(std::is_pointer_v<T>)
  {
    return reinterpret_cast<T>(get(symbol));
  }

  void* handle() const noexcept
  {
    return handle_;
  }

  std::filesystem::path path() const noexcept
  {
    return path_;
  }

private:
  void* handle_{ nullptr };
  std::filesystem::path path_;
};

}  // namespace ice
