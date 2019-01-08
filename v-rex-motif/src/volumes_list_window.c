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
#include "network_list_window.h"

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
	docker_volumes_list_frame_w = XmCreateFrame(parent_w, "docker_volumes_list_frame_w", args, n);

	n = 0;
	XtSetArg(args[n], XmNframeChildType, XmFRAME_TITLE_CHILD);
	n++;
	XtSetArg(args[n], XmNchildVerticalAlignment, XmALIGNMENT_CENTER);
	n++;
	label = XmCreateLabelGadget(docker_volumes_list_frame_w, "Docker Volumes", args, n);

	docker_volumes_list_w = XtVaCreateManagedWidget("docker_volumes_list_w", xbaeMatrixWidgetClass,
			docker_volumes_list_frame_w,
			XmNcolumns, 4,
			XmNrows, 0,
			XmNvisibleColumns, 4,
			XmNfixedColumns, 1,
			XmNheight, 200,
			XmNshadowType, XmSHADOW_ETCHED_OUT,
			XmNcellShadowThickness, 0,
			XmNtextShadowThickness, 0,
			XmNcellHighlightThickness, 1,
			XmNcolumnLabelColor, 0x800000,
			NULL);

//	XtAddCallback(docker_volumes_list_w, XmNlabelActivateCallback, labelCB, NULL);
//	XtAddCallback(docker_volumes_list_w, XmNenterCellCallback, cellCB, NULL);
	XtVaSetValues(docker_volumes_list_frame_w,
	XmNtopAttachment, XmATTACH_POSITION,
	XmNtopPosition, 70,
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

	return VREX_SUCCESS;
}

/**
 * Refresh the network list in the window.
 *
 * \param vrex the app context
 * \return error code
 */
vrex_err_t refresh_volumes_list(vrex_context* vrex) {
	return VREX_SUCCESS;
}
