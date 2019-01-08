/*
 * v-rex-motif: events_window.c
 * Created on: 02-Jan-2019
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
#include <X11/Intrinsic.h>
#include <Xm/List.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>
#include <docker_system.h>
#include <log.h>
#include "events_window.h"

void docker_events_cb(docker_event* event, void* cbargs) {
	Widget* events_w = (Widget*) cbargs;
	String content = (char*) calloc(2048, sizeof(char));
	strcat(content, "");
	struct tm* timeinfo = localtime(&event->time);
	char evt_time_str[256];
	//sprintf(evt_time_str, "%s", asctime(timeinfo));
	strftime(evt_time_str, 255, "%d-%m-%Y:%H:%M:%S", timeinfo);
	strcat(content, evt_time_str);
	strcat(content, ": ");
	strcat(content, event->type);
	strcat(content, " | ");
	strcat(content, event->action);
	//		strcat(content, "</td><td>");
	//
	strcat(content, " | ");
	strcat(content, event->actor_id);
	//		strcat(content, "</td><td>");
	//
	//		strcat(content, json_object_get_string(event->actor_attributes));
	strcat(content, "");
	int pos_to_add = 1;
//	XtVaGetValues((*events_w), XmNtopItemPosition, &pos_to_add, NULL);
	XmString msg = XmStringCreateLocalized(content);
	XmListAddItemUnselected((*events_w), msg, pos_to_add);
	XmListSelectPos((*events_w), pos_to_add, True);
	MakePosVisible((*events_w), pos_to_add);
	XmUpdateDisplay((*events_w));
}

void* update_events_table_cb(void* args) {
	docker_result* res;
	array_list* evts;
	time_t now = time(NULL);
	vrex_context* vrex = (vrex_context*) args;
	docker_system_events_cb(vrex->d_ctx, &res, &docker_events_cb,
			vrex->events_w, &evts, now - (3600 * 24), 0);
	return NULL;
}

void* update_events_table(void* args) {
	docker_result* res;
	array_list* evts;
	time_t now = time(NULL);
	vrex_context* vrex = (vrex_context*) args;
	Widget events_w = *(vrex->events_w);
	docker_system_events(vrex->d_ctx, &res, &evts, now - (3600 * 24), now);
	vrex->handle_error(vrex, res);

	int evts_len = array_list_length(evts);
	docker_log_debug("Num events %d", evts_len);
	for (int i = 0; i < evts_len; i++) {
		docker_event* event = array_list_get_idx(evts, i);
		String content = (char*) calloc(2048, sizeof(char));
		strcat(content, "");
		struct tm* timeinfo = localtime(&event->time);
		char evt_time_str[256];
		//sprintf(evt_time_str, "%s", asctime(timeinfo));
		strftime(evt_time_str, 255, "%d-%m-%Y:%H:%M:%S", timeinfo);
		strcat(content, evt_time_str);
		strcat(content, ": ");
		strcat(content, event->type);
		strcat(content, " | ");
		strcat(content, event->action);
		//		strcat(content, "</td><td>");
		//
		strcat(content, " | ");
		strcat(content, event->actor_id);
		//		strcat(content, "</td><td>");
		//
		//		strcat(content, json_object_get_string(event->actor_attributes));
		strcat(content, "");
		int pos_to_add;
		XtVaGetValues(events_w, XmNtopItemPosition, &pos_to_add, NULL);
		XmString msg = XmStringCreateLocalized(content);
		XmListAddItemUnselected(events_w, msg, pos_to_add);
		XmListSelectPos(events_w, pos_to_add, True);
		MakePosVisible(events_w, pos_to_add);
	}
	return NULL;
}

vrex_err_t make_docker_events_window(vrex_context* vrex, Widget server_w,
		Widget* events_w) {
	Widget docker_server_summary_textSW = XtNameToWidget(server_w,
			"docker_server_summary_textSW");
	Widget events_frame_w, label;
	int n = 0;
	Arg args[10];

	n = 0;
	XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT);
	n++;
	XtSetArg(args[n], XmNmarginWidth, 1);
	n++;
	XtSetArg(args[n], XmNmarginHeight, 1);
	n++;
	events_frame_w = XmCreateFrame(server_w, "events_frame_w", args, n);

	n = 0;
	XtSetArg(args[n], XmNframeChildType, XmFRAME_TITLE_CHILD);
	n++;
	XtSetArg(args[n], XmNchildVerticalAlignment, XmALIGNMENT_CENTER);
	n++;
	label = XmCreateLabelGadget(events_frame_w, "Docker Events", args, n);

	(*events_w) = XmCreateScrolledList(events_frame_w, "events_w",
	NULL, 0);
	XtVaSetValues((*events_w),
	XmNvisibleItemCount, 5,
	XmNwidth, 50,
	XmNshadowThickness, 0,
	NULL);

	XtVaSetValues(events_frame_w,
	//Need to attach to the bottom right of the summary text window.
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, docker_server_summary_textSW,
			XmNleftAttachment, XmATTACH_POSITION,
			XmNleftPosition, 70,
			XmNbottomAttachment, XmATTACH_POSITION,
			XmNbottomPosition, 50,
			XmNrightAttachment, XmATTACH_POSITION,
			XmNrightPosition, 100,
			NULL);

	XtManageChild((*events_w));
	XtManageChild(label);
	XtManageChild(events_frame_w);

	vrex->events_w = events_w;

	pthread_t docker_ping_thread;
	int thread_id = pthread_create(&docker_ping_thread, NULL,
			&update_events_table_cb, vrex);
//	update_events_table(vrex);
	return E_SUCCESS;
}
