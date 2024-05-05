#include "tessel.hpp"
#include <ctime>

static void output_frame(wl_listener *l, void *data) {
	Output *o = wl_container_of(l, o, frame);
	wlr_scene_output *so = wlr_scene_get_scene_output(o->server->scene, o->out);
	wlr_scene_output_commit(so, NULL);

	timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	wlr_scene_output_send_frame_done(so, &now);
}

static void output_destroy(wl_listener *l, void *data) {
	Output *o = wl_container_of(l, o, destroy);
	wl_list_remove(&o->frame.link);
	wl_list_remove(&o->destroy.link);
	delete o;
}
