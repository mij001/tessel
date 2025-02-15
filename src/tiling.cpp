#include "tessel.hpp"

static void place(View *v, int x, int y, int w, int h) {
	v->geo = {x, y, w, h};
	wlr_scene_node_set_position(&v->tree->node, x, y);
	wlr_xdg_toplevel_set_size(v->toplevel, w, h);
	wlr_xdg_toplevel_set_tiled(v->toplevel,
		WLR_EDGE_TOP | WLR_EDGE_BOTTOM | WLR_EDGE_LEFT | WLR_EDGE_RIGHT);
}
