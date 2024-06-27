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

static void kbd_destroy(wl_listener *l, void *data) {
	Keyboard *k = wl_container_of(l, k, destroy);
	wl_list_remove(&k->modifiers.link);
	wl_list_remove(&k->key.link);
	wl_list_remove(&k->destroy.link);
	wl_list_remove(&k->link);
	delete k;
}

static void new_keyboard(Server *s, wlr_input_device *device) {
	wlr_keyboard *wk = wlr_keyboard_from_input_device(device);
	Keyboard *k = new Keyboard{};
	k->server = s;
	k->kbd = wk;

	xkb_context *ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
	xkb_keymap *km = xkb_keymap_new_from_names(ctx, NULL, XKB_KEYMAP_COMPILE_NO_FLAGS);
	wlr_keyboard_set_keymap(wk, km);
	xkb_keymap_unref(km);
	xkb_context_unref(ctx);
	wlr_keyboard_set_repeat_info(wk, 25, 600);

	k->modifiers.notify = kbd_modifiers;
	wl_signal_add(&wk->events.modifiers, &k->modifiers);
	k->key.notify = kbd_key;
	wl_signal_add(&wk->events.key, &k->key);
	k->destroy.notify = kbd_destroy;
	wl_signal_add(&device->events.destroy, &k->destroy);

	wlr_seat_set_keyboard(s->seat, wk);
	wl_list_insert(&s->keyboards, &k->link);
}
