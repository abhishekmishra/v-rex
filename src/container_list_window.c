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

#include "vrex_util.h"
#include "ps_window.h"


int list_containers(Widget mw, docker_context* ctx) {
	char* id;
	docker_result* res;
	docker_containers_list_filter* filter;
	docker_containers_list* containers;

	curl_global_init(CURL_GLOBAL_ALL);
	char* first_id = NULL;

	printf("Docker containers list.\n\n");
	make_docker_containers_list_filter(&filter);
//		containers_filter_add_name(filter, "/registryui");
//		containers_filter_add_id(filter, id);
	docker_container_list(ctx, &res, &containers, 0, -1, 1, filter);
	handle_error(res);
	docker_log_info("Read %d containers.\n",
			docker_containers_list_length(containers));
	int col_num = 0;
	add_column(mw, "Name", col_num, 20);
	col_num++;
//		add_column(mw, "Id", col_num, 30);
//		col_num++;
	add_column(mw, "Image", col_num, 20);
	col_num++;
	add_column(mw, "Command", col_num, 20);
	col_num++;
	add_column(mw, "FS Size", col_num, 10);
	col_num++;
	add_column(mw, "State", col_num, 30);
//		col_num++;
//		add_column(mw, "Status", col_num, 20);
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
//			rows[1] = docker_container_list_item_get_id(item);
		rows[col_num++] = docker_container_list_item_get_image(item);
		rows[col_num++] = docker_container_list_item_get_command(item);
//			char* root_fs_size = (char*) XtCalloc(10, sizeof(char));
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
//			sprintf(root_fs_size, "%lld",
//					docker_container_list_item_get_size_root_fs(item));
//			rows[col_num++] = root_fs_size;
		char* status = (char*) XtCalloc(1024, sizeof(char));
		strcpy(status, docker_container_list_item_get_state(item));
		strcat(status, ":");
		strcat(status, docker_container_list_item_get_status(item));
		rows[col_num++] = status;

		XbaeMatrixAddRows(mw, XbaeMatrixNumRows(mw), rows, NULL, NULL, 1);
		free(rows);
	}

	Widget top = XtParent(XtParent(mw));
	docker_log_info("Found parent %s.", XtName(top));
	Widget toolbar = XtNameToWidget(top, "toolbar");
	docker_log_info("Found toolbar %s.", XtName(toolbar));
	Widget refresh = XtNameToWidget(toolbar, "Refresh");
	docker_log_info("Found refresh %s.", XtName(refresh));

	if (first_id) {
		set_ps_window_docker_id(XtNameToWidget(XtParent(mw), "ps_w"), ctx,
				first_id);
	}

	XtSetSensitive(refresh, True);
	XmUpdateDisplay(XtParent(mw));
	curl_global_cleanup();
	return 0;
}

int load_containers_list(Widget matrix_w, docker_context* ctx) {
	int num_rows = XbaeMatrixNumRows(matrix_w);
	if (num_rows > 0) {
		XbaeMatrixDeleteRows(matrix_w, 0, num_rows);
	}
	list_containers(matrix_w, ctx);
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

/**
 * Create a new container list window
 *
 * \param parent window
 * \param container_ls_w handle to populate with created widget.
 * \return error code
 */
int make_container_list_window(Widget parent, Widget* container_ls_w, docker_context* ctx) {
	Widget ps_w, matrix_w;
	XFontStruct *plain_font = XLoadQueryFont(XtDisplay(parent),
			"-*-terminus-medium-r-*-*-14-*-*-*-*-*-*-*");
	XmFontListEntry font_list_entry = XmFontListEntryCreate(
	XmFONTLIST_DEFAULT_TAG, XmFONT_IS_FONT, plain_font);

	XmFontList plain_font_list = XmFontListAppendEntry(
	NULL, font_list_entry);

	docker_log_info("plain font: %d plain font list %d", plain_font == NULL,
			plain_font_list == NULL);

	matrix_w = XtVaCreateManagedWidget("mw", xbaeMatrixWidgetClass, parent,
	/* attach to top, left of form */
	XmNtopAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_POSITION,
	XmNrightPosition, 50,
	XmNbottomAttachment, XmATTACH_POSITION,
	XmNbottomPosition, 50,
//	XmNlabelFont, plain_font_list,
//	XmNfontList, plain_font_list,
	NULL);

	XmFontListEntryFree(&font_list_entry);
	XmFontListFree(plain_font_list);

	XtAddCallback(matrix_w, XmNlabelActivateCallback, labelCB, NULL);
	XtAddCallback(matrix_w, XmNenterCellCallback, cellCB, NULL);

	make_ps_window(parent, &ps_w);
	container_ls_w = &matrix_w;
	load_containers_list(matrix_w, ctx);
	return 0;
}

