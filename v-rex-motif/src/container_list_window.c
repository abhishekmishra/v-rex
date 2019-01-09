/*
 * v-rex: container_window.c
 * Created on: 27-Dec-2018
 *
 * v-rex
 * Copyright (C) 2018 Abhishek Mishra <abhishekmishra3@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <docker_containers.h>
#include <log.h>
#include <X11/Intrinsic.h>
#include <X11/Xmu/Editres.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>

#include "vrex_util.h"
#include "ps_window.h"

static int show_running = 1;

void show_running_callback(Widget widget, XtPointer client_data,
		XtPointer call_data) {
	XmToggleButtonCallbackStruct *state =
			(XmToggleButtonCallbackStruct *) call_data;
	docker_log_debug("%s: %s\n", XtName(widget),
			state->set == XmSET ? "on" :
			state->set == XmOFF ? "off" : "indeterminate");
	if (state->set == XmSET) {
		show_running = 1;
	} else {
		show_running = 0;
	}
}
//void refresh_call(Widget widget, XtPointer client_data, XtPointer call_data) {
//	vrex_context* vrex = (vrex_context*) client_data;
//	XtSetSensitive(widget, False);
////	Widget top = XtParent(XtParent(widget));
////	docker_log_debug("Refresh button name - %s", XtName(widget));
////	docker_log_debug("top - %s", XtName(top));
//	load_containers_list(get_container_list_w(vrex), vrex);
//}

void create_container_toolbar(vrex_context* vrex, Widget toolbar_w) {
	Widget toolbarButton, runningToggleButton, refreshButton;
	Widget container_toolbar_frame_w, label;
	int n = 0;
	Arg args[10];
	n = 0;
	XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT);
	n++;
	XtSetArg(args[n], XmNmarginWidth, 1);
	n++;
	XtSetArg(args[n], XmNmarginHeight, 1);
	n++;
	container_toolbar_frame_w = XmCreateFrame(toolbar_w,
			"container_toolbar_frame_w", args, n);
	XtVaSetValues(container_toolbar_frame_w, XmNmarginWidth, 0, XmNmarginHeight,
			0,
			XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 0,
			XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 0,
			XmNbottomAttachment, XmATTACH_POSITION, XmNbottomPosition, 2,
			XmNrightAttachment, XmATTACH_POSITION, XmNrightPosition, 2, NULL);
	Widget container_toolbar_w = XtVaCreateManagedWidget("container_toolbar_w",
			xmRowColumnWidgetClass, container_toolbar_frame_w, XmNorientation,
			XmHORIZONTAL, XmNmarginWidth, 0, XmNmarginHeight, 0,
			XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 0,
			XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 0,
			XmNbottomAttachment, XmATTACH_POSITION, XmNbottomPosition, 2,
			XmNrightAttachment, XmATTACH_POSITION, XmNrightPosition, 2, NULL);
	n = 0;
	label = XmCreateLabelGadget(container_toolbar_w, "Container", args, n);
	toolbarButton = XtVaCreateManagedWidget("Run Container",
			xmPushButtonWidgetClass, container_toolbar_w, XmNsensitive, False,
			NULL);
	XtManageChild(toolbarButton);

	runningToggleButton = XtVaCreateManagedWidget("Show Running",
			xmToggleButtonWidgetClass, container_toolbar_w,
			XmNset, XmSET,
			NULL);
	XtAddCallback(runningToggleButton, XmNvalueChangedCallback,
			show_running_callback, vrex->d_ctx);

//	refreshButton = XtVaCreateManagedWidget("Refresh", xmPushButtonWidgetClass,
//			container_toolbar_w, NULL);
//	XtManageChild(refreshButton);
//
//	XtAddCallback(refreshButton, XmNactivateCallback, refresh_call, vrex);

	XtManageChild(label);
	XtManageChild(container_toolbar_frame_w);
}

int list_containers(Widget mw, vrex_context* vrex) {
	char* id;
	docker_result* res;
	docker_containers_list_filter* filter;
	docker_containers_list* containers;

	char* first_id = NULL;

	make_docker_containers_list_filter(&filter);
	int limit = -1;
	if (show_running == 0) {
		limit = 25;
	}
	docker_container_list(vrex->d_ctx, &res, &containers, show_running == 0,
			limit, 1, filter);
	vrex->handle_error(vrex, res);
	docker_log_debug("Read %d containers.\n",
			docker_containers_list_length(containers));
	int col_num = 0;
	xbae_matrix_add_column(mw, "Name", col_num, 20);
	col_num++;
	xbae_matrix_add_column(mw, "Image", col_num, 20);
	col_num++;
	xbae_matrix_add_column(mw, "Command", col_num, 20);
	col_num++;
	xbae_matrix_add_column(mw, "State", col_num, 30);
	for (int i = 0; i < docker_containers_list_length(containers); i++) {
		col_num = 0;
		char** rows;
		rows = (char**) XtCalloc(10, sizeof(String));

		docker_container_list_item* item = docker_containers_list_get_idx(
				containers, i);
		rows[col_num++] = docker_container_list_item_names_get_idx(item, 0);
		if (i == 0) {
			first_id = docker_container_list_item_get_id(item);
		}
		rows[col_num++] = docker_container_list_item_get_image(item);
		rows[col_num++] = docker_container_list_item_get_command(item);
		if (docker_container_list_item_ports_length(item) > 0) {
			docker_container_ports* first_port =
					docker_container_list_item_ports_get_idx(item, 0);
			if (first_port
					&& docker_container_ports_get_public_port(first_port) > 0
					&& docker_container_ports_get_private_port(first_port)
							> 0) {
				char* ports_str = (char*) XtCalloc(128, sizeof(char));
				sprintf(ports_str, "%ld:%ld",
						docker_container_ports_get_public_port(first_port),
						docker_container_ports_get_private_port(first_port));
				rows[col_num++] = ports_str;
			}
		}
		char* status = (char*) XtCalloc(1024, sizeof(char));
		strcpy(status, docker_container_list_item_get_state(item));
		strcat(status, ":");
		strcat(status, docker_container_list_item_get_status(item));
		rows[col_num++] = status;

		XbaeMatrixAddRows(mw, XbaeMatrixNumRows(mw), rows, NULL, NULL, 1);
		free(rows);
	}

	Widget toolbar = get_toolbar_w(vrex);
	Widget refresh = XtNameToWidget(toolbar, "Refresh");

	if (first_id) {
		set_ps_window_docker_id(vrex, first_id);
	}

	XmUpdateDisplay(XtParent(XtParent(mw)));
	return 0;
}

Widget get_container_list_window(vrex_context* vrex) {
	Widget container_list_toplevel = XtNameToWidget(
			XtNameToWidget(
					XtNameToWidget(*(vrex->main_w), "docker_server_frame_w"),
					"docker_server_w"), "container_list_toplevel");
	return XtNameToWidget(
			XtNameToWidget(container_list_toplevel,
					"docker_containers_list_frame_w"), "mw");
}

int refresh_containers_list(vrex_context* vrex) {
	Widget matrix_w = get_container_list_window(vrex);
	int num_rows = XbaeMatrixNumRows(matrix_w);
	if (num_rows > 0) {
		XbaeMatrixDeleteRows(matrix_w, 0, num_rows);
	}
	list_containers(matrix_w, vrex);
	return 0;
}

void labelCB(Widget mw, XtPointer cd, XtPointer cb) {
	XbaeMatrixLabelActivateCallbackStruct *cbs =
			(XbaeMatrixLabelActivateCallbackStruct *) cb;

	if (cbs->row_label)
		if (XbaeMatrixIsRowSelected(mw, cbs->row))
			XbaeMatrixDeselectRow(mw, cbs->row);
		else
			XbaeMatrixSelectRow(mw, cbs->row);
	else if (XbaeMatrixIsColumnSelected(mw, cbs->column))
		XbaeMatrixDeselectColumn(mw, cbs->column);
	else
		XbaeMatrixSelectColumn(mw, cbs->column);
}

/**
 * Create a new container list window
 *
 * \param parent window
 * \param container_ls_w handle to populate with created widget.
 * \return error code
 */
int make_container_list_window(Widget parent, Widget* container_ls_w,
		vrex_context* vrex) {
	Widget container_list_toplevel, ps_w, matrix_w;
	Widget docker_server_summary_textSW = XtNameToWidget(parent,
			"docker_server_summary_textSW");
	Widget docker_containers_list_frame_w, label;
	int n = 0;
	Arg args[10];

	container_list_toplevel = XtVaCreateManagedWidget("container_list_toplevel",
			xmFormWidgetClass, parent,
			XmNborderWidth, 0,
			XmNshadowThickness, 0,
			XmNfractionBase, 100,
			XmNorientation, XmVERTICAL,
			XmNpacking, XmPACK_TIGHT,
			XmNcolumns, 1,
			XmNrows, 3,
//Need to attach to the bottom right of the summary text window.
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, docker_server_summary_textSW,
			XmNleftAttachment, XmATTACH_POSITION,
			XmNleftPosition, 0,
			XmNbottomAttachment, XmATTACH_POSITION,
			XmNbottomPosition, 60,
			XmNrightAttachment, XmATTACH_POSITION,
			XmNrightPosition, 70,
			NULL);

	n = 0;
	XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT);
	n++;
	XtSetArg(args[n], XmNmarginWidth, 1);
	n++;
	XtSetArg(args[n], XmNmarginHeight, 1);
	n++;
	docker_containers_list_frame_w = XmCreateFrame(container_list_toplevel,
			"docker_containers_list_frame_w", args, n);

	n = 0;
	XtSetArg(args[n], XmNframeChildType, XmFRAME_TITLE_CHILD);
	n++;
	XtSetArg(args[n], XmNchildVerticalAlignment, XmALIGNMENT_CENTER);
	n++;
	label = XmCreateLabelGadget(docker_containers_list_frame_w, "Containers",
			args, n);

	matrix_w = XtVaCreateManagedWidget("mw", xbaeMatrixWidgetClass,
			docker_containers_list_frame_w,
			XmNcolumns, 4,
			XmNrows, 0,
			XmNvisibleColumns, 4,
			XmNfixedColumns, 1,
			XmNheight, 200,
			XmNshadowType, XmSHADOW_ETCHED_OUT,
			XmNcellShadowThickness, 0,
			XmNtextShadowThickness, 0,
			XmNcellHighlightThickness, 1,
			NULL);

	XtVaSetValues(docker_containers_list_frame_w,
	XmNtopAttachment, XmATTACH_POSITION,
	XmNtopPosition, 0,
	XmNleftAttachment, XmATTACH_POSITION,
	XmNleftPosition, 0,
	XmNbottomAttachment, XmATTACH_POSITION,
	XmNbottomPosition, 100,
	XmNrightAttachment, XmATTACH_POSITION,
	XmNrightPosition, 50,
	NULL);

	XtAddCallback(matrix_w, XmNlabelActivateCallback, labelCB, NULL);
	XtAddCallback(matrix_w, XmNenterCellCallback, xbae_matrix_readonly_cell_cb,
	NULL);
	XtManageChild(label);
	XtManageChild(matrix_w);
	XtManageChild(docker_containers_list_frame_w);

	make_ps_window(container_list_toplevel, &ps_w);
	XtManageChild(ps_w);

	container_ls_w = &matrix_w;
	refresh_containers_list(vrex);
	return 0;
}

