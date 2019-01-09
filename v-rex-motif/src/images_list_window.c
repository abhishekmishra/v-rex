/*
 * v-rex-motif: images_list_window.c
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

#include <stdlib.h>
#include <Xbae/Matrix.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>
#include <log.h>
#include <docker_images.h>
#include "images_list_window.h"

/**
 * Create a new docker images list window
 *
 * \param vrex the app context
 * \param parent_w the parent window widget
 * \return error code.
 */
vrex_err_t make_docker_images_list_window(vrex_context* vrex, Widget parent_w) {
	Widget docker_images_list_w;
	Widget docker_images_list_frame_w, label;
	int n = 0;
	Arg args[10];

	n = 0;
	XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT);
	n++;
	XtSetArg(args[n], XmNmarginWidth, 1);
	n++;
	XtSetArg(args[n], XmNmarginHeight, 1);
	n++;
	docker_images_list_frame_w = XmCreateFrame(parent_w,
			"docker_images_list_frame_w", args, n);

	n = 0;
	XtSetArg(args[n], XmNframeChildType, XmFRAME_TITLE_CHILD);
	n++;
	XtSetArg(args[n], XmNchildVerticalAlignment, XmALIGNMENT_CENTER);
	n++;
	label = XmCreateLabelGadget(docker_images_list_frame_w, "Images", args, n);

	docker_images_list_w = XtVaCreateManagedWidget("docker_images_list_w",
			xbaeMatrixWidgetClass, docker_images_list_frame_w,
			XmNcolumns, 3,
			XmNrows, 0,
			XmNvisibleColumns, 3,
			XmNheight, 200,
			XmNshadowType, XmSHADOW_ETCHED_OUT,
			XmNcellShadowThickness, 0,
			XmNtextShadowThickness, 0,
			XmNcellHighlightThickness, 1,
			NULL);

//	XtAddCallback(docker_images_list_w, XmNlabelActivateCallback, labelCB, NULL);
	XtAddCallback(docker_images_list_frame_w, XmNenterCellCallback,
			xbae_matrix_readonly_cell_cb, NULL);
	XtVaSetValues(docker_images_list_frame_w,
	XmNtopAttachment, XmATTACH_POSITION,
	XmNtopPosition, 60,
	XmNleftAttachment, XmATTACH_POSITION,
	XmNleftPosition, 0,
	XmNbottomAttachment, XmATTACH_POSITION,
	XmNbottomPosition, 100,
	XmNrightAttachment, XmATTACH_POSITION,
	XmNrightPosition, 30,
	NULL);

	XtManageChild(label);
	XtManageChild(docker_images_list_w);
	XtManageChild(docker_images_list_frame_w);

	refresh_images_list(vrex);

	return VREX_SUCCESS;
}

Widget get_images_list_window(vrex_context* vrex) {
	return XtNameToWidget(
			XtNameToWidget(
					XtNameToWidget(
							XtNameToWidget(*(vrex->main_w),
									"docker_server_frame_w"),
							"docker_server_w"), "docker_images_list_frame_w"),
			"docker_images_list_w");
}

char* get_image_tags_concat(docker_image* img) {
	char* tags = NULL;
	if (img->repo_tags) {
		int len_tags = array_list_length(img->repo_tags);
		int tag_strlen = 0;
		for (int i = 0; i < len_tags; i++) {
			tag_strlen += strlen((char*) array_list_get_idx(img->repo_tags, i));
			tag_strlen += 1; //for newline
		}
		tag_strlen += 1; //for null terminator
		tags = (char*) calloc(tag_strlen, sizeof(char));
		if (tags != NULL) {
			tags[0] = '\0';
			for (int i = 0; i < len_tags; i++) {
				strcat(tags, (char*) array_list_get_idx(img->repo_tags, i));
				if (i != (len_tags - 1)) {
					strcat(tags, "\n");
				}
			}
		}
	}
	return tags;
}

/**
 * Refresh the images list in the window.
 *
 * \param vrex the app context
 * \return error code
 */
vrex_err_t refresh_images_list(vrex_context* vrex) {
	docker_result* res;
	Widget docker_images_list_w = get_images_list_window(vrex);

	int col_num = 0;
	xbae_matrix_add_column(docker_images_list_w, "Image Tag", col_num, 40);
	col_num++;
	xbae_matrix_add_column(docker_images_list_w, "Size", col_num, 20);
	col_num++;
	xbae_matrix_add_column(docker_images_list_w, "Virtual Size", col_num, 20);
	col_num++;

	struct array_list* images;
	docker_images_list(vrex->d_ctx, &res, &images, 0, 1, NULL, 0, NULL, NULL,
	NULL);
	vrex->handle_error(vrex, res);
	int len_images = array_list_length(images);
	for (int i = 0; i < len_images; i++) {
		docker_image* img = (docker_image*) array_list_get_idx(images, i);
		col_num = 0;
		char** rows;
		rows = (char**) XtCalloc(10, sizeof(String));
		char* tags = get_image_tags_concat(img);
		rows[col_num++] = tags;
		char* size = calculate_size(docker_image_get_size(img));
		rows[col_num++] = size;
		char* virtual_size = calculate_size(docker_image_get_virtual_size(img));
		rows[col_num++] = virtual_size;
		XbaeMatrixAddRows(docker_images_list_w,
				XbaeMatrixNumRows(docker_images_list_w), rows, NULL, NULL, 1);
		free(size);
		free(virtual_size);
		free(tags);
		free(rows);
	}
	return VREX_SUCCESS;
}

