/*
 * v-rex-motif: network_list_window.c
 * Created on: 08-Jan-2019
 *
 * v-rex-motif
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
#include <Xbae/Matrix.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>
#include <docker_networks.h>
#include "network_list_window.h"
#include <docker_log.h>

/**
 * Create a new docker networks list window
 *
 * \param vrex the app context
 * \param parent_w the parent window widget
 * \return error code.
 */
vrex_err_t make_docker_networks_list_window(vrex_context* vrex, Widget parent_w) {
	Widget docker_networks_list_w;
	Widget docker_networks_list_frame_w, label;
	int n = 0;
	Arg args[10];

	n = 0;
	XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT);
	n++;
	XtSetArg(args[n], XmNmarginWidth, 1);
	n++;
	XtSetArg(args[n], XmNmarginHeight, 1);
	n++;
	docker_networks_list_frame_w = XmCreateFrame(parent_w,
			"docker_networks_list_frame_w", args, n);

	n = 0;
	XtSetArg(args[n], XmNframeChildType, XmFRAME_TITLE_CHILD);
	n++;
	XtSetArg(args[n], XmNchildVerticalAlignment, XmALIGNMENT_CENTER);
	n++;
	label = XmCreateLabelGadget(docker_networks_list_frame_w, "Networks", args,
			n);

	docker_networks_list_w = XtVaCreateManagedWidget("docker_networks_list_w",
			xbaeMatrixWidgetClass, docker_networks_list_frame_w,
			XmNcolumns, 4,
			XmNrows, 0,
			XmNvisibleColumns, 4,
			XmNheight, 200,
			XmNshadowType, XmSHADOW_ETCHED_OUT,
			XmNcellShadowThickness, 0,
			XmNtextShadowThickness, 0,
			XmNcellHighlightThickness, 1,
			NULL);

	XtAddCallback(docker_networks_list_w, XmNenterCellCallback,
			xbae_matrix_readonly_cell_cb, NULL);

	XtVaSetValues(docker_networks_list_frame_w,
	XmNtopAttachment, XmATTACH_POSITION,
	XmNtopPosition, 60,
	XmNleftAttachment, XmATTACH_POSITION,
	XmNleftPosition, 30,
	XmNbottomAttachment, XmATTACH_POSITION,
	XmNbottomPosition, 100,
	XmNrightAttachment, XmATTACH_POSITION,
	XmNrightPosition, 46,
	NULL);

	XtManageChild(label);
	XtManageChild(docker_networks_list_w);
	XtManageChild(docker_networks_list_frame_w);

	refresh_networks_list(vrex);

	return VREX_SUCCESS;
}

Widget get_networks_list_window(vrex_context* vrex) {
	return XtNameToWidget(
			XtNameToWidget(
					XtNameToWidget(
							XtNameToWidget(*(vrex->main_w),
									"docker_server_frame_w"),
							"docker_server_w"), "docker_networks_list_frame_w"),
			"docker_networks_list_w");
}

/**
 * Refresh the network list in the window.
 *
 * \param vrex the app context
 * \return error code
 */
vrex_err_t refresh_networks_list(vrex_context* vrex) {
	docker_result* res;
	Widget docker_networks_list_w = get_networks_list_window(vrex);
	int col_num = 0;
	xbae_matrix_add_column(docker_networks_list_w, "Name", col_num, 10);
	col_num++;
	xbae_matrix_add_column(docker_networks_list_w, "Driver", col_num, 10);
	col_num++;
	xbae_matrix_add_column(docker_networks_list_w, "Scope", col_num, 10);
	col_num++;
	xbae_matrix_add_column(docker_networks_list_w, "Attachable", col_num, 15);
	col_num++;

	int num_rows = XbaeMatrixNumRows(docker_networks_list_w);
	if (num_rows > 0) {
		XbaeMatrixDeleteRows(docker_networks_list_w, 0, num_rows);
	}

	arraylist* networks;
	docker_networks_list(vrex->d_ctx, &res, &networks, NULL, NULL, NULL, NULL,
	NULL,
	NULL);
	vrex->handle_error(vrex, res);
	int len_nets = arraylist_length(networks);
	for (int i = 0; i < len_nets; i++) {
		docker_network* ni = (docker_network*) arraylist_get(networks, i);
		docker_log_info("Found network %s %s", ni->name, ni->id);

		col_num = 0;
		char** rows;
		rows = (char**) XtCalloc(10, sizeof(String));

		rows[col_num++] = ni->name;
		rows[col_num++] = ni->driver;
		rows[col_num++] = ni->scope;
		char* attachable_str = calloc(6, sizeof(char));
		if (ni->attachable == 0) {
			strcpy(attachable_str, "false");
		} else {
			strcpy(attachable_str, "true");
		}
		rows[col_num++] = attachable_str;

		XbaeMatrixAddRows(docker_networks_list_w,
				XbaeMatrixNumRows(docker_networks_list_w), rows, NULL, NULL, 1);
		free(attachable_str);
		free(rows);
	}

	return VREX_SUCCESS;
}

