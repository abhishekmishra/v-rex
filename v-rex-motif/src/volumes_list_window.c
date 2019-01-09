/*
 * v-rex-motif: volumes_list_window.c
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
#include <log.h>
#include <docker_volumes.h>
#include "volumes_list_window.h"

/**
 * Create a new docker volumes list window
 *
 * \param vrex the app context
 * \param parent_w the parent window widget
 * \return error code.
 */
vrex_err_t make_docker_volumes_list_window(vrex_context* vrex, Widget parent_w) {
	Widget docker_volumes_list_w;
	Widget docker_volumes_list_frame_w, label;
	int n = 0;
	Arg args[10];

	n = 0;
	XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT);
	n++;
	XtSetArg(args[n], XmNmarginWidth, 1);
	n++;
	XtSetArg(args[n], XmNmarginHeight, 1);
	n++;
	docker_volumes_list_frame_w = XmCreateFrame(parent_w,
			"docker_volumes_list_frame_w", args, n);

	n = 0;
	XtSetArg(args[n], XmNframeChildType, XmFRAME_TITLE_CHILD);
	n++;
	XtSetArg(args[n], XmNchildVerticalAlignment, XmALIGNMENT_CENTER);
	n++;
	label = XmCreateLabelGadget(docker_volumes_list_frame_w, "Volumes",
			args, n);

	docker_volumes_list_w = XtVaCreateManagedWidget("docker_volumes_list_w",
			xbaeMatrixWidgetClass, docker_volumes_list_frame_w,
			XmNcolumns, 4,
			XmNrows, 0,
			XmNvisibleColumns, 4,
			XmNheight, 200,
			XmNshadowType, XmSHADOW_ETCHED_OUT,
			XmNcellShadowThickness, 0,
			XmNtextShadowThickness, 0,
			XmNcellHighlightThickness, 1,
			NULL);

//	XtAddCallback(docker_volumes_list_w, XmNlabelActivateCallback, labelCB, NULL);
//	XtAddCallback(docker_volumes_list_w, XmNenterCellCallback, cellCB, NULL);
	XtVaSetValues(docker_volumes_list_frame_w,
	XmNtopAttachment, XmATTACH_POSITION,
	XmNtopPosition, 60,
	XmNleftAttachment, XmATTACH_POSITION,
	XmNleftPosition, 46,
	XmNbottomAttachment, XmATTACH_POSITION,
	XmNbottomPosition, 100,
	XmNrightAttachment, XmATTACH_POSITION,
	XmNrightPosition, 70,
	NULL);

	XtManageChild(label);
	XtManageChild(docker_volumes_list_w);
	XtManageChild(docker_volumes_list_frame_w);

	refresh_volumes_list(vrex);

	return VREX_SUCCESS;
}

Widget get_volumes_list_window(vrex_context* vrex) {
	return XtNameToWidget(
			XtNameToWidget(
					XtNameToWidget(
							XtNameToWidget(*(vrex->main_w),
									"docker_server_frame_w"),
							"docker_server_w"), "docker_volumes_list_frame_w"),
			"docker_volumes_list_w");
}

/**
 * Refresh the volume list in the window.
 *
 * \param vrex the app context
 * \return error code
 */
vrex_err_t refresh_volumes_list(vrex_context* vrex) {
	docker_result* res;
	Widget docker_volumes_list_w = get_volumes_list_window(vrex);
	int col_num = 0;
	xbae_matrix_add_column(docker_volumes_list_w, "Name", col_num, 25);
	col_num++;
	xbae_matrix_add_column(docker_volumes_list_w, "Driver", col_num, 10);
	col_num++;
	xbae_matrix_add_column(docker_volumes_list_w, "Scope", col_num, 10);
	col_num++;
	xbae_matrix_add_column(docker_volumes_list_w, "Mount Point", col_num, 25);
	col_num++;

	int num_rows = XbaeMatrixNumRows(docker_volumes_list_w);
	if (num_rows > 0) {
		XbaeMatrixDeleteRows(docker_volumes_list_w, 0, num_rows);
	}

	struct array_list* volumes;
	struct array_list* warnings;
	docker_volumes_list(vrex->d_ctx, &res, &volumes, &warnings, 1, NULL, NULL,
			NULL);
	vrex->handle_error(vrex, res);
	int len_nets = array_list_length(volumes);
	for (int i = 0; i < len_nets; i++) {
		docker_volume* vol = (docker_volume*) array_list_get_idx(
				volumes, i);
		docker_log_info("Found volume %s %s", vol->name, vol->mountpoint);

		col_num = 0;
		char** rows;
		rows = (char**) XtCalloc(10, sizeof(String));

		rows[col_num++] = docker_volume_get_name(vol);
		rows[col_num++] = docker_volume_get_driver(vol);
		rows[col_num++] = docker_volume_get_scope(vol);
		rows[col_num++] = docker_volume_get_mountpoint(vol);

		XbaeMatrixAddRows(docker_volumes_list_w,
				XbaeMatrixNumRows(docker_volumes_list_w), rows, NULL, NULL, 1);
		free(rows);
	}

	return VREX_SUCCESS;
}
