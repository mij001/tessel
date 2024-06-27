#include "tessel.hpp"
#include <cstdlib>
#include <unistd.h>
static void kbd_modifiers(wl_listener *l, void *data) {
	Keyboard *k = wl_container_of(l, k, modifiers);
	wlr_seat_set_keyboard(k->server->seat, k->kbd);
	wlr_seat_keyboard_notify_modifiers(k->server->seat, &k->kbd->modifiers);
}

static void kbd_key(wl_listener *l, void *data) {
	Keyboard *k = wl_container_of(l, k, key);
	auto *e = static_cast<wlr_keyboard_key_event *>(data);
	wlr_seat_set_keyboard(k->server->seat, k->kbd);
	wlr_seat_keyboard_notify_key(k->server->seat, e->time_msec, e->keycode, e->state);
}
