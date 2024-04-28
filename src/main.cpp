#include "tessel.hpp"
#include <cstdlib>

int main(void) {
	wlr_log_init(WLR_INFO, NULL);

	Server s{};
	s.display = wl_display_create();
	wl_event_loop *loop = wl_display_get_event_loop(s.display);

	s.backend = wlr_backend_autocreate(loop, NULL);
	if (!s.backend) {
		wlr_log(WLR_ERROR, "no backend");
		return 1;
	}

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
	wlr_backend_destroy(s.backend);
	wl_display_destroy(s.display);
	return 0;
}
