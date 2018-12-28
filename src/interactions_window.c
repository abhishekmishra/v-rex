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
#include <time.h>
#include <X11/Intrinsic.h>
#include <Xm/Label.h>
#include <Xm/PanedW.h>
#include <Xm/Text.h>
#include <Xm/List.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include "vrex_util.h"
#include "docker_connection_util.h"
#include "log.h"

vrex_err_t update_request_form(Widget interactions_pane, time_t request_time,
		char* method, char* request_json) {
	struct tm * timeinfo;
	Widget request_form = XtNameToWidget(interactions_pane,
			"interact_request_form");
	timeinfo = localtime(&request_time);
	char request_time_str[256];
	sprintf(request_time_str, "%s", asctime(timeinfo));
	if (request_time_str != NULL) {
		Widget request_time_text = XtNameToWidget(request_form,
				"Request Sent Text");
		XtVaSetValues(request_time_text,
		XmNvalue, request_time_str,
		NULL);
	}

	Widget http_method_text = XtNameToWidget(request_form, "HTTP Method Text");
	if (method != NULL) {
		XtVaSetValues(http_method_text,
		XmNvalue, method,
		NULL);
	} else {
		XtVaSetValues(http_method_text,
		XmNvalue, "-",
		NULL);

	}

	Widget request_text = XtNameToWidget(
			XtNameToWidget(request_form, "Request TextSW"), "Request Text");
	json_object* request_obj = NULL;
	if (request_json != NULL) {
		request_obj = json_tokener_parse(request_json);
		XtVaSetValues(request_text,
		XmNvalue, json_object_to_json_string_ext(request_obj,
		JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_SPACED),
		NULL);

	} else {
		XtVaSetValues(request_text,
		XmNvalue, "-",
		NULL);
	}

	return VREX_SUCCESS;
}

vrex_err_t update_response_form(Widget interactions_pane, time_t response_time,
		int http_code, char* msg, char* response_json) {
	struct tm * timeinfo;
	Widget response_form = XtNameToWidget(interactions_pane,
			"interact_response_form");
	timeinfo = localtime(&response_time);
	char response_time_str[256];
	sprintf(response_time_str, "%s", asctime(timeinfo));
	if (response_time_str != NULL) {
		Widget response_time_text = XtNameToWidget(response_form,
				"Response Received Text");
		XtVaSetValues(response_time_text,
		XmNvalue, response_time_str,
		NULL);
	}

	char http_code_str[128];
	sprintf(http_code_str, "%d", http_code);
	if (http_code_str != NULL) {
		Widget http_code_text = XtNameToWidget(response_form, "HTTP Code Text");
		XtVaSetValues(http_code_text,
		XmNvalue, http_code_str,
		NULL);
	}
	Widget message_text = XtNameToWidget(response_form, "Message Text");
	if (msg != NULL) {
		XtVaSetValues(message_text,
		XmNvalue, msg,
		NULL);
	} else {
		XtVaSetValues(message_text,
		XmNvalue, "-",
		NULL);

	}

	Widget response_text = XtNameToWidget(
			XtNameToWidget(response_form, "Response TextSW"), "Response Text");
	json_object* response_obj = NULL;
	if (response_json != NULL) {
		response_obj = json_tokener_parse(response_json);
		XtVaSetValues(response_text,
		XmNvalue, json_object_to_json_string_ext(response_obj,
		JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_SPACED),
		NULL);

	} else {
		XtVaSetValues(response_text,
		XmNvalue, "-",
		NULL);
	}

	return VREX_SUCCESS;
}

vrex_err_t create_request_form(Widget interactions_pane) {
	Widget request_time_label, request_time_text, http_method_label,
			http_method_text, message_label, message_text, request_label,
			request_text;
	Widget request_form = XmCreateForm(interactions_pane,
			"interact_request_form",
			NULL, 0);

	request_time_label = XtVaCreateManagedWidget("Request Sent",
			xmLabelWidgetClass, request_form,
			XmNtopAttachment, XmATTACH_FORM,
			XmNleftAttachment, XmATTACH_FORM,
			NULL);

	request_time_text = XtVaCreateManagedWidget("Request Sent Text",
			xmTextWidgetClass, request_form,
			XmNeditable, False,
			XmNvalue, "-",
			XmNcolumns, 40,
			XmNhighlightThickness, 0,
			XmNscrollHorizontal, False,
			XmNcursorPositionVisible, False,
			XmNleftAttachment, XmATTACH_WIDGET,
			XmNleftWidget, request_time_label,
			XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNtopWidget, request_time_label,
			NULL);

	http_method_label = XtVaCreateManagedWidget("HTTP Method",
			xmLabelWidgetClass, request_form,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, request_time_text,
			XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNleftWidget, request_time_label,
			NULL);

	http_method_text = XtVaCreateManagedWidget("HTTP Method Text",
			xmTextWidgetClass, request_form,
			XmNeditable, False,
			XmNvalue, "-",
			XmNcolumns, 40,
			XmNhighlightThickness, 0,
			XmNscrollHorizontal, False,
			XmNcursorPositionVisible, False,
			XmNleftAttachment, XmATTACH_WIDGET,
			XmNleftWidget, request_time_label,
			XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNtopWidget, http_method_label,
			NULL);

//	message_label = XtVaCreateManagedWidget("Message", xmLabelWidgetClass,
//			request_form,
//			XmNtopAttachment, XmATTACH_WIDGET,
//			XmNtopWidget, http_method_text,
//			XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
//			XmNleftWidget, http_method_label,
//			NULL);
//
//	message_text = XtVaCreateManagedWidget("Message Text", xmTextWidgetClass,
//			request_form,
//			XmNeditable, False,
//			XmNvalue, "-",
//			XmNcolumns, 40,
//			XmNrows, 2,
//			XmNeditMode, XmMULTI_LINE_EDIT,
//			XmNscrollHorizontal, False,
//			XmNwordWrap, True,
//			XmNcursorPositionVisible, False,
//			XmNhighlightThickness, 0,
//			XmNleftAttachment, XmATTACH_WIDGET,
//			XmNleftWidget, request_time_label,
//			XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
//			XmNtopWidget, message_label,
//			NULL);
//
	request_label = XtVaCreateManagedWidget("Request", xmLabelWidgetClass,
			request_form,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, http_method_text,
			XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNleftWidget, http_method_label,
			NULL);

	request_text = XmCreateScrolledText(request_form, "Request Text", NULL, 0);
	XtVaSetValues(XtParent(request_text),
	XmNleftAttachment, XmATTACH_WIDGET,
	XmNleftWidget, request_time_label,
	XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
	XmNtopWidget, request_label,
	NULL);

	XtVaSetValues(request_text,
	XmNeditable, False,
	XmNvalue, "-",
	XmNcolumns, 40,
	XmNrows, 10,
	XmNeditMode, XmMULTI_LINE_EDIT,
	XmNscrollHorizontal, False,
	XmNscrollVertical, True,
	XmNcursorPositionVisible, False,
	XmNwordWrap, True,
	XmNhighlightThickness, 0,
	NULL);

	XtManageChild(request_text);
	XtManageChild(request_form);

//	XtWidgetGeometry size;
//	size.request_mode = CWHeight;
//	XtQueryGeometry(request_form, NULL, &size);
//	XtVaSetValues(request_form, XmNpaneMaximum, size.height,
//	XmNpaneMinimum, size.height, NULL);
//
	return VREX_SUCCESS;
}

vrex_err_t create_response_form(Widget interactions_pane) {
	Widget response_time_label, response_time_text, http_code_label,
			http_code_text, message_label, message_text, response_label,
			response_text;
	Widget response_form = XmCreateForm(interactions_pane,
			"interact_response_form",
			NULL, 0);

	response_time_label = XtVaCreateManagedWidget("Response Received",
			xmLabelWidgetClass, response_form,
			XmNtopAttachment, XmATTACH_FORM,
			XmNleftAttachment, XmATTACH_FORM,
			NULL);

	response_time_text = XtVaCreateManagedWidget("Response Received Text",
			xmTextWidgetClass, response_form,
			XmNeditable, False,
			XmNvalue, "-",
			XmNcolumns, 40,
			XmNhighlightThickness, 0,
			XmNscrollHorizontal, False,
			XmNcursorPositionVisible, False,
			XmNleftAttachment, XmATTACH_WIDGET,
			XmNleftWidget, response_time_label,
			XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNtopWidget, response_time_label,
			NULL);

	http_code_label = XtVaCreateManagedWidget("HTTP Code", xmLabelWidgetClass,
			response_form,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, response_time_text,
			XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNleftWidget, response_time_label,
			NULL);

	http_code_text = XtVaCreateManagedWidget("HTTP Code Text",
			xmTextWidgetClass, response_form,
			XmNeditable, False,
			XmNvalue, "-",
			XmNcolumns, 40,
			XmNhighlightThickness, 0,
			XmNscrollHorizontal, False,
			XmNcursorPositionVisible, False,
			XmNleftAttachment, XmATTACH_WIDGET,
			XmNleftWidget, response_time_label,
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
			XmNcolumns, 40,
			XmNrows, 2,
			XmNeditMode, XmMULTI_LINE_EDIT,
			XmNscrollHorizontal, False,
			XmNwordWrap, True,
			XmNcursorPositionVisible, False,
			XmNhighlightThickness, 0,
			XmNleftAttachment, XmATTACH_WIDGET,
			XmNleftWidget, response_time_label,
			XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNtopWidget, message_label,
			NULL);

	response_label = XtVaCreateManagedWidget("Response", xmLabelWidgetClass,
			response_form,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, message_text,
			XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNleftWidget, message_label,
			NULL);

	response_text = XmCreateScrolledText(response_form, "Response Text", NULL,
			0);
	XtVaSetValues(XtParent(response_text),
	XmNleftAttachment, XmATTACH_WIDGET,
	XmNleftWidget, response_time_label,
	XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
	XmNtopWidget, response_label,
	NULL);

	XtVaSetValues(response_text,
	XmNeditable, False,
	XmNvalue, "-",
	XmNcolumns, 40,
	XmNrows, 10,
	XmNeditMode, XmMULTI_LINE_EDIT,
	XmNscrollHorizontal, False,
	XmNscrollVertical, True,
	XmNcursorPositionVisible, False,
	XmNwordWrap, True,
	XmNhighlightThickness, 0,
	NULL);

	XtManageChild(response_text);
	XtManageChild(response_form);

//	XtWidgetGeometry size;
//	size.request_mode = CWHeight;
//	XtQueryGeometry(response_form, NULL, &size);
//	XtVaSetValues(response_form, XmNpaneMaximum, size.height,
//	XmNpaneMinimum, size.height, NULL);
//
	return VREX_SUCCESS;
}

void sel_callback(Widget list_w, XtPointer client_data, XtPointer call_data) {
	XmListCallbackStruct *cbs = (XmListCallbackStruct *) call_data;
	vrex_context* vrex = (vrex_context *) client_data;
//	docker_log_debug("result len %d, selection pos %d, pos in list %d",
//			results_list_length(vrex), cbs->item_position,
//			results_list_length(vrex) - cbs->item_position);
	docker_result* res = results_list_get_idx(vrex,
			results_list_length(vrex) - cbs->item_position);

	update_response_form(XtParent(XtParent(list_w)),
			get_docker_result_end_time(res), get_docker_result_http_error(res),
			get_docker_result_message(res), get_docker_result_response(res));
	update_request_form(XtParent(XtParent(list_w)),
			get_docker_result_start_time(res),
			get_docker_result_http_method(res), get_docker_result_request(res));
}

vrex_err_t make_interactions_window(vrex_context* vrex) {
	Widget list_w, child;
	XtWidgetGeometry size;

	Widget iw = XmVaCreatePanedWindow(*(vrex->main_w), "interactions_pane",
	XmNorientation, XmHORIZONTAL,
	NULL);

	docker_log_debug("parent of interactions window is %s", XtName(XtParent(iw)));

	/* Create the ScrolledList */
	list_w = XmCreateScrolledList(iw, "interact_list", NULL, 0);
	/* set the items, the item count, and the visible items */
	XtVaSetValues(list_w,
	XmNvisibleItemCount, 10,
	NULL);
	XtManageChild(list_w);

	size.request_mode = CWHeight;
	XtQueryGeometry(list_w, NULL, &size);
	XtVaSetValues(list_w, XmNpaneMaximum, size.height,
	XmNpaneMinimum, size.height, NULL);

	/**
	 * Create the Request Form
	 */
	create_request_form(iw);

	/**
	 * Create the Response Form
	 */
	create_response_form(iw);

	/* Convenience routines don't create managed children */
	XtManageChild(iw);
	XtAddCallback(list_w, XmNdefaultActionCallback, sel_callback, vrex);
	XtAddCallback(list_w, XmNbrowseSelectionCallback, sel_callback, vrex);
	return VREX_SUCCESS;
}

void MakePosVisible(Widget list_w, int item_no) {
	int top, visible;
	XtVaGetValues(list_w, XmNtopItemPosition, &top,
	XmNvisibleItemCount, &visible, NULL);
	if (item_no < top)
		XmListSetPos(list_w, item_no);
	else if (item_no >= top + visible)
		XmListSetBottomPos(list_w, item_no);
}

vrex_err_t add_interactions_entry(vrex_context* vrex, docker_result* res) {
	results_list_add(vrex, res);
	Widget iw = vrex->interactions_w(vrex);
	docker_log_error(XtName(iw));
	Widget list_w = XtNameToWidget(XtNameToWidget(iw, "interact_listSW"),
			"interact_list");
	docker_log_error(XtName(list_w));
	if (res) {
		char* url = get_docker_result_url(res);
		if (url != NULL && (strlen(url) > 0)) {
			int pos_to_add;
			XtVaGetValues(list_w, XmNtopItemPosition, &pos_to_add, NULL);
			XmString msg = XmStringCreateLocalized(url);
			XmListAddItemUnselected(list_w, msg, pos_to_add);
			XmListSelectPos(list_w, pos_to_add, True);
			MakePosVisible(list_w, pos_to_add);
		}
	}
	return VREX_SUCCESS;
}