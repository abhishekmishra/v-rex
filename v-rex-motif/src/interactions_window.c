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
#include <docker_log.h>
#include <time.h>
#include <X11/Intrinsic.h>
#include <Xm/Label.h>
#include <Xm/PanedW.h>
#include <Xm/Text.h>
#include <Xm/List.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include "vrex_util.h"
#include "docker_connection_util.h"

vrex_err_t update_request_form(Widget interactions_pane, time_t request_time,
		char* method, char* request_json, time_t response_time, int http_code,
		char* msg, char* response_json) {
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

	timeinfo = localtime(&response_time);
	char response_time_str[256];
	sprintf(response_time_str, "%s", asctime(timeinfo));
	if (response_time_str != NULL) {
		Widget response_time_text = XtNameToWidget(request_form,
				"Response Received Text");
		XtVaSetValues(response_time_text,
		XmNvalue, response_time_str,
		NULL);
	}

	char http_code_str[128];
	sprintf(http_code_str, "%d", http_code);
	if (http_code_str != NULL) {
		Widget http_code_text = XtNameToWidget(request_form, "HTTP Code Text");
		XtVaSetValues(http_code_text,
		XmNvalue, http_code_str,
		NULL);
	}
//	Widget message_text = XtNameToWidget(request_form, "Message Text");
//	if (msg != NULL) {
//		XtVaSetValues(message_text,
//		XmNvalue, msg,
//		NULL);
//	} else {
//		XtVaSetValues(message_text,
//		XmNvalue, "-",
//		NULL);
//
//	}

	Widget response_text = XtNameToWidget(
			XtNameToWidget(request_form, "Response TextSW"), "Response Text");
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

//vrex_err_t update_response_form(Widget interactions_pane, time_t response_time,
//		int http_code, char* msg, char* response_json) {
//	struct tm * timeinfo;
//	Widget request_form = XtNameToWidget(interactions_pane,
//			"interact_response_form");
//
//	return VREX_SUCCESS;
//}

vrex_err_t create_request_response_form(Widget interactions_pane) {
	Widget request_time_label, request_time_text, http_method_label,
			http_method_text,
			request_label, //message_label, message_text,
			request_text, response_time_label, response_time_text,
			http_code_label, http_code_text, response_label, response_text;
	int n = 0;
	Arg args[10];

	Widget request_form = XmCreateForm(interactions_pane,
			"interact_request_form",
			NULL, 0);

	XtVaSetValues(request_form,
	XmNwidth, 50,
	XmNshadowThickness, 0,
	NULL);

	request_time_label = XtVaCreateManagedWidget("Request Sent",
			xmLabelWidgetClass, request_form,
			XmNtopAttachment, XmATTACH_FORM,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 10,
			XmNtopOffset, 2,
			XmNbottomOffset, 2,
			NULL);

	request_time_text = XtVaCreateManagedWidget("Request Sent Text",
			xmTextWidgetClass, request_form,
			XmNeditable, False,
			XmNvalue, "-",
			XmNcolumns, 50,
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
			XmNcolumns, 50,
			XmNhighlightThickness, 0,
			XmNscrollHorizontal, False,
			XmNcursorPositionVisible, False,
			XmNleftAttachment, XmATTACH_WIDGET,
			XmNleftWidget, request_time_label,
			XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNtopWidget, http_method_label,
			NULL);

	request_label = XtVaCreateManagedWidget("Request", xmLabelWidgetClass,
			request_form,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, http_method_text,
			XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNleftWidget, http_method_label,
			NULL);

	XtSetArg(args[n], XmNscrollHorizontal, False);
	n++;
	XtSetArg(args[n], XmNscrollVertical, False);
	n++;
	XtSetArg(args[n], XmNwordWrap, True);
	n++;
	XtSetArg(args[n], XmNeditable, False);
	n++;

	request_text = XmCreateScrolledText(request_form, "Request Text", args, n);
	XtVaSetValues(XtParent(request_text),
	XmNleftAttachment, XmATTACH_WIDGET,
	XmNleftWidget, request_time_label,
	XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
	XmNtopWidget, request_label,
	NULL);

	XtVaSetValues(request_text,
	XmNeditable, False,
	XmNvalue, "-",
	XmNcolumns, 50,
	XmNrows, 10,
	XmNeditMode, XmMULTI_LINE_EDIT,
	XmNscrollHorizontal, False,
	XmNscrollVertical, True,
	XmNcursorPositionVisible, False,
	XmNwordWrap, True,
	XmNhighlightThickness, 0,
	NULL);

	response_time_label = XtVaCreateManagedWidget("Response Received",
			xmLabelWidgetClass, request_form,
			XmNleftAttachment, XmATTACH_WIDGET,
			XmNleftWidget, request_text,
			XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNtopWidget, request_time_label,
			XmNleftOffset, 10,
			XmNtopOffset, 2,
			XmNbottomOffset, 2,
			NULL);

	response_time_text = XtVaCreateManagedWidget("Response Received Text",
			xmTextWidgetClass, request_form,
			XmNeditable, False,
			XmNvalue, "-",
			XmNcolumns, 50,
			XmNhighlightThickness, 0,
			XmNscrollHorizontal, False,
			XmNcursorPositionVisible, False,
			XmNleftAttachment, XmATTACH_WIDGET,
			XmNleftWidget, response_time_label,
			XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNtopWidget, response_time_label,
			NULL);

	http_code_label = XtVaCreateManagedWidget("HTTP Code", xmLabelWidgetClass,
			request_form,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, response_time_text,
			XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNleftWidget, response_time_label,
			NULL);

	http_code_text = XtVaCreateManagedWidget("HTTP Code Text",
			xmTextWidgetClass, request_form,
			XmNeditable, False,
			XmNvalue, "-",
			XmNcolumns, 50,
			XmNhighlightThickness, 0,
			XmNscrollHorizontal, False,
			XmNcursorPositionVisible, False,
			XmNleftAttachment, XmATTACH_WIDGET,
			XmNleftWidget, response_time_label,
			XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNtopWidget, http_code_label,
			NULL);

//	message_label = XtVaCreateManagedWidget("Message", xmLabelWidgetClass,
//			request_form,
//			XmNtopAttachment, XmATTACH_WIDGET,
//			XmNtopWidget, http_code_text,
//			XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
//			XmNleftWidget, http_code_label,
//			NULL);
//
//	message_text = XtVaCreateManagedWidget("Message Text", xmTextWidgetClass,
//			request_form,
//			XmNeditable, False,
//			XmNvalue, "-",
//			XmNcolumns, 50,
//			XmNrows, 2,
//			XmNeditMode, XmMULTI_LINE_EDIT,
//			XmNscrollHorizontal, False,
//			XmNwordWrap, True,
//			XmNcursorPositionVisible, False,
//			XmNhighlightThickness, 0,
//			XmNleftAttachment, XmATTACH_WIDGET,
//			XmNleftWidget, response_time_label,
//			XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
//			XmNtopWidget, message_label,
//			NULL);

	response_label = XtVaCreateManagedWidget("Response", xmLabelWidgetClass,
			request_form,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, http_code_text,
			XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
			XmNleftWidget, http_code_label,
			NULL);

	n = 0;
	XtSetArg(args[n], XmNscrollHorizontal, False);
	n++;
	XtSetArg(args[n], XmNscrollVertical, False);
	n++;
	XtSetArg(args[n], XmNwordWrap, True);
	n++;
	XtSetArg(args[n], XmNeditable, False);
	n++;

	response_text = XmCreateScrolledText(request_form, "Response Text", args,
			n);
	XtVaSetValues(XtParent(response_text),
	XmNleftAttachment, XmATTACH_WIDGET,
	XmNleftWidget, response_time_label,
	XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
	XmNtopWidget, response_label,
	NULL);

	XtVaSetValues(response_text,
	XmNeditable, False,
	XmNvalue, "-",
	XmNcolumns, 50,
	XmNrows, 10,
	XmNeditMode, XmMULTI_LINE_EDIT,
	XmNscrollHorizontal, False,
	XmNscrollVertical, True,
	XmNcursorPositionVisible, False,
	XmNwordWrap, True,
	XmNhighlightThickness, 0,
	NULL);

	XtManageChild(response_text);
	XtManageChild(request_text);

	XtManageChild(request_form);

	return VREX_SUCCESS;
}

void sel_callback(Widget list_w, XtPointer client_data, XtPointer call_data) {
	XmListCallbackStruct *cbs = (XmListCallbackStruct *) call_data;
	vrex_context* vrex = (vrex_context *) client_data;
	docker_result* res = results_list_get_idx(vrex,
			results_list_length(vrex) - cbs->item_position);

	update_request_form(XtParent(XtParent(list_w)),
			res->start_time,
			res->method, res->request_json_str,
			res->end_time, res->http_error_code,
			res->message, res->response_json_str);
}

vrex_err_t make_interactions_window(vrex_context* vrex) {
	Widget list_w, child, interactions_frame, label;
	int n = 0;
	Arg args[10];

	n = 0;
	XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT);
	n++;
	interactions_frame = XmCreateFrame(*(vrex->main_w), "interactions_frame", args, n);
	XtVaSetValues(*(vrex->main_w), XmNcommandWindow, interactions_frame,
	NULL);

	n = 0;
	XtSetArg(args[n], XmNframeChildType, XmFRAME_TITLE_CHILD);
	n++;
	XtSetArg(args[n], XmNchildVerticalAlignment, XmALIGNMENT_CENTER);
	n++;
	label = XmCreateLabelGadget(interactions_frame, "Docker API Interactions", args, n);

	Widget iw = XmCreateRowColumn(interactions_frame, "interactions_pane", NULL, 0);
	XtVaSetValues(iw, XmNpacking, XmPACK_TIGHT,
	XmNnumColumns, 1,
	XmNnumRows, 2,
	XmNorientation, XmVERTICAL,
	NULL);

	/* Create the ScrolledList */
	list_w = XmCreateScrolledList(iw, "interact_list", NULL, 0);
	/* set the items, the item count, and the visible items */
	XtVaSetValues(list_w,
	XmNvisibleItemCount, 5,
	XmNwidth, 50,
	NULL);
	XtManageChild(list_w);

	/**
	 * Create the Request Form
	 */
	create_request_response_form(iw);

	/* Convenience routines don't create managed children */
	XtManageChild(iw);
	XtManageChild(label);
	XtManageChild(interactions_frame);
	XtAddCallback(list_w, XmNdefaultActionCallback, sel_callback, vrex);
	XtAddCallback(list_w, XmNbrowseSelectionCallback, sel_callback, vrex);
	return VREX_SUCCESS;
}

vrex_err_t add_interactions_entry(vrex_context* vrex, docker_result* res) {
	results_list_add(vrex, res);
	Widget iw = vrex->interactions_w(vrex);
	Widget list_w = XtNameToWidget(XtNameToWidget(iw, "interact_listSW"),
			"interact_list");
	if (res) {
		char* url = res->url;
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

vrex_err_t hide_interactions_window(vrex_context* vrex) {
	Widget iw = vrex->interactions_w(vrex);
	XtUnmanageChild(XtParent(iw));
	return VREX_SUCCESS;
}

vrex_err_t show_interactions_window(vrex_context* vrex) {
	Widget iw = vrex->interactions_w(vrex);
	XtManageChild(XtParent(iw));
	return VREX_SUCCESS;
}
