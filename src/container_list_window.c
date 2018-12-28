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

#include "vrex_util.h"
#include "ps_window.h"

static int show_running = 1;

int list_containers(Widget mw, vrex_context* vrex) {
	char* id;
	docker_result* res;
	docker_containers_list_filter* filter;
	docker_containers_list* containers;

	curl_global_init(CURL_GLOBAL_ALL);
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
	add_column(mw, "Name", col_num, 20);
	col_num++;
	add_column(mw, "Image", col_num, 20);
	col_num++;
	add_column(mw, "Command", col_num, 20);
	col_num++;
	add_column(mw, "State", col_num, 30);
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

	Widget top = XtParent(mw);
	Widget toolbar = XtNameToWidget(top, "docker_list_toolbar");
	Widget refresh = XtNameToWidget(toolbar, "Refresh");

	if (first_id) {
		set_ps_window_docker_id(XtNameToWidget(XtParent(mw), "ps_w"), vrex,
				first_id);
	}

	XtSetSensitive(refresh, True);
	XmUpdateDisplay(XtParent(XtParent(mw)));
	curl_global_cleanup();
	return 0;
}

int load_containers_list(Widget matrix_w, vrex_context* vrex) {
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

void cellCB(Widget mw, XtPointer cd, XtPointer cb) {
	XbaeMatrixEnterCellCallbackStruct *cbs =
			(XbaeMatrixEnterCellCallbackStruct*) cb;
	cbs->map = True;
	cbs->doit = False;
	cbs->select_text = True;
}

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
void refresh_call(Widget widget, XtPointer client_data, XtPointer call_data) {
	vrex_context* vrex = (vrex_context*) client_data;
	XtSetSensitive(widget, False);
	Widget top = XtParent(XtParent(widget));
	docker_log_debug("Refresh button name - %s", XtName(widget));
	docker_log_debug("top - %s", XtName(top));
	load_containers_list(XtNameToWidget(top, "mw"), vrex);
}

void create_docker_list_toolbar(Widget container_list_toplevel,
		vrex_context* vrex) {
	Widget toolbar, runningToggleButton, refreshButton;
	toolbar = XtVaCreateManagedWidget("docker_list_toolbar",
			xmRowColumnWidgetClass, container_list_toplevel,
			XmNorientation, XmHORIZONTAL,
			NULL);

	runningToggleButton = XtVaCreateManagedWidget("Show Running",
			xmToggleButtonWidgetClass, toolbar,
			XmNset, XmSET,
			NULL);
	XtAddCallback(runningToggleButton, XmNvalueChangedCallback,
			show_running_callback, vrex->d_ctx);

	refreshButton = XtVaCreateManagedWidget("Refresh", xmPushButtonWidgetClass,
			toolbar, NULL);
	XtManageChild(refreshButton);

	XtAddCallback(refreshButton, XmNactivateCallback, refresh_call, vrex);

	XtManageChild(toolbar);
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

	container_list_toplevel = XtVaCreateManagedWidget("container_list_toplevel",
			xmRowColumnWidgetClass, parent,
			XmNorientation, XmVERTICAL,
			XmNpacking, XmPACK_TIGHT,
			XmNcolumns, 1,
			XmNtopAttachment, XmATTACH_FORM,
			XmNleftAttachment, XmATTACH_FORM,
			XmNrightAttachment, XmATTACH_POSITION,
			XmNrightPosition, 100,
			XmNbottomAttachment, XmATTACH_POSITION,
			XmNbottomPosition, 100,
			NULL);

	create_docker_list_toolbar(container_list_toplevel, vrex);

	matrix_w = XtVaCreateManagedWidget("mw", xbaeMatrixWidgetClass,
			container_list_toplevel,
			XmNcolumns, 4,
			XmNrows, 0,
			XmNvisibleColumns, 4,
			XmNfixedColumns, 1,
			XmNheight, 200,
			XmNshadowType, XmSHADOW_ETCHED_OUT,
			XmNcellShadowThickness, 0,
			XmNtextShadowThickness, 0,
			XmNcellHighlightThickness, 1,
			XmNcolumnLabelColor, 0x800000,
			NULL);

	XtAddCallback(matrix_w, XmNlabelActivateCallback, labelCB, NULL);
	XtAddCallback(matrix_w, XmNenterCellCallback, cellCB, NULL);
	XtManageChild(matrix_w);

	make_ps_window(container_list_toplevel, &ps_w);
	XtManageChild(ps_w);

	container_ls_w = &matrix_w;
	load_containers_list(matrix_w, vrex);
	return 0;
}
