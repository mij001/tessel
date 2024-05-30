#include "tessel.hpp"


static void view_map(wl_listener *l, void *data) {
	View *v = wl_container_of(l, v, map);
	wl_list_insert(&v->server->views, &v->link);
	wlr_log(WLR_INFO, "map %s -> %dx%d+%d+%d (%d views)",
		v->toplevel->title ? v->toplevel->title : "?",
		v->geo.width, v->geo.height, v->geo.x, v->geo.y,
		wl_list_length(&v->server->views));
}

static void view_unmap(wl_listener *l, void *data) {
	View *v = wl_container_of(l, v, unmap);
	wl_list_remove(&v->link);
}

static void view_commit(wl_listener *l, void *data) {
	View *v = wl_container_of(l, v, commit);
	if (v->toplevel->base->initial_commit)
		wlr_xdg_toplevel_set_size(v->toplevel, 0, 0);
}
