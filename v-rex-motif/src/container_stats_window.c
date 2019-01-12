/*
 * v-rex-motif: container_stats_window.c
 * Created on: 11-Jan-2019
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

#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <Xm/Scale.h>
#include <Xm/RowColumn.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <docker_containers.h>
#include "container_stats_window.h"
#include <log.h>

//static pthread_mutex_t stats_on;
static pthread_mutex_t thread_start_lock;
static Widget cpu_scale = NULL, mem_scale = NULL, mem_usage_label = NULL;
static char* id;

void new_value(Widget scale_w, XtPointer client_data, XtPointer call_data) {
	XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;

	printf("%s: %d\n", XtName(scale_w), cbs->value);
}
Widget create_scale(Widget parent, char *title, int max, int min, int value) {
	Arg args[20];
	int n = 0;
	XmString xms = XmStringCreateLocalized(title);
	void new_value(Widget, XtPointer, XtPointer); /* callback for Scale widgets */
	Widget scale;

	XtSetArg(args[n], XmNtitleString, xms);
	n++;
	XtSetArg(args[n], XmNmaximum, max);
	n++;
	XtSetArg(args[n], XmNminimum, min);
	n++;
	XtSetArg(args[n], XmNvalue, value);
	n++;
	XtSetArg(args[n], XmNshowValue, True);
	n++;
	XtSetArg(args[n], XmNeditable, False);
	n++;
	XtSetArg(args[n], XmNdecimalPoints, 2);
	n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL);
	n++;
	XtSetArg(args[n], XmNslidingMode, XmTHERMOMETER);
	n++;

	scale = XmCreateScale(parent, title, args, n);
//	XtAddCallback(scale, XmNvalueChangedCallback, new_value, NULL);
	XtManageChild(scale);

	return scale;
}

/**
 * Create a new container stats window
 *
 * \param vrex the app context
 * \param parent_w the parent window widget
 * \return error code.
 */
vrex_err_t make_docker_container_stats_window(vrex_context* vrex,
		Widget parent_w) {
	Widget container_stats_frame_w, label;
	int n = 0;
	Arg args[10];
	n = 0;
	XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT);
	n++;
	XtSetArg(args[n], XmNmarginWidth, 1);
	n++;
	XtSetArg(args[n], XmNmarginHeight, 1);
	n++;
	container_stats_frame_w = XmCreateFrame(parent_w, "container_stats_frame_w",
			args, n);
	XtVaSetValues(container_stats_frame_w, XmNmarginWidth, 0, XmNmarginHeight,
			0,
			XmNtopAttachment, XmATTACH_POSITION,
			XmNtopPosition, 50,
			XmNleftAttachment, XmATTACH_POSITION,
			XmNleftPosition, 50,
			XmNbottomAttachment, XmATTACH_POSITION,
			XmNbottomPosition, 100,
			XmNrightAttachment, XmATTACH_POSITION,
			XmNrightPosition, 100,
			NULL);
	Widget container_stats_w = XtVaCreateManagedWidget("container_stats_w",
			xmRowColumnWidgetClass, container_stats_frame_w, XmNorientation,
			XmVERTICAL, XmNmarginWidth, 0, XmNmarginHeight, 0,
			NULL);
	n = 0;
	XtSetArg(args[n], XmNframeChildType, XmFRAME_TITLE_CHILD);
	n++;
	XtSetArg(args[n], XmNchildVerticalAlignment, XmALIGNMENT_CENTER);
	n++;
	label = XmCreateLabelGadget(container_stats_frame_w, "Container stats",
			args, n);

	n = 0;
	XmString url_str = XmStringCreateLocalized("Using                 ");
	XtSetArg(args[n], XmNlabelString, url_str);
	n++;
	mem_usage_label = XmCreateLabel(container_stats_w, "mem_usage_label", args,
			n);

	mem_scale = create_scale(container_stats_w, "Mem Usage", 10000, 0, 0);
	cpu_scale = create_scale(container_stats_w, "CPU Usage", 10000, 0, 0);

	XtManageChild(mem_usage_label);
	XtManageChild(label);
	XtManageChild(container_stats_frame_w);

	pthread_mutexattr_t Attr;
	pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutexattr_init(&Attr);
	if (pthread_mutex_init(&thread_start_lock, &Attr) != 0) {
		printf("\n thread_start_lock mutex init has failed\n");
		return 1;
	}

	return VREX_SUCCESS;
}

typedef struct {
	vrex_context* vrex;
	char* id;
} stats_args;

void log_stats(docker_container_stats* stats, void* client_cbargs) {
	stats_args* sargs = (stats_args*) client_cbargs;
	if (strcmp(id, sargs->id) != 0) {
//		printf("%lu is dying\n", pthread_self());
//		fflush(0);
		pthread_exit(0);
	}

	if (stats) {
		docker_container_cpu_stats* cpu_stats =
				docker_container_stats_get_cpu_stats(stats);
		docker_container_mem_stats* mem_stats =
				docker_container_stats_get_mem_stats(stats);
		float mem_usage =
				(docker_container_mem_stats_get_usage(mem_stats) * 1.0)
						/ docker_container_mem_stats_get_limit(mem_stats) * 100
						* 100;
		float cpu_usage = docker_container_stats_get_cpu_usage_percent(stats);
//		docker_log_info("cpu usage%% is %f, mem usage%% is %f, %lu", cpu_usage,
//				mem_usage, docker_container_mem_stats_get_usage(mem_stats));
		XmScaleSetValue(cpu_scale,
				docker_container_stats_get_cpu_usage_percent(stats) * 100);
		char* mem_mesg = (char*) calloc(1024, sizeof(char));
		strcpy(mem_mesg, "Using ");
		strcat(mem_mesg,
				calculate_size(
						docker_container_mem_stats_get_usage(mem_stats)));
		strcat(mem_mesg, "/");
		strcat(mem_mesg,
				calculate_size(
						docker_container_mem_stats_get_limit(mem_stats)));
		XmString mem_usage_str = XmStringCreateLocalized(mem_mesg);
		XmScaleSetValue(mem_scale, mem_usage);
		XtVaSetValues(mem_usage_label, XmNlabelString, mem_usage_str,
		NULL);
		XmStringFree(mem_usage_str);
		XmUpdateDisplay(XtParent(mem_scale));
	}
}

void* show_stats_util(void* args) {
	stats_args* sargs = (stats_args*) args;
	if (cpu_scale && mem_scale) {
		docker_result* res;
		docker_container_get_stats_cb(sargs->vrex->d_ctx, &res, &log_stats,
		sargs, sargs->id);
		sargs->vrex->handle_error(sargs->vrex, res);
	}
	return NULL;
}

/**
 * Show stats for the given container id (must be running).
 *
 * \param vrex the app context
 * \param new_id container id
 * \return error code.
 */
vrex_err_t show_stats_for_container(vrex_context* vrex, char* new_id) {
	int ret;
	pthread_t self = pthread_self();
	ret = pthread_mutex_trylock(&thread_start_lock);
	if (ret == EBUSY) {
//		printf("%lu could not take a lock to shutdown.\n", self);
//		fflush(0);
		return E_INVALID_INPUT;
	}

	id = new_id;

	stats_args* sargs = (stats_args*) calloc(1, sizeof(stats_args));
	sargs->id = id;
	sargs->vrex = vrex;
	pthread_t stats_thread;
	int thread_id = pthread_create(&stats_thread, NULL, &show_stats_util,
			sargs);
//	printf("%lu created thread id %lu...\n", self, stats_thread);
//	fflush(0);

	pthread_mutex_unlock(&thread_start_lock);
	return VREX_SUCCESS;
}

