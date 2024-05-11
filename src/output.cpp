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
	wl_list_remove(&o->link);
	delete o;
}

void new_output(wl_listener *l, void *data) {
	Server *s = wl_container_of(l, s, new_output);
	auto *wo = static_cast<wlr_output *>(data);

	wlr_output_init_render(wo, s->allocator, s->renderer);

	wlr_output_state st;
	wlr_output_state_init(&st);
	wlr_output_state_set_enabled(&st, true);
	if (wlr_output_mode *m = wlr_output_preferred_mode(wo))
		wlr_output_state_set_mode(&st, m);
	wlr_output_commit_state(wo, &st);
	wlr_output_state_finish(&st);

	Output *o = new Output{};
	o->server = s;
	o->out = wo;
	o->frame.notify = output_frame;
	wl_signal_add(&wo->events.frame, &o->frame);
	o->destroy.notify = output_destroy;
	wl_signal_add(&wo->events.destroy, &o->destroy);
	wl_list_insert(&s->outputs, &o->link);

}
