// gcc main.c -lxcb
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  xcb_connection_t* connection = xcb_connect(NULL, NULL);
  xcb_screen_t* screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
  uint32_t value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  uint32_t value_list[2] = { screen->black_pixel, XCB_EVENT_MASK_EXPOSURE };

  xcb_window_t id[2];
  xcb_intern_atom_reply_t* wm_delete_window[2];

  for (int i = 2; i > 0; i--) {
    id[i] = xcb_generate_id(connection);
    xcb_create_window(
      connection, screen->root_depth, id[i], screen->root,
      0, 0, i * 100, i * 100, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
      screen->root_visual, value_mask, value_list);

    xcb_intern_atom_cookie_t wm_protocols_cookie = xcb_intern_atom(connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* wm_protocols = xcb_intern_atom_reply(connection, wm_protocols_cookie, 0);
    xcb_intern_atom_cookie_t wm_delete_window_cookie = xcb_intern_atom(connection, 0, 16, "WM_DELETE_WINDOW");
    wm_delete_window[i] = xcb_intern_atom_reply(connection, wm_delete_window_cookie, 0);
    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, id[i], wm_protocols->atom, 4, 32, 1, &wm_delete_window[i]->atom);
    xcb_flush(connection);
    free(wm_protocols);

    xcb_map_window(connection, id[i]);
    xcb_flush(connection);
  }

  while (1) {
    xcb_generic_event_t* event = xcb_wait_for_event(connection);
    if (!event) {
      const int code = xcb_connection_has_error(connection);
      if (code) {
        fprintf(stderr, "connection error: %d\n", code);
        return EXIT_FAILURE;
      }
      break;
    }
    if ((event->response_type & ~0x80) == XCB_CLIENT_MESSAGE) {
      xcb_client_message_event_t* e = (xcb_client_message_event_t*)event;
      for (size_t i = 0; i < 2; i++) {
        if (e->data.data32[0] == wm_delete_window[i]->atom) {
          xcb_destroy_window(connection, id[i]);
        }
      }
    }
    free(event);
  }
}
