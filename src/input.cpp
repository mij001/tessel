#include "tessel.hpp"
#include <cstdlib>
#include <unistd.h>
static void pointer_motion(Server *s, uint32_t time) {
	double sx, sy;
	wlr_surface *surface = NULL;
	view_at(s, s->cursor->x, s->cursor->y, &surface, &sx, &sy);
	if (!surface) {
		wlr_cursor_set_xcursor(s->cursor, s->cursor_mgr, "default");
		wlr_seat_pointer_notify_clear_focus(s->seat);
		return;
	}
	wlr_seat_pointer_notify_enter(s->seat, surface, sx, sy);
	wlr_seat_pointer_notify_motion(s->seat, time, sx, sy);
}

void cursor_motion(wl_listener *l, void *data) {
	Server *s = wl_container_of(l, s, cursor_motion);
	auto *e = static_cast<wlr_pointer_motion_event *>(data);
	wlr_cursor_move(s->cursor, &e->pointer->base, e->delta_x, e->delta_y);
	pointer_motion(s, e->time_msec);
}

void cursor_motion_absolute(wl_listener *l, void *data) {
	Server *s = wl_container_of(l, s, cursor_motion_absolute);
	auto *e = static_cast<wlr_pointer_motion_absolute_event *>(data);
	wlr_cursor_warp_absolute(s->cursor, &e->pointer->base, e->x, e->y);
	pointer_motion(s, e->time_msec);
}

void cursor_button(wl_listener *l, void *data) {
	Server *s = wl_container_of(l, s, cursor_button);
	auto *e = static_cast<wlr_pointer_button_event *>(data);
	if (e->state == WL_POINTER_BUTTON_STATE_RELEASED) {
		s->cursor_mode = Server::PASSTHROUGH;
		s->grabbed = NULL;
	} else {
		double sx, sy;
		wlr_surface *surface = NULL;
		View *v = view_at(s, s->cursor->x, s->cursor->y, &surface, &sx, &sy);
		wlr_keyboard *kb = wlr_seat_get_keyboard(s->seat);
		uint32_t mods = kb ? wlr_keyboard_get_modifiers(kb) : 0;
		if (v)
			focus_view(v);
	}
	wlr_seat_pointer_notify_button(s->seat, e->time_msec, e->button, e->state);
}

void cursor_axis(wl_listener *l, void *data) {
	Server *s = wl_container_of(l, s, cursor_axis);
	auto *e = static_cast<wlr_pointer_axis_event *>(data);
	wlr_seat_pointer_notify_axis(s->seat, e->time_msec, e->orientation,
		e->delta, e->delta_discrete, e->source, e->relative_direction);
}

void cursor_frame(wl_listener *l, void *data) {
	Server *s = wl_container_of(l, s, cursor_frame);
	wlr_seat_pointer_notify_frame(s->seat);
}

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

void new_input(wl_listener *l, void *data) {
	Server *s = wl_container_of(l, s, new_input);
	auto *device = static_cast<wlr_input_device *>(data);
	if (device->type == WLR_INPUT_DEVICE_KEYBOARD)
		new_keyboard(s, device);
	else if (device->type == WLR_INPUT_DEVICE_POINTER)
		wlr_cursor_attach_input_device(s->cursor, device);

	uint32_t caps = WL_SEAT_CAPABILITY_POINTER;
	if (!wl_list_empty(&s->keyboards))
		caps |= WL_SEAT_CAPABILITY_KEYBOARD;
	wlr_seat_set_capabilities(s->seat, caps);
}

void request_set_selection(wl_listener *l, void *data) {
	Server *s = wl_container_of(l, s, request_set_selection);
	auto *e = static_cast<wlr_seat_request_set_selection_event *>(data);
	wlr_seat_set_selection(s->seat, e->source, e->serial);
}
