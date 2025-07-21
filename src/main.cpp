#include "tessel.hpp"
#include <cstdlib>

int main(void) {
	wlr_log_init(WLR_INFO, NULL);

	Server s{};
	s.master_ratio = 0.58;
	s.gap = 8;
	s.display = wl_display_create();
	wl_event_loop *loop = wl_display_get_event_loop(s.display);

	s.backend = wlr_backend_autocreate(loop, NULL);
	if (!s.backend) {
		wlr_log(WLR_ERROR, "no backend");
		return 1;
	}

	s.renderer = wlr_renderer_autocreate(s.backend);
	wlr_renderer_init_wl_display(s.renderer, s.display);
	s.allocator = wlr_allocator_autocreate(s.backend, s.renderer);

	wlr_compositor_create(s.display, 5, s.renderer);
	wlr_subcompositor_create(s.display);
	wlr_data_device_manager_create(s.display);

	s.output_layout = wlr_output_layout_create(s.display);
	wl_list_init(&s.outputs);
	s.new_output.notify = new_output;
	wl_signal_add(&s.backend->events.new_output, &s.new_output);

	s.scene = wlr_scene_create();
	s.scene_layout = wlr_scene_attach_output_layout(s.scene, s.output_layout);

	wl_list_init(&s.views);
	s.xdg_shell = wlr_xdg_shell_create(s.display, 3);
	s.new_xdg_toplevel.notify = new_xdg_toplevel;
	wl_signal_add(&s.xdg_shell->events.new_toplevel, &s.new_xdg_toplevel);
	s.new_xdg_popup.notify = new_xdg_popup;
	wl_signal_add(&s.xdg_shell->events.new_popup, &s.new_xdg_popup);

	s.cursor = wlr_cursor_create();
	wlr_cursor_attach_output_layout(s.cursor, s.output_layout);
	s.cursor_mgr = wlr_xcursor_manager_create(NULL, 24);
	s.cursor_motion.notify = cursor_motion;
	wl_signal_add(&s.cursor->events.motion, &s.cursor_motion);
	s.cursor_motion_absolute.notify = cursor_motion_absolute;
	wl_signal_add(&s.cursor->events.motion_absolute, &s.cursor_motion_absolute);
	s.cursor_button.notify = cursor_button;
	wl_signal_add(&s.cursor->events.button, &s.cursor_button);
	s.cursor_axis.notify = cursor_axis;
	wl_signal_add(&s.cursor->events.axis, &s.cursor_axis);
	s.cursor_frame.notify = cursor_frame;
	wl_signal_add(&s.cursor->events.frame, &s.cursor_frame);

	wl_list_init(&s.keyboards);
	s.new_input.notify = new_input;
	wl_signal_add(&s.backend->events.new_input, &s.new_input);
	s.seat = wlr_seat_create(s.display, "seat0");
	s.request_set_selection.notify = request_set_selection;
	wl_signal_add(&s.seat->events.request_set_selection, &s.request_set_selection);
	s.request_cursor.notify = request_cursor;
	wl_signal_add(&s.seat->events.request_set_cursor, &s.request_cursor);
	wlr_seat_set_capabilities(s.seat, WL_SEAT_CAPABILITY_POINTER);

	const char *socket = wl_display_add_socket_auto(s.display);
	if (!socket) {
		wlr_log(WLR_ERROR, "no socket");
		return 1;
	}

	if (!wlr_backend_start(s.backend)) {
		wlr_log(WLR_ERROR, "backend start failed");
		return 1;
	}

	setenv("WAYLAND_DISPLAY", socket, true);
	wlr_log(WLR_INFO, "tessel up on %s", socket);
	wl_display_run(s.display);

	wl_display_destroy_clients(s.display);
	wlr_scene_node_destroy(&s.scene->tree.node);
	wlr_allocator_destroy(s.allocator);
	wlr_renderer_destroy(s.renderer);
	wlr_backend_destroy(s.backend);
	wl_display_destroy(s.display);
	return 0;
}
