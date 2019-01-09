/*
 * statusbar_w.c
 *
 *  Created on: Jan 3, 2019
 *      Author: abhishek
 */
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <pthread.h>
#include "vrex_util.h"

/**
 * Create a new status bar and register as the messageWindow of the main window provided.
 *
 * \param vrex the app context
 * \param main_w the main window widget
 * \return error code
 */
vrex_err_t create_statusbar(vrex_context* vrex, Widget main_w) {
	Widget statusbar, urlbar, statusbar_frame_w, statusbar_layout_w;
	Arg args[10];
	int n = 0;

	// Create the frame for the status bar.
	n = 0;
	XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT);
	n++;
	statusbar_frame_w = XmCreateFrame(main_w, "statusbar_frame_w", args, n);

	// create a row layout form window for items in the status bar
	statusbar_layout_w = XtVaCreateManagedWidget("statusbar_layout_w",
			xmFormWidgetClass, statusbar_frame_w,
			XmNborderWidth, 0,
			XmNshadowThickness, 0,
			XmNfractionBase, 100,
			NULL);

	// create a simple label for messages sent to the status bar.
	n = 0;
	XmString str = XmStringCreateLocalized("V-Rex: starting...");
	XtSetArg(args[n], XmNlabelString, str);
	n++;
	statusbar = XmCreateLabel(statusbar_layout_w, "statusbar_msg", args, n);
	XmStringFree(str);

	XtVaSetValues(statusbar,
	XmNalignment, XmALIGNMENT_BEGINNING,
	XmNtopAttachment, XmATTACH_POSITION,
	XmNtopPosition, 0,
	XmNleftAttachment, XmATTACH_POSITION,
	XmNleftPosition, 0,
	XmNbottomAttachment, XmATTACH_POSITION,
	XmNbottomPosition, 100,
	XmNrightAttachment, XmATTACH_POSITION,
	XmNrightPosition, 80,
	XmNborderWidth, 1,
	NULL);

	// create a label for connection information.
	n = 0;
	XmString url_str = XmStringCreateLocalized("not connected");
	XtSetArg(args[n], XmNlabelString, url_str);
	n++;
	urlbar = XmCreateLabel(statusbar_layout_w, "urlbar_msg", args, n);
	XmStringFree(url_str);

	XtVaSetValues(urlbar,
	XmNalignment, XmALIGNMENT_BEGINNING,
	XmNtopAttachment, XmATTACH_POSITION,
	XmNtopPosition, 0,
	XmNleftAttachment, XmATTACH_POSITION,
	XmNleftPosition, 80,
	XmNbottomAttachment, XmATTACH_POSITION,
	XmNbottomPosition, 100,
	XmNrightAttachment, XmATTACH_POSITION,
	XmNrightPosition, 100,
	XmNborderWidth, 1,
	NULL);

	//add all children as managed.
	XtManageChild(statusbar);
	XtManageChild(urlbar);
	XtManageChild(statusbar_layout_w);
	XtManageChild(statusbar_frame_w);

	//register the status bar frame as the message window for the main window.
	XtVaSetValues(main_w,
	XmNmessageWindow, statusbar_frame_w, NULL);
	return VREX_SUCCESS;
}

/**
 * Find and return the statusbar window.
 */
Widget statusbar_w(vrex_context* vrex) {
	return XtNameToWidget(XtNameToWidget(*(vrex->main_w), "statusbar_frame_w"),
			"statusbar_layout_w");
}

/**
 * Set the message on the status bar.
 *
 * \param vrex app context
 * \param msg string message
 * \return error code.
 */
vrex_err_t set_statusbar_message(vrex_context* vrex, char* msg) {
	if (msg != NULL) {
		Widget statusbar_msg = XtNameToWidget(statusbar_w(vrex),
				"statusbar_msg");
		XmString str = XmStringCreateLocalized(msg);
		XtVaSetValues(statusbar_msg, XmNlabelString, str, NULL);
		XmStringFree(str);
	}
	return VREX_SUCCESS;
}

/**
 * Set the url on the status bar.
 *
 * \param vrex app context
 * \param msg the connection url
 * \return error code.
 */
vrex_err_t set_statusbar_url(vrex_context* vrex, char* url) {
	if (url != NULL) {
		Widget urlbar_msg = XtNameToWidget(statusbar_w(vrex),
				"urlbar_msg");
		XmString str = XmStringCreateLocalized(url);
		XtVaSetValues(urlbar_msg, XmNlabelString, str, NULL);
		XmStringFree(str);
	}
	return VREX_SUCCESS;
}

