#include "tessel.hpp"

void focus_view(View *v) {
	if (!v)
		return;
	Server *s = v->server;
	wlr_surface *surface = v->toplevel->base->surface;
	wlr_surface *prev = s->seat->keyboard_state.focused_surface;
	if (prev == surface)
		return;
	if (prev) {
		wlr_xdg_toplevel *pt = wlr_xdg_toplevel_try_from_wlr_surface(prev);
		if (pt)
			wlr_xdg_toplevel_set_activated(pt, false);
	}
	wl_list_remove(&v->link);
	wl_list_insert(&s->views, &v->link);
	wlr_xdg_toplevel_set_activated(v->toplevel, true);

	wlr_keyboard *kb = wlr_seat_get_keyboard(s->seat);
	if (kb)
		wlr_seat_keyboard_notify_enter(s->seat, surface, kb->keycodes,
			kb->num_keycodes, &kb->modifiers);
	else
		wlr_seat_keyboard_notify_enter(s->seat, surface, NULL, 0, NULL);
}

static void view_map(wl_listener *l, void *data) {
	View *v = wl_container_of(l, v, map);
	wl_list_insert(&v->server->views, &v->link);
	arrange(v->server);
	wlr_log(WLR_INFO, "map %s -> %dx%d+%d+%d (%d views)",
		v->toplevel->title ? v->toplevel->title : "?",
		v->geo.width, v->geo.height, v->geo.x, v->geo.y,
		wl_list_length(&v->server->views));
}

static void view_unmap(wl_listener *l, void *data) {
	View *v = wl_container_of(l, v, unmap);
	wl_list_remove(&v->link);
	arrange(v->server);
}

static void view_commit(wl_listener *l, void *data) {
	View *v = wl_container_of(l, v, commit);
	if (v->toplevel->base->initial_commit)
		wlr_xdg_toplevel_set_size(v->toplevel, 0, 0);
}

static void view_destroy(wl_listener *l, void *data) {
	View *v = wl_container_of(l, v, destroy);
	wl_list_remove(&v->map.link);
	wl_list_remove(&v->unmap.link);
	wl_list_remove(&v->commit.link);
	wl_list_remove(&v->destroy.link);
	delete v;
}

void new_xdg_toplevel(wl_listener *l, void *data) {
	Server *s = wl_container_of(l, s, new_xdg_toplevel);
	auto *toplevel = static_cast<wlr_xdg_toplevel *>(data);

	View *v = new View{};
	v->server = s;
	v->toplevel = toplevel;
	v->tree = wlr_scene_xdg_surface_create(&s->scene->tree, toplevel->base);
	v->tree->node.data = v;
	toplevel->base->data = v->tree;

	v->map.notify = view_map;
	wl_signal_add(&toplevel->base->surface->events.map, &v->map);
	v->unmap.notify = view_unmap;
	wl_signal_add(&toplevel->base->surface->events.unmap, &v->unmap);
	v->commit.notify = view_commit;
	wl_signal_add(&toplevel->base->surface->events.commit, &v->commit);
	v->destroy.notify = view_destroy;
	wl_signal_add(&toplevel->events.destroy, &v->destroy);
}
