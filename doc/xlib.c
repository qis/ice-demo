// gcc xlib.c -lX11
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
int main() {
  Display* display = XOpenDisplay(NULL);
  int screen = DefaultScreen(display);

  for (int i = 2; i > 0; i--) {
    Window window = XCreateSimpleWindow(display,
      RootWindow(display, screen), 0, 0, i * 100, i * 100, 1,
      BlackPixel(display, screen), BlackPixel(display, screen));
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);
  }

  XEvent event;
  while (1) {
    XNextEvent(display, &event);
  }
}
