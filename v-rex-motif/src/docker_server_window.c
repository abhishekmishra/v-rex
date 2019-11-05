/*
 * v-rex-motif: docker_server_window.c
 * Created on: 30-Dec-2018
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
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/Frame.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <X11/Xos.h>
// #include <XmHTML/XmHTML.h>
#include "docker_server_window.h"

#include <docker_log.h>
#include "docker_system.h"
#include "vrex_util.h"

void create_summary_scrolled_text(Widget* dst, Widget docker_server_w) {
	int n;
	Arg args[20];
	n = 0;
	XtSetArg(args[n], XmNrows, 1);
	n++;
	XtSetArg(args[n], XmNcolumns, 40);
	n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);
	n++;
	XtSetArg(args[n], XmNscrollHorizontal, False);
	n++;
	XtSetArg(args[n], XmNscrollVertical, False);
	n++;
	XtSetArg(args[n], XmNwordWrap, True);
	n++;
	XtSetArg(args[n], XmNeditable, False);
	n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION);
	n++;
	XtSetArg(args[n], XmNtopPosition, 0);
	n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION);
	n++;
	XtSetArg(args[n], XmNleftPosition, 0);
	n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION);
	n++;
	XtSetArg(args[n], XmNrightPosition, 100);
	n++;
//  Don't want a fixed height, just enough to fill one line of text for now.
//	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION);
//	n++;
//	XtSetArg(args[n], XmNbottomPosition, 10);
//	n++;
	Widget docker_summary_text = XmCreateScrolledText(docker_server_w,
			"docker_server_summary_text", args, n);
	XtManageChild(docker_summary_text);
	XtVaSetValues(docker_summary_text,
	XmNvalue, "-",
	XmNcursorPositionVisible, False,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	NULL);
	XtManageChild(docker_server_w);

	(*dst) = docker_summary_text;
}

vrex_err_t make_docker_server_window(vrex_context* vrex, Widget* server_w) {
	Widget list_w, child, docker_summary_text, docker_summary_scrolled_w;
	Widget docker_server_frame, label;
	int n = 0;
	Arg args[10];

	n = 0;
	XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT);
	n++;
	docker_server_frame = XmCreateFrame(*(vrex->main_w),
			"docker_server_frame_w", args, n);
	XtVaSetValues(*(vrex->main_w), XmNworkWindow, docker_server_frame,
	NULL);

	n = 0;
	XtSetArg(args[n], XmNframeChildType, XmFRAME_TITLE_CHILD);
	n++;
	XtSetArg(args[n], XmNchildVerticalAlignment, XmALIGNMENT_CENTER);
	n++;
	label = XmCreateLabelGadget(docker_server_frame, "Docker Server Summary",
			args, n);

	Widget docker_server_w = XtVaCreateManagedWidget("docker_server_w",
			xmFormWidgetClass, docker_server_frame,
			XmNborderWidth, 0,
			XmNshadowThickness, 0,
			XmNfractionBase, 100,
			NULL);

	docker_result* res;
	docker_info* info;
	docker_system_info(vrex->d_ctx, &res, &info);
	vrex->handle_error(vrex, res);

	// printf("| Server: %s | CPU: %d | Memtotal: %s | Containers: %lu (Running: %lu, Paused: %lu, Stopped: %lu) | Images: %lu |",
	// 		info->name, info->ncpu, calculate_size_str(info->memtotal),
	// 		info->containers, info->containers_running, info->containers_paused,
	// 		info->containers_stopped, info->images);

	char* summary = (char*) calloc(1024, sizeof(char));
	sprintf(summary,
			"| Server: %s | CPU: %d | Memtotal: %s | Containers: %lu (Running: %lu, Paused: %lu, Stopped: %lu) | Images: %lu |",
			info->name, info->ncpu, calculate_size_str(info->memtotal),
			info->containers, info->containers_running, info->containers_paused,
			info->containers_stopped, info->images);

	create_summary_scrolled_text(&docker_summary_text, docker_server_w);

	XtVaSetValues(docker_summary_text,
	XmNvalue, summary, NULL);

//	Widget events_table;
//	create_events_table(&events_table, docker_server_w);
//
//	update_events_table(events_table, vrex, res);

//	create_docker_server_toolbar(docker_server_w, vrex);

	XtManageChild(label);
	XtManageChild(docker_server_frame);
	(*server_w) = docker_server_w;
	return VREX_SUCCESS;
}
