/*
 * v-rex-motif: container_log_window.c
 * Created on: 12-Jan-2019
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

#include <Xm/Frame.h>
#include <Xm/LabelG.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <docker_containers.h>
#include "container_log_window.h"

/**
 * Create a new container log window
 *
 * \param vrex the app context
 * \param parent_w the parent window widget
 * \return error code.
 */
vrex_err_t make_docker_container_log_window(vrex_context* vrex, Widget parent_w) {
	Widget container_log_frame_w, label, container_log_text_w;
	int n = 0;
	Arg args[10];
	n = 0;
	XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT);
	n++;
	XtSetArg(args[n], XmNmarginWidth, 1);
	n++;
	XtSetArg(args[n], XmNmarginHeight, 1);
	n++;
	container_log_frame_w = XmCreateFrame(parent_w, "container_log_frame_w",
			args, n);
	XtVaSetValues(container_log_frame_w, XmNmarginWidth, 0, XmNmarginHeight, 0,
//	XmNtopAttachment, XmATTACH_POSITION,
//	XmNtopPosition, 50,
			XmNleftAttachment, XmATTACH_POSITION,
			XmNleftPosition, 0,
			XmNbottomAttachment, XmATTACH_POSITION,
			XmNbottomPosition, 100,
			XmNrightAttachment, XmATTACH_POSITION,
			XmNrightPosition, 50,
			NULL);
	Widget container_log_w = XtVaCreateManagedWidget("container_log_w",
			xmRowColumnWidgetClass, container_log_frame_w, XmNorientation,
			XmVERTICAL, XmNmarginWidth, 0, XmNmarginHeight, 0,
			NULL);
	n = 0;
	XtSetArg(args[n], XmNframeChildType, XmFRAME_TITLE_CHILD);
	n++;
	XtSetArg(args[n], XmNchildVerticalAlignment, XmALIGNMENT_CENTER);
	n++;
	label = XmCreateLabelGadget(container_log_frame_w, "Container logs (last 100 lines)", args,
			n);

	n = 0;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);
	n++;
	XtSetArg(args[n], XmNrows, 10);
	n++;
	XtSetArg(args[n], XmNwordWrap, True);
	n++;
	XtSetArg(args[n], XmNeditable, False);
	n++;
	container_log_text_w = XmCreateScrolledText(container_log_w,
			"container_log_text_w", args, n);

	XtManageChild(container_log_text_w);
	XtManageChild(label);
	XtManageChild(container_log_w);
	XtManageChild(container_log_frame_w);
	return VREX_SUCCESS;
}

Widget get_log_window(vrex_context* vrex) {
	Widget container_list_toplevel = XtNameToWidget(
			XtNameToWidget(
					XtNameToWidget(*(vrex->main_w), "docker_server_frame_w"),
					"docker_server_w"), "container_list_toplevel");
	return XtNameToWidget(container_list_toplevel, "container_log_frame_w");
}

Widget get_log_text_window(vrex_context* vrex) {
	return XtNameToWidget(
			XtNameToWidget(
					XtNameToWidget(get_log_window(vrex), "container_log_w"),
					"container_log_text_wSW"), "container_log_text_w");
}

/**
 * Show log for the given container id (must be running).
 *
 * \param vrex the app context
 * \param id container id
 * \return error code.
 */
vrex_err_t show_log_for_container(vrex_context* vrex, char* id) {
	docker_result* res;
	char* log_text;
	docker_container_logs(vrex->d_ctx, &res, &log_text, id, 0, 1, 1, -1, -1, 1,
			100);
	vrex->handle_error(vrex, res);
	XmTextSetString(get_log_text_window(vrex), log_text);
	return VREX_SUCCESS;
}
