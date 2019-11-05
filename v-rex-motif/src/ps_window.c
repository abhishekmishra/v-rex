/*
 * ps_window.c
 *
 *  Created on: 25-Dec-2018
 *      Author: abhishek
 */
#include <stdlib.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>
#include "ps_window.h"

#include <docker_log.h>

#include "vrex_util.h"

/**
 * Create a new ps_window
 *
 * \param parent window
 * \param ps_w handle to populate with created widget.
 * \return error code
 */
int make_ps_window(Widget parent, Widget* ps_w) {
	Widget docker_process_list_frame_w, label;
	int n = 0;
	Arg args[10];

	n = 0;
	XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT);
	n++;
	XtSetArg(args[n], XmNmarginWidth, 1);
	n++;
	XtSetArg(args[n], XmNmarginHeight, 1);
	n++;
	docker_process_list_frame_w = XmCreateFrame(parent,
			"docker_process_list_frame_w", args, n);

	n = 0;
	XtSetArg(args[n], XmNframeChildType, XmFRAME_TITLE_CHILD);
	n++;
	XtSetArg(args[n], XmNchildVerticalAlignment, XmALIGNMENT_CENTER);
	n++;
	label = XmCreateLabelGadget(docker_process_list_frame_w, "Container PS",
			args, n);

	(*ps_w) = XtVaCreateManagedWidget("ps_w", xbaeMatrixWidgetClass,
			docker_process_list_frame_w,
			XmNcolumns, 8,
			XmNrows, 0,
			XmNvisibleColumns, 8,
			XmNheight, 200,
			XmNshadowType, XmSHADOW_ETCHED_OUT,
			XmNcellShadowThickness, 0,
			XmNtextShadowThickness, 0,
			XmNcellHighlightThickness, 1,
			NULL);

	XtAddCallback((*ps_w), XmNenterCellCallback,
			xbae_matrix_readonly_cell_cb, NULL);

	XtVaSetValues(docker_process_list_frame_w,
	XmNtopAttachment, XmATTACH_POSITION,
	XmNtopPosition, 0,
	XmNleftAttachment, XmATTACH_POSITION,
	XmNleftPosition, 50,
	XmNbottomAttachment, XmATTACH_POSITION,
	XmNbottomPosition, 30,
	XmNrightAttachment, XmATTACH_POSITION,
	XmNrightPosition, 100,
	NULL);

	XtManageChild(label);
	XtManageChild(docker_process_list_frame_w);
	return 0;
}

Widget get_ps_list_window(vrex_context* vrex) {
	Widget container_list_toplevel = XtNameToWidget(
			XtNameToWidget(
					XtNameToWidget(*(vrex->main_w), "docker_server_frame_w"),
					"docker_server_w"), "container_list_toplevel");
	return XtNameToWidget(
			XtNameToWidget(container_list_toplevel,
					"docker_process_list_frame_w"), "ps_w");
}

/**
 * Set the docker container id for the table
 *
 * \param ctx already initialized docker_context
 * \param id container id
 * \return error code
 */
int set_ps_window_docker_id(vrex_context* vrex, char* id) {
	docker_log_debug("Updating ps for id %s", id);
	Widget ps_w = get_ps_list_window(vrex);
	docker_result* result;
	docker_container_ps* ps;
	docker_process_list_container(vrex->d_ctx, &result, &ps, id, NULL);
	vrex->handle_error(vrex, result);
	if (is_ok(result)) {
		docker_log_debug("num titles %d num processes %d",
				arraylist_length(ps->titles),
				arraylist_length(ps->processes));
		for (int i = 0; i < arraylist_length(ps->titles); i++) {
//		docker_log_info("Title is %s at %d", arraylist_get(ps->titles, i),
//				i);
			xbae_matrix_add_column(ps_w, arraylist_get(ps->titles, i), i,
					10);
		}

		int num_rows = XbaeMatrixNumRows(ps_w);
		if (num_rows > 0) {
			XbaeMatrixDeleteRows(ps_w, 0, num_rows);
		}

		for (int i = 0; i < arraylist_length(ps->processes); i++) {
			arraylist* process = arraylist_get(ps->processes, i);
			printf("%d\n", arraylist_length(process));
			char** row = (char**) XtCalloc(arraylist_length(process) * 2,
					sizeof(char*));
			for (int j = 0; j < arraylist_length(process); j++) {
				// docker_log_info("Value is %s at %d", arraylist_get(process, j),
				// 		j);
				row[j] = str_clone(arraylist_get(process, j));
			}
			XbaeMatrixAddRows(ps_w, XbaeMatrixNumRows(ps_w), row, NULL, NULL, 1);
			free(row);
		}
	}
	return 0;
}
