/*
* v-rex: vrex_util.c
* Created on: 27-Dec-2018
*
*/

#include <Xbae/Matrix.h>
#include "vrex_util.h"

void handle_error(docker_result* res) {
	docker_simple_error_handler_log(res);
}

void add_column(Widget mw, char* name, int num, int width) {
	XbaeMatrixSetColumnLabel(mw, num, name);
	XbaeMatrixSetColumnWidth(mw, num, width);
}

