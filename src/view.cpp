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
	wlr_scene_node_raise_to_top(&v->tree->node);
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

View *first_view(Server *s) {
	if (wl_list_empty(&s->views))
		return NULL;
	View *v = wl_container_of(s->views.next, v, link);
	return v;
}

static void view_map(wl_listener *l, void *data) {
	View *v = wl_container_of(l, v, map);
	wl_list_insert(&v->server->views, &v->link);
	focus_view(v);
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
	focus_view(first_view(v->server));
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

struct Popup {
	wlr_xdg_popup *popup;
	wl_listener commit;
	wl_listener destroy;
};

static void popup_commit(wl_listener *l, void *data) {
	Popup *p = wl_container_of(l, p, commit);
	if (p->popup->base->initial_commit)
		wlr_xdg_surface_schedule_configure(p->popup->base);
}

static void popup_destroy(wl_listener *l, void *data) {
	Popup *p = wl_container_of(l, p, destroy);
	wl_list_remove(&p->commit.link);
	wl_list_remove(&p->destroy.link);
	delete p;
}

void new_xdg_popup(wl_listener *l, void *data) {
	auto *xp = static_cast<wlr_xdg_popup *>(data);
	wlr_xdg_surface *parent = wlr_xdg_surface_try_from_wlr_surface(xp->parent);
	if (!parent)
		return;
	auto *parent_tree = static_cast<wlr_scene_tree *>(parent->data);
	xp->base->data = wlr_scene_xdg_surface_create(parent_tree, xp->base);

	Popup *p = new Popup{};
	p->popup = xp;
	p->commit.notify = popup_commit;
	wl_signal_add(&xp->base->surface->events.commit, &p->commit);
	p->destroy.notify = popup_destroy;
	wl_signal_add(&xp->events.destroy, &p->destroy);
}

View *view_at(Server *s, double lx, double ly, wlr_surface **surface,
		double *sx, double *sy) {
	wlr_scene_node *node = wlr_scene_node_at(&s->scene->tree.node, lx, ly, sx, sy);
	if (!node || node->type != WLR_SCENE_NODE_BUFFER)
		return NULL;
	wlr_scene_buffer *sb = wlr_scene_buffer_from_node(node);
	wlr_scene_surface *ss = wlr_scene_surface_try_from_buffer(sb);
	if (!ss)
		return NULL;
	*surface = ss->surface;
	wlr_scene_tree *tree = node->parent;
	while (tree && !tree->node.data)
		tree = tree->node.parent;
	return tree ? static_cast<View *>(tree->node.data) : NULL;
}
