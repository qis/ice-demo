#include <ice/application.hpp>
#include <ice/ui/context.hpp>
#include <ice/window.hpp>

class window : public ice::window {
public:
  void on_create() noexcept override
  {
    // Start rendering native UI.
    if (const auto e = start()) {
      ice::application::message("Could not initialize renderer.\n{}: {} ({})", e.type(), e, e.code());
      destroy();
      return;
    }
    // TODO: Set window title and icon.
    // TODO: Move or resize window.
    // Show window.
    if (const auto e = show()) {
      ice::application::message("Could not show application window.\n{}: {} ({})", e.type(), e, e.code());
      destroy();
      return;
    }
  }

  void on_render(ice::ui::context& context) noexcept override
  {
    // Basic nuklear demo.
    enum class option { easy, hard };
    static option op = option::easy;
    static int property = 1;
    context.layout_row_static(30, 300, 1);
    if (context.button_label("Unicode（ユニコード）")) {
      fprintf(stdout, "button pressed\n");
      fflush(stdout);
    }
    context.layout_row_dynamic(30, 2);
    if (context.option_label("Easy", op == option::easy)) {
      op = option::easy;
    }
    if (context.option_label("Hard", op == option::hard)) {
      op = option::hard;
    }
    context.layout_row_dynamic(22, 1);
    context.property_int("Scaling:", 1, &property, 16, 1, 0.3f);
  }

  void on_render(ice::vk::context& context) noexcept override
  {}

  void on_destroy() noexcept override
  {
    // Can't stop from closing, but can do cleanup.
  }

  void on_close() noexcept override
  {
    // Do not call destroy() to prevent from closing.
    destroy();
  }
};

int run() noexcept
{
  window window;
  if (const auto e = window.create()) {
    ice::application::message("Could not create application window.\n{}: {} ({})", e.type(), e, e.code());
    return EXIT_FAILURE;
  }
  if (const auto e = window.run()) {
    ice::application::message("Application exited with an error.\n{}: {} ({})", e.type(), e, e.code());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

#ifdef _WIN32
// NOLINTNEXTLINE(readability-identifier-naming)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int)
{
  ice::application::initialize(hInstance, lpCmdLine);
  return run();
}
#else
int main(int argc, char* argv[])
{
  ice::application::initialize(argc, argv);
  return run();
}
#endif
