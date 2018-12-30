/*
 * v-rex: vrex_util.c
 * Created on: 27-Dec-2018
 *
 */

#include <Xbae/Matrix.h>
#include "vrex_util.h"
#include "log.h"

void add_column(Widget mw, char* name, int num, int width) {
	XbaeMatrixSetColumnLabel(mw, num, name);
	XbaeMatrixSetColumnWidth(mw, num, width);
}

vrex_err_t init_results_list(vrex_context* vrex) {
	if (vrex != NULL) {
		vrex->results = array_list_new((void (*)(void *)) &free_docker_result);
		return VREX_SUCCESS;
	}
	return VREX_E_UNKNOWN;
}

vrex_err_t results_list_add(vrex_context* vrex, docker_result* res) {
	if (vrex != NULL && vrex->results != NULL) {
		array_list_add(vrex->results, res);
		return VREX_SUCCESS;
	}
	return VREX_E_UNKNOWN;
}

int results_list_length(vrex_context* vrex) {
	if (vrex != NULL && vrex->results != NULL) {
		return array_list_length(vrex->results);
	}
	return 0;
}

docker_result* results_list_get_idx(vrex_context* vrex, int i) {
	return (docker_result*) array_list_get_idx(vrex->results, i);
}

Widget get_toolbar_w(vrex_context* vrex) {
	return XtNameToWidget(XtNameToWidget(*(vrex->main_w), "main_header_w"),
			"toolbar_w");
}

Widget get_container_list_w(vrex_context* vrex) {
	return XtNameToWidget(
			XtNameToWidget(
					XtNameToWidget(
							XtNameToWidget(*(vrex->main_w),
									"docker_server_frame_w"), "docker_server_w"),
					"container_list_toplevel"), "mw");
}
