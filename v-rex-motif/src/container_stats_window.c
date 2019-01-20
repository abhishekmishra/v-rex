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

#include <stdlib.h>
#include <Xm/Scale.h>
#include <Xm/RowColumn.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Form.h>
#include <docker_containers.h>
#include "container_stats_window.h"

#include <docker_log.h>
#include <WScroll.h>

#define STAT_UPDATE_INTERVAL 10000L //in milliseconds

static Widget cpu_scale = NULL, mem_scale = NULL, mem_usage_label = NULL,
		stats_plot_w;

static char* id;

static long cpu_curve, mem_curve;
static time_t start_time = 0;

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

Pixel GetPixel(char *str, Widget wid) {
	Display *dpy = XtDisplay(wid);
	int scr = DefaultScreen(dpy);
	Colormap cmap = DefaultColormap(dpy, scr);
	XColor color, ignore;

	if (XAllocNamedColor(dpy, cmap, str, &color, &ignore))
		return (color.pixel);
	else
		return (BlackPixel(dpy, scr));
}

Widget make_stats_plot_w(Widget parent) {
	int n = 0;
	Arg args[10];
	float scal[4] = { 0, 100., 0, 100. };
	float grid[4] = { 0, 10., 0, 25. };
//	XtSetArg(args[n], XmNleftAttachment, XmATTACH_SELF);
//	n++;
//	XtSetArg(args[n], XmNx, 15);
//	n++;
//	XtSetArg(args[n], XmNtopAttachment, XmATTACH_SELF);
//	n++;
//	XtSetArg(args[n], XmNy, 360);
//	n++;
//	XtSetArg(args[n], XmNrightAttachment, XmATTACH_SELF);
//	n++;
	XtSetArg(args[n], XmNwidth, 250);
	n++;
//	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_SELF);
//	n++;
	XtSetArg(args[n], XmNheight, 50);
	n++;
	XtSetArg(args[n], XwNscrollHistory, True);
	n++;
	XtSetArg(args[n], XwNscrollScale, scal);
	n++;
	XtSetArg(args[n], XwNscrollGridScale, grid);
	n++;
	XtSetArg(args[n], XwNscrollCUcolor, GetPixel("#000000", parent)); //black
	n++;
	XtSetArg(args[n], XwNscrollGridColor, GetPixel("#444444", parent)); //very dark gray
	n++;
	XtSetArg(args[n], XwNscrollAnnotColor, GetPixel("696969", parent)); //DimGray
	n++;
	stats_plot_w = XtCreateWidget("stats_plot_w", XwScrollWidgetClass, parent,
			args, n);

	XtVaSetValues(stats_plot_w,
	XmNtopAttachment, XmATTACH_POSITION,
	XmNtopPosition, 0,
	XmNleftAttachment, XmATTACH_POSITION,
	XmNleftPosition, 0,
	XmNbottomAttachment, XmATTACH_POSITION,
	XmNbottomPosition, 30,
	XmNrightAttachment, XmATTACH_POSITION,
	XmNrightPosition, 100,
	NULL);

	cpu_curve = XwScrollAddcurve(stats_plot_w, GetPixel("blue", parent));
	mem_curve = XwScrollAddcurve(stats_plot_w, GetPixel("green", parent));

	XtManageChild(stats_plot_w);
	return stats_plot_w;
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
			XmNtopPosition, 30,
			XmNleftAttachment, XmATTACH_POSITION,
			XmNleftPosition, 50,
			XmNbottomAttachment, XmATTACH_POSITION,
			XmNbottomPosition, 100,
			XmNrightAttachment, XmATTACH_POSITION,
			XmNrightPosition, 100,
			NULL);
	Widget container_stats_w = XtVaCreateManagedWidget("container_stats_w",
			xmFormWidgetClass, container_stats_frame_w,
			XmNorientation, XmVERTICAL,
			XmNmarginWidth, 0,
			XmNmarginHeight, 0,
			XmNfractionBase, 100,
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

	XtVaSetValues(mem_usage_label,
	XmNtopAttachment, XmATTACH_POSITION,
	XmNtopPosition, 30,
	XmNleftAttachment, XmATTACH_POSITION,
	XmNleftPosition, 0,
	XmNbottomAttachment, XmATTACH_POSITION,
	XmNbottomPosition, 40,
	XmNrightAttachment, XmATTACH_POSITION,
	XmNrightPosition, 100,
	NULL);

	mem_scale = create_scale(container_stats_w, "Mem Usage", 10000, 0, 0);
	XtVaSetValues(mem_scale,
	XmNtopAttachment, XmATTACH_POSITION,
	XmNtopPosition, 40,
	XmNleftAttachment, XmATTACH_POSITION,
	XmNleftPosition, 0,
	XmNbottomAttachment, XmATTACH_POSITION,
	XmNbottomPosition, 70,
	XmNrightAttachment, XmATTACH_POSITION,
	XmNrightPosition, 100,
	NULL);

	cpu_scale = create_scale(container_stats_w, "CPU Usage", 10000, 0, 0);
	XtVaSetValues(cpu_scale,
	XmNtopAttachment, XmATTACH_POSITION,
	XmNtopPosition, 70,
	XmNleftAttachment, XmATTACH_POSITION,
	XmNleftPosition, 0,
	XmNbottomAttachment, XmATTACH_POSITION,
	XmNbottomPosition, 100,
	XmNrightAttachment, XmATTACH_POSITION,
	XmNrightPosition, 100,
	NULL);

	make_stats_plot_w(container_stats_w);

	XtManageChild(mem_usage_label);
	XtManageChild(label);
	XtManageChild(container_stats_frame_w);

	return VREX_SUCCESS;
}

typedef struct {
	vrex_context* vrex;
	char* id;
} stats_args;

void log_stats(XtPointer client_sargs, XtIntervalId* interval_id) {
	docker_container_stats* stats;
	stats_args* sargs = (stats_args*) client_sargs;
	docker_result* res;
	docker_container_get_stats(sargs->vrex->d_ctx, &res, &stats, sargs->id);
	sargs->vrex->handle_error(sargs->vrex, res);

	if (id == NULL | strcmp(id, sargs->id) != 0) {
		docker_log_debug("container id is changed. exiting.\n");
		free(sargs);
		return;
	}

	if (stats) {
		float cpu_xy[2], mem_xy[2];
		time_t now;
		time(&now);

		cpu_xy[0] = now - start_time;
		mem_xy[0] = now - start_time;

		docker_container_cpu_stats* cpu_stats =
				docker_container_stats_get_cpu_stats(stats);
		docker_container_mem_stats* mem_stats =
				docker_container_stats_get_mem_stats(stats);
		float mem_usage =
				(docker_container_mem_stats_get_usage(mem_stats) * 1.0)
						/ docker_container_mem_stats_get_limit(mem_stats) * 100
						* 100;
		mem_xy[1] = mem_usage / 100;
		XwScrollcurve(stats_plot_w, mem_curve, mem_xy, 1);

		float cpu_usage = docker_container_stats_get_cpu_usage_percent(stats)
				* 100;
//		docker_log_info("cpu usage%% is %f, mem usage%% is %f, %lu", cpu_usage,
//				mem_usage, docker_container_mem_stats_get_usage(mem_stats));
		XmScaleSetValue(cpu_scale, cpu_usage);
		cpu_xy[1] = cpu_usage / 100;
		XwScrollcurve(stats_plot_w, cpu_curve, cpu_xy, 1);

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
		free_docker_container_stats(stats);
	}

	XtAppContext app = XtWidgetToApplicationContext(cpu_scale);
	XtAppAddTimeOut(app, STAT_UPDATE_INTERVAL, &log_stats, sargs);
}

/**
 * Show stats for the given container id (must be running).
 *
 * \param vrex the app context
 * \param new_id container id
 * \return error code.
 */
vrex_err_t show_stats_for_container(vrex_context* vrex, char* new_id) {
	XtAppContext app = XtWidgetToApplicationContext(cpu_scale);
	if (start_time == 0) {
		start_time = time(NULL);
	}
	if (new_id == NULL) {
		id = NULL;
	} else if (id == NULL || strcmp(new_id, id) != 0) {
		id = new_id;

		stats_args* sargs = (stats_args*) calloc(1, sizeof(stats_args));
		sargs->id = id;
		sargs->vrex = vrex;

		XtAppAddTimeOut(app, 2000L, &log_stats, sargs);
	}
	return VREX_SUCCESS;
}

