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
#include <X11/Xos.h>
#include <XmHTML/XmHTML.h>
#include "docker_server_window.h"
#include "docker_system.h"
#include "log.h"

void create_summary_scrolled_text(Widget* dst, Widget docker_server_w) {
	int n;
	Arg args[10];
	n = 0;
	XtSetArg(args[n], XmNrows, 25);
	n++;
	XtSetArg(args[n], XmNcolumns, 80);
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
	Widget docker_summary_text = XmCreateScrolledText(docker_server_w,
			"Docker Server Summary", args, n);
	XtManageChild(docker_summary_text);
	XtVaSetValues(docker_summary_text, XmNvalue, "-", XmNcursorPositionVisible,
	False, XmNhighlightThickness, 0, XmNshadowThickness, 0, NULL);
	XtManageChild(docker_server_w);
	(*dst) = docker_summary_text;
}

void updated_events_table(Widget html, vrex_context* vrex, docker_result* res) {
	//TODO: test events call
	array_list* evts;
	time_t now = time(NULL);
	docker_system_events(vrex->d_ctx, &res, &evts, now - (3600 * 24), now);
	vrex->handle_error(vrex, res);
	String content = (char*) calloc(2048, sizeof(char));
	strcpy(content,
			"<html><body><h5>Server events in the last 24 hours</h5><font size=\"3\"><table border=1>");
	strcat(content,
			"<tr><td><b>Time</b></td><td><b>Type</b></td><td><b>Action</b></td></tr>");
	int evts_len = array_list_length(evts);
	docker_log_debug("Num events %d", evts_len);
	for (int i = 0; i < evts_len; i++) {
		docker_event* event = array_list_get_idx(evts, i);
		strcat(content, "<tr><td>");
		struct tm* timeinfo = localtime(&event->time);
		char evt_time_str[256];
		sprintf(evt_time_str, "%s", asctime(timeinfo));
		strcat(content, evt_time_str);
		strcat(content, "</td><td>");
		strcat(content, event->type);
		strcat(content, "</td><td>");
		strcat(content, event->action);
		//		strcat(content, "</td><td>");
		//
		//		strcat(content, event->actor_id);
		//		strcat(content, "</td><td>");
		//
		//		strcat(content, json_object_get_string(event->actor_attributes));
		strcat(content, "</td></tr>");
	}
	strcat(content, "</table></font></body></html>");
	XmHTMLTextSetString(html, content);
}

void create_events_table(Widget* events_table, Widget docker_server_w) {
	(*events_table) = XtVaCreateManagedWidget("events_table", xmHTMLWidgetClass,
			docker_server_w, XmNmarginWidth, 20, XmNmarginHeight, 20, XmNwidth,
			600, XmNheight, 500, NULL);
}

vrex_err_t make_docker_server_window(vrex_context* vrex, Widget* server_w) {
	Widget list_w, child, docker_summary_text, docker_summary_scrolled_w;

	Widget docker_server_w = XmCreateRowColumn(
			XtNameToWidget(*(vrex->main_w), "main_form_w"), "docker_server",
			NULL, 0);
	XtVaSetValues(docker_server_w, XmNpacking, XmPACK_TIGHT,
	XmNnumColumns, 2,
	XmNnumRows, 1,
	XmNorientation, XmHORIZONTAL,
	XmNtopAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	XmNleftOffset, 2,
	XmNtopOffset, 2,
	XmNbottomOffset, 2,
	NULL);

	//TODO: test info call
	docker_result* res;
	docker_info* info;
	docker_system_info(vrex->d_ctx, &res, &info);
	vrex->handle_error(vrex, res);

	char* summary = (char*) calloc(1024, sizeof(char));
	sprintf(summary,
			"Containers: %lu (Running: %lu, Paused: %lu, Stopped: %lu)\n",
			info->containers, info->containers_running, info->containers_paused,
			info->containers_stopped);

	create_summary_scrolled_text(&docker_summary_text, docker_server_w);

	XtVaSetValues(docker_summary_text,
	XmNvalue, summary, NULL);

	Widget events_table;
	create_events_table(&events_table, docker_server_w);

	updated_events_table(events_table, vrex, res);
	return VREX_SUCCESS;
}
