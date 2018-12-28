/*
 * v-rex: interactions_window.c
 * Created on: 28-Dec-2018
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
#include <X11/Intrinsic.h>
#include <Xm/Label.h>
#include <Xm/PanedW.h>
#include <Xm/Text.h>
#include <Xm/List.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>

#include "vrex_util.h"
#include "docker_connection_util.h"
#include "log.h"

vrex_err_t update_response_form(Widget interactions_pane, int http_code,
		char* msg, char* response_json) {
	Widget response_form = XtNameToWidget(interactions_pane, "interact_response_form");
	char http_code_str[128];
	sprintf(http_code_str, "%d", http_code);
	if(http_code_str != NULL) {
		Widget http_code_text = XtNameToWidget(response_form, "HTTP Code Text");
		XtVaSetValues(http_code_text,
				XmNvalue, http_code_str,
				NULL);
	}
	if(msg != NULL) {
		Widget message_text = XtNameToWidget(response_form, "Message Text");
		XtVaSetValues(message_text,
				XmNvalue, msg,
				NULL);
	}
	return VREX_SUCCESS;
}

vrex_err_t create_resonpse_form(Widget interactions_pane) {
	Widget http_code_label, http_code_text, message_label, message_text;
	Widget response_form = XmCreateForm(interactions_pane,
			"interact_response_form",
			NULL, 0);

	XtVaSetValues(response_form,
	XmNpaneMinimum, 300,
	NULL);

	http_code_label = XtVaCreateManagedWidget("HTTP Code",
			xmLabelWidgetClass, response_form,
			XmNtopAttachment, XmATTACH_FORM,
			XmNleftAttachment, XmATTACH_FORM,
			NULL);

	http_code_text = XtVaCreateManagedWidget("HTTP Code Text",
			xmTextWidgetClass, response_form,
			XmNeditable, False,
			XmNvalue, "-",
			XmNleftAttachment, XmATTACH_WIDGET,
			XmNleftWidget, http_code_label,
			XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNtopWidget, http_code_label,
			NULL);

	message_label = XtVaCreateManagedWidget("Message", xmLabelWidgetClass,
			response_form,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, http_code_text,
			XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNleftWidget, http_code_label,
			NULL);

	message_text = XtVaCreateManagedWidget("Message Text", xmTextWidgetClass,
			response_form,
			XmNeditable, False,
			XmNvalue, "-",
			XmNleftAttachment, XmATTACH_WIDGET,
			XmNleftWidget, http_code_label,
			XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNtopWidget, message_label,
			NULL);

	XtManageChild(http_code_label);
	XtManageChild(http_code_text);
	XtManageChild(response_form);

	return VREX_SUCCESS;
}

vrex_err_t make_interactions_window(vrex_context* vrex) {
	Widget list_w, child;
	Widget iw = XmVaCreatePanedWindow(*(vrex->main_w), "interactions_pane",
	XmNorientation, XmHORIZONTAL,
	NULL);

	/* Create the ScrolledList */
	list_w = XmCreateScrolledList(iw, "interact_list", NULL, 0);
	/* set the items, the item count, and the visible items */
	XtVaSetValues(list_w,
	XmNvisibleItemCount, 10,
	XmNpaneMinimum, 25,
	XmNpaneMaximum, 45,
	NULL);

	/**
	 * Create the Response Form
	 */
	create_resonpse_form(iw);

	/* Convenience routines don't create managed children */
	XtManageChild(iw);
	XtManageChild(list_w);
	docker_log_info(XtName(iw));
	docker_log_info(XtName(XtParent(list_w)));
	return VREX_SUCCESS;
}

vrex_err_t add_interactions_entry(vrex_context* vrex, docker_result* res) {
	Widget iw = vrex->interactions_w(vrex);
	docker_log_error(XtName(iw));
	Widget list_w = XtNameToWidget(XtNameToWidget(iw, "interact_listSW"),
			"interact_list");
	docker_log_error(XtName(list_w));
	if (res) {
		char* url = get_url(res);
		if (url != NULL && (strlen(url) > 0)) {

			XmString msg = XmStringCreateLocalized(url);
			XmListAddItem(list_w, msg, 0);
		}
		update_response_form(iw, get_http_error(res), get_message(res), NULL);
	}
	return VREX_SUCCESS;
}
