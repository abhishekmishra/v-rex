/*
 * ps_window.c
 *
 *  Created on: 25-Dec-2018
 *      Author: abhishek
 */
#include <stdlib.h>
#include <log.h>
#include "ps_window.h"
#include "vrex_util.h"

/**
 * Create a new ps_window
 *
 * \param parent window
 * \param ps_w handle to populate with created widget.
 * \return error code
 */
int make_ps_window(Widget parent, Widget* ps_w) {
	(*ps_w) = XtVaCreateManagedWidget("ps_w", xbaeMatrixWidgetClass, parent,
	XmNcolumns, 8,
	XmNrows, 0,
	XmNvisibleColumns, 8,
	XmNheight, 200,
	XmNshadowType, XmSHADOW_ETCHED_OUT,
	XmNcellShadowThickness, 0,
	XmNtextShadowThickness, 0,
	XmNcellHighlightThickness, 1,
	XmNcolumnLabelColor, 0x800000,
	NULL);
	return 0;
}

/**
 * Set the docker container id for the table
 *
 * \param ps_w the window handle
 * \param ctx already initialized docker_context
 * \param id container id
 * \return error code
 */
int set_ps_window_docker_id(Widget ps_w, vrex_context* vrex, char* id) {
	docker_log_debug("Updating ps for id %s", id);
	docker_result* result;
	docker_container_ps* ps;
	docker_process_list_container(vrex->d_ctx, &result, &ps, id, NULL);
	vrex->handle_error(vrex, result);
	docker_log_debug("num titles %d num processes %d",
			array_list_length(ps->titles), array_list_length(ps->processes));
	for (int i = 0; i < array_list_length(ps->titles); i++) {
//		docker_log_info("Title is %s at %d", array_list_get_idx(ps->titles, i),
//				i);
		add_column(ps_w, array_list_get_idx(ps->titles, i), i, 10);
	}

	int num_rows = XbaeMatrixNumRows(ps_w);
	if (num_rows > 0) {
		XbaeMatrixDeleteRows(ps_w, 0, num_rows);
	}

	for (int i = 0; i < array_list_length(ps->processes); i++) {
		struct array_list* process = array_list_get_idx(ps->processes, i);
		char** rows;
		rows = (char**) XtCalloc(array_list_length(process), sizeof(String));
		for (int j = 0; j < array_list_length(process); j++) {
//			docker_log_info("Title is %s at %d", array_list_get_idx(process, j),
//					j);
			rows[j] = array_list_get_idx(process, j);
		}
		XbaeMatrixAddRows(ps_w, XbaeMatrixNumRows(ps_w), rows, NULL, NULL, 1);
		free(rows);
	}
	return 0;
}