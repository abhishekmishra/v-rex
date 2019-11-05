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
#include <pthread.h>
#include <Xbae/Matrix.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/PushB.h>
#include <docker_images.h>
#include "images_list_window.h"

#include <docker_log.h>

#include "statusbar_window.h"
#include "vrex_util.h"
#include "messages_window.h"

static pthread_mutex_t images_list_w_lock;

void docker_pull_status_cb(docker_image_create_status* status, void* cbargs) {
	vrex_context* vrex = (vrex_context*) cbargs;
	add_messages_entry(vrex, status->status);
}

struct pull_args {
	vrex_context* vrex;
	char* image_name;
};

void* docker_pull_util(void* args) {
//	vrex_context* vrex = (vrex_context*) args;
	struct pull_args* pa = (struct pull_args*) args;
	docker_result* result;
	docker_image_create_from_image_cb(pa->vrex->d_ctx, &result,
			&docker_pull_status_cb, pa->vrex, pa->image_name, NULL, NULL);
	pa->vrex->handle_error(pa->vrex, result);
	free(pa->image_name);
	free(pa);
	pthread_exit(0);
	return NULL;
}

void docker_pull_cb(Widget widget, XtPointer client_data, XtPointer call_data) {
	vrex_context* vrex = (vrex_context*) client_data;
	pthread_t docker_pull_thread;
	struct pull_args* pa = (struct pull_args*) calloc(1,
			sizeof(struct pull_args));
	pa->vrex = vrex;
	String val;
	Widget imageWidget = XtNameToWidget(XtParent(widget), "Image Name");
	XtVaGetValues(imageWidget, XmNvalue, &val, NULL);
	pa->image_name = (char*) val;
	int thread_id = pthread_create(&docker_pull_thread, NULL, &docker_pull_util,
			pa);
}

void create_images_toolbar(vrex_context* vrex, Widget toolbar_w) {
	Widget toolbarButton;
	Widget images_toolbar_frame_w, label, request_time_text;
	int n = 0;
	Arg args[10];
	n = 0;
	XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT);
	n++;
	XtSetArg(args[n], XmNmarginWidth, 1);
	n++;
	XtSetArg(args[n], XmNmarginHeight, 1);
	n++;
	images_toolbar_frame_w = XmCreateFrame(toolbar_w, "images_toolbar_frame_w",
			args, n);
	XtVaSetValues(images_toolbar_frame_w, XmNmarginWidth, 0, XmNmarginHeight, 0,
			XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 0,
			XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 0,
			XmNbottomAttachment, XmATTACH_POSITION, XmNbottomPosition, 2,
			XmNrightAttachment, XmATTACH_POSITION, XmNrightPosition, 2, NULL);
	Widget images_toolbar_w = XtVaCreateManagedWidget("images_toolbar_w",
			xmRowColumnWidgetClass, images_toolbar_frame_w, XmNorientation,
			XmHORIZONTAL, XmNmarginWidth, 0, XmNmarginHeight, 0,
			XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 0,
			XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 0,
			XmNbottomAttachment, XmATTACH_POSITION, XmNbottomPosition, 2,
			XmNrightAttachment, XmATTACH_POSITION, XmNrightPosition, 2, NULL);
	n = 0;
	label = XmCreateLabelGadget(images_toolbar_w, "Image", args, n);

	request_time_text = XtVaCreateManagedWidget("Image Name", xmTextWidgetClass,
			images_toolbar_w,
			XmNeditable, True,
			XmNvalue, "alpine:latest",
			XmNcolumns, 40,
			NULL);

	toolbarButton = XtVaCreateManagedWidget("Pull",
			xmPushButtonWidgetClass, images_toolbar_w, NULL);
	XtAddCallback(toolbarButton, XmNactivateCallback, docker_pull_cb, vrex);
	XtManageChild(toolbarButton);

	XtManageChild(label);
	XtManageChild(images_toolbar_frame_w);
}


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

	XtAddCallback(docker_images_list_w, XmNenterCellCallback,
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
		int len_tags = arraylist_length(img->repo_tags);
		int tag_strlen = 0;
		for (int i = 0; i < len_tags; i++) {
			tag_strlen += strlen((char*) arraylist_get(img->repo_tags, i));
			tag_strlen += 1; //for newline
		}
		tag_strlen += 1; //for null terminator
		tags = (char*) calloc(tag_strlen, sizeof(char));
		if (tags != NULL) {
			tags[0] = '\0';
			for (int i = 0; i < len_tags; i++) {
				strcat(tags, (char*) arraylist_get(img->repo_tags, i));
				if (i != (len_tags - 1)) {
					strcat(tags, "\n");
				}
			}
		}
	}
	return tags;
}

void* refresh_images_list_util(void* args) {
	int ret = pthread_mutex_lock(&images_list_w_lock);

	vrex_context* vrex = (vrex_context*) args;
	docker_result* res;
	set_statusbar_message(vrex, "Loading images...");

	Widget docker_images_list_w = get_images_list_window(vrex);
	int col_num = 0;
	xbae_matrix_add_column(docker_images_list_w, "Image Tag", col_num, 40);
	col_num++;
	xbae_matrix_add_column(docker_images_list_w, "Size", col_num, 20);
	col_num++;
	xbae_matrix_add_column(docker_images_list_w, "Virtual Size", col_num, 20);
	col_num++;
	arraylist* images;
	docker_images_list(vrex->d_ctx, &res, &images, 0, 1, NULL, 0, NULL, NULL,
	NULL);
	vrex->handle_error(vrex, res);
	int len_images = arraylist_length(images);
	for (int i = 0; i < len_images; i++) {
		docker_image* img = (docker_image*) arraylist_get(images, i);
		col_num = 0;
		char** rows;
		rows = (char**) XtCalloc(10, sizeof(String));
		char* tags = get_image_tags_concat(img);
		rows[col_num++] = tags;
		char* size = calculate_size(img->size);
		rows[col_num++] = size;
		char* virtual_size = calculate_size(img->virtual_size);
		rows[col_num++] = virtual_size;
		XbaeMatrixAddRows(docker_images_list_w,
				XbaeMatrixNumRows(docker_images_list_w), rows, NULL, NULL, 1);
		free(size);
		free(virtual_size);
		free(tags);
		free(rows);
	}

	set_statusbar_message(vrex, "Loaded images.");
	pthread_mutex_unlock(&images_list_w_lock);
	return NULL;
}

/**
 * Refresh the images list in the window.
 *
 * \param vrex the app context
 * \return error code
 */
vrex_err_t refresh_images_list(vrex_context* vrex) {
	pthread_t docker_ping_thread;
	int thread_id = pthread_create(&docker_ping_thread, NULL,
			&refresh_images_list_util, vrex);
	return VREX_SUCCESS;
}

