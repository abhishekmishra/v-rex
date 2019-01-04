/*
 * v-rex-motif: messages_window.c
 * Created on: 04-Jan-2019
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

#include <pthread.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>
#include <log.h>
#include "messages_window.h"

static pthread_mutex_t messages_w_lock;

/**
 * Create a new messages list window.
 *
 * \param vrex the app context
 * \param parent_w the parent window widget
 * \return error code.
 */
vrex_err_t make_docker_messages_window(vrex_context* vrex, Widget parent_w) {
	Widget messages_frame_w, label;
	Widget messages_w;
	int n = 0;
	Arg args[10];

	n = 0;
	XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT);
	n++;
	XtSetArg(args[n], XmNmarginWidth, 1);
	n++;
	XtSetArg(args[n], XmNmarginHeight, 1);
	n++;
	messages_frame_w = XmCreateFrame(parent_w, "messages_frame_w", args, n);

	n = 0;
	XtSetArg(args[n], XmNframeChildType, XmFRAME_TITLE_CHILD);
	n++;
	XtSetArg(args[n], XmNchildVerticalAlignment, XmALIGNMENT_CENTER);
	n++;
	label = XmCreateLabelGadget(messages_frame_w, "Docker Messages", args, n);

	messages_w = XmCreateScrolledList(messages_frame_w, "messages_w",
	NULL, 0);
	XtVaSetValues(messages_w,
	XmNvisibleItemCount, 5,
	XmNwidth, 50,
	XmNshadowThickness, 0,
	NULL);

	XtVaSetValues(messages_frame_w,
	XmNtopAttachment, XmATTACH_POSITION,
	XmNtopPosition, 50,
	XmNleftAttachment, XmATTACH_POSITION,
	XmNleftPosition, 70,
	XmNbottomAttachment, XmATTACH_POSITION,
	XmNbottomPosition, 100,
	XmNrightAttachment, XmATTACH_POSITION,
	XmNrightPosition, 100,
	NULL);

	XtManageChild(messages_w);
	XtManageChild(label);
	XtManageChild(messages_frame_w);

	return VREX_SUCCESS;
}

Widget get_messages_window(vrex_context* vrex) {
	return XtNameToWidget(
			XtNameToWidget(
					XtNameToWidget(
							XtNameToWidget(
									XtNameToWidget(*(vrex->main_w),
											"docker_server_frame_w"),
									"docker_server_w"), "messages_frame_w"),
					"messages_wSW"), "messages_w");
}

/**
 * Add a new messages entry
 *
 * \param vrex the app context
 * \param msg to add to the messages list
 * \return error code
 */
vrex_err_t add_messages_entry(vrex_context* vrex, char* content) {
	int ret = pthread_mutex_lock(&messages_w_lock);

	Widget messages_w = get_messages_window(vrex);
	int pos_to_add;
	XtVaGetValues(messages_w, XmNtopItemPosition, &pos_to_add, NULL);
	XmString msg = XmStringCreateLocalized(content);
	XmListAddItemUnselected(messages_w, msg, pos_to_add);
	XmListSelectPos(messages_w, pos_to_add, True);
	MakePosVisible(messages_w, pos_to_add);

	pthread_mutex_unlock(&messages_w_lock);
	return VREX_SUCCESS;
}

