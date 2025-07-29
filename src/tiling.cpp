#include "tessel.hpp"

static void place(View *v, int x, int y, int w, int h) {
	v->geo = {x, y, w, h};
	wlr_scene_node_set_position(&v->tree->node, x, y);
	wlr_xdg_toplevel_set_size(v->toplevel, w, h);
	wlr_xdg_toplevel_set_tiled(v->toplevel,
		WLR_EDGE_TOP | WLR_EDGE_BOTTOM | WLR_EDGE_LEFT | WLR_EDGE_RIGHT);
}

void arrange(Server *s) {
	if (wl_list_empty(&s->outputs))
		return;
	Output *o = wl_container_of(s->outputs.next, o, link);
	wlr_box area;
	wlr_output_layout_get_box(s->output_layout, o->out, &area);
	View *vv;

	int n = 0;
	wl_list_for_each(vv, &s->views, link)
		if (!vv->floating && !vv->fullscreen)
			n++;
	if (n == 0)
		return;

	int g = s->gap;
	int x = area.x + g, y = area.y + g;
	int w = area.width - 2 * g, h = area.height - 2 * g;

	if (n == 1) {
		wl_list_for_each(vv, &s->views, link)
			if (!vv->floating) {
				place(vv, x, y, w, h);
				break;
			}
		return;
	}

	int mw = (int)(w * s->master_ratio) - g / 2;
	int sw = w - mw - g;
	int stack = n - 1;
	int i = 0;
	View *v;
	wl_list_for_each(v, &s->views, link) {
		if (v->floating)
			continue;
		if (i == 0) {
			place(v, x, y, mw, h);
		} else {
			int sh = (h - (stack - 1) * g) / stack;
			int sy = y + (i - 1) * (sh + g);
			if (i == stack)
				sh = y + h - sy;
			place(v, x + mw + g, sy, sw, sh);
		}
		i++;
	}
}
