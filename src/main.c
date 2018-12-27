#ifdef HAVE_CONFIG_H
#include <XbaeConfig.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <curl/curl.h>
//#ifdef USE_EDITRES
#include <X11/Intrinsic.h>
#include <X11/Xmu/Editres.h>
//#endif

#include <Xm/MainW.h>
#include <Xm/Label.h>
#include <Xbae/Matrix.h>
#include <Xm/CascadeB.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/MessageB.h>

#include "docker_connection_util.h"
#include "docker_system.h"
#include "vrex_util.h"
#include "container_list_window.h"

#include <log.h>

#define USE_RENDER_TABLE 0

#define VREX_X_HAS_NO_THREADS -2

//#define VREX_USE_THREADS 0

static String fallback[] = { "V-Rex*main_w.width:		1024",
		"V-Rex*.background:		#A9A9A9", "V-Rex*.foreground:		#000000",
		"V-Rex*main_w.height:		768",
//		"V-Rex*mw.shadowType:		SHADOW_ETCHED_OUT",
//		"V-Rex*mw.shadowThickness:		1", "V-Rex*mw.cellShadowThickness:	1",
//		"V-Rex*mw.gridType:		GRID_CELL_LINE",
//		"V-Rex*mw.cellShadowType:		shadow_in", "V-Rex*mw.visibleColumns:		4",
//		"V-Rex*mw.visibleRows:		0", "V-Rex*mw.rows:			0",
//		"V-Rex*mw.columns:			4", "V-Rex*mw.fixedRows:		0",
//		"V-Rex*mw.fixedColumns:		1", "V-Rex*mw.trailingFixedRows:	0",
//		"V-Rex*mw.trailingFixedColumns:	0", "V-Rex*mw.traverseFixedCells:	True",
//		"V-Rex*mw.multiLineCell:	True",
//		"V-Rex*mw.wrapType: 	wrap_continuous", //other option is wrap_word
//		"V-Rex*mw.fill: 	True", "V-Rex*mw.horzFill: 	True",
//		"V-Rex*mw.vertFill: 	True", "V-Rex*mw.height: 	800",
//		"V-Rex*mw.width: 	1024", "V-Rex*mw.gridLineColor: 	#A0A0A0",
//		"V-Rex*mw.background: 	#111111", "V-Rex*mw.foreground: 	#D3D3D3",
//		"V-Rex*mw.columnLabelColor: 	#a0a0ff",
//		"V-Rex*mw.highlightColor: 	#6495ED", "V-Rex*mw.rowHeight: 	200",

//				"V-Rex*mw.columnWidths:		10, 5, 10, 5, 10, 5,"
//						"					10, 5, 10, 5, 10, 5",
//				"V-Rex*mw.columnLabels:		Zero, One, Two, Three, Four,"
//						"					Five, Six, Seven, Eight, Nine",
//				"V-Rex*mw.rowLabels:		0, 1, 2, 3, 4, 5, 6, 7, 8, 9",
		"V-Rex*mw.fontList: -*-lucida-medium-r-*-*-14-*-*-*-*-*-*-*",
		"V-Rex*mw.labelFont: -*-lucida-bold-r-*-*-14-*-*-*-*-*-*-*",
//		"V-Rex*mw.cellShadowThickness:		0", "V-Rex*mw.textShadowThickness:		0",
//		"V-Rex*mw.cellHighlightThickness:		2", "V-Rex*mw.cellMarginHeight:		0",
//		"V-Rex*mw.cellMarginWidth:		1",
		NULL };

void quit_call()

{
	docker_log_info("Quitting program\n");
	exit(0);
}

void help_call()

{
	docker_log_info("Sorry, I'm Not Much Help\n");
}

void refresh_call(Widget widget, XtPointer client_data, XtPointer call_data) {
	docker_context* ctx = (docker_context*) client_data;
	XtSetSensitive(widget, False);
	Widget top = XtParent(XtParent(widget));
	load_containers_list(
			XtNameToWidget(
					XtNameToWidget(XtNameToWidget(top, "main_form_w"),
							"container_list_toplevel"), "mw"), ctx);
	docker_log_debug("Refresh button name - %s", XtName(widget));
}

void docker_version_show(Widget widget, XtPointer client_data,
		XtPointer call_data) {
	docker_context* ctx = (docker_context*) client_data;
	docker_result* res;
	docker_version* version;
	Widget dialog;
	Arg arg[5];
	XmString xms;
	int n = 0;

	if (docker_system_version(ctx, &res, &version) == E_SUCCESS) {
		char* version_msg = (char*) XtCalloc(1024, sizeof(char));
		sprintf(version_msg, "Docker Version: %s\n"
				"OS (Kernel): %s (%s)\n"
				"Arch: %s\n"
				"API Version: %s\n"
				"Min API Version: %s\n"
				"Go Version: %s\n"
				"Git Commit: %s\n"
				"Build Time: %s\n"
				"Experimental (1 means True): %d", version->version,
				version->os, version->kernel_version, version->arch,
				version->api_version, version->min_api_version,
				version->go_version, version->git_commit, version->build_time,
				version->experimental);
		xms = XmStringCreateLocalized(version_msg);
	} else {
		xms =
				XmStringCreateLocalized(
						"Error: could not fetch version information from docker server.");
	}
	XtSetArg(arg[n], XmNmessageString, xms);
	n++;
	XtSetArg(arg[n], XmNdialogType, XmDIALOG_INFORMATION);
	n++;
	dialog = XmCreateMessageDialog(widget, "V-Rex: docker version info", arg,
			n);
	XtManageChild(dialog);
	XtUnmanageChild(XtNameToWidget(dialog, "Cancel"));
	XtUnmanageChild(XtNameToWidget(dialog, "Help"));
	XmStringFree(xms);
}

void create_menubar(Widget main_w, docker_context* ctx) {
	Widget menu_bar, quit, docker_version, help;
	Arg arg[1];
	menu_bar = XmCreateMenuBar(main_w, "main_list", NULL, 0);
	XtManageChild(menu_bar);

	/* create quit widget + callback */

	quit = XtVaCreateManagedWidget("Quit", xmCascadeButtonWidgetClass, menu_bar,
	XmNmnemonic, 'Q',
	NULL);

	XtAddCallback(quit, XmNactivateCallback, quit_call, NULL);

	docker_version = XtVaCreateManagedWidget("Docker Version",
			xmCascadeButtonWidgetClass, menu_bar,
			XmNmnemonic, 'V',
			NULL);

	XtAddCallback(quit, XmNactivateCallback, quit_call, NULL);
	XtAddCallback(docker_version, XmNactivateCallback, docker_version_show,
			ctx);

//	/* create help widget + callback */
//
//	help = XtVaCreateManagedWidget("Help", xmCascadeButtonWidgetClass, menu_bar,
//	XmNmnemonic, 'H',
//	NULL);
//
//	XtAddCallback(help, XmNactivateCallback, help_call, NULL);
//
//	/* Tell the menubar which button is the help menu  */
//
//	XtSetArg(arg[0], XmNmenuHelpWidget, help);
//	XtSetValues(menu_bar, arg, 1);
	XtVaSetValues(main_w,
	XmNmenuBar, menu_bar, NULL);
}

void create_toolbar(Widget main_w, docker_context* ctx) {
	Widget toolbar, refreshButton, showRunningButton, showAllButton;
	toolbar = XtVaCreateManagedWidget("toolbar", xmRowColumnWidgetClass, main_w,
	XmNorientation, XmHORIZONTAL,
	NULL);

	XtManageChild(toolbar);
	refreshButton = XtVaCreateManagedWidget("Refresh", xmPushButtonWidgetClass,
			toolbar, NULL);
	XtManageChild(refreshButton);

	XtAddCallback(refreshButton, XmNactivateCallback, refresh_call, ctx);

	showRunningButton = XtVaCreateManagedWidget("Show Running",
			xmPushButtonWidgetClass, toolbar,
			NULL);

	showAllButton = XtVaCreateManagedWidget("Show All", xmPushButtonWidgetClass,
			toolbar,
			NULL);

	XtVaSetValues(main_w,
	XmNcommandWindow, toolbar,
	NULL);
}

void exit_if_no_threads() {
	if (XInitThreads() != True) {
		docker_log_error("X could not initialize threads, will exit now.");
		exit(VREX_X_HAS_NO_THREADS);
	} else {
		docker_log_debug("XInitThreads successful.");
	}
	Boolean has_threads = XtToolkitThreadInitialize();
	if (has_threads) {
		docker_log_debug("Threading is enabled.");
	} else {
		docker_log_error("Threading is disabled. Exit.");
		exit(VREX_X_HAS_NO_THREADS);
	}
}

int extract_args_url_connection(int argc, char* url, char* argv[],
		docker_context** ctx) {
	int connected = 0;
	if (argc > 1) {
		url = argv[1];
		if (is_http_url(url)) {
			if (make_docker_context_url(ctx, url) == E_SUCCESS) {
				connected = 1;
			}
		} else if (is_unix_socket(url)) {
			if (make_docker_context_socket(ctx, url) == E_SUCCESS) {
				connected = 1;
			}
		}
	} else {
		url = DOCKER_DEFINE_DEFAULT_UNIX_SOCKET;
		if (make_docker_context_socket(ctx, url) == E_SUCCESS) {
			connected = 1;
		}
	}
	if (connected) {
		docker_result* res;
		if (docker_ping((*ctx), &res) != E_SUCCESS) {
			docker_log_fatal("Could not ping the server %s", url);
			connected = 0;
		} else {
			docker_log_info("%s is alive.", url);
		}
	}
	return connected;
}

int main(int argc, char *argv[]) {
	Widget toplevel, main_w, matrix_w, main_form_w;
	XtAppContext app;
	docker_context* ctx;
	char* url;
	int row, column, n_rows, n_columns;
	int connected = 0;
	docker_log_set_level(LOG_INFO);

	exit_if_no_threads();

	toplevel = XtVaAppInitialize(&app, "V-Rex",
	NULL, 0, &argc, argv, fallback,
	NULL);

	connected = extract_args_url_connection(argc, url, argv, &ctx);
	if (!connected) {
		return E_PING_FAILED;
	}

	main_w = XtVaCreateManagedWidget("main_w", xmMainWindowWidgetClass,
			toplevel,
			NULL);

	main_form_w = XtVaCreateManagedWidget("main_form_w", xmFormWidgetClass,
			main_w, NULL);

	XtVaSetValues(main_w, XmNworkWindow, main_form_w,
	NULL);
	create_toolbar(main_w, ctx);
	create_menubar(main_w, ctx);

	XtRealizeWidget(toplevel);

	make_container_list_window(main_form_w, &matrix_w, ctx);

	XtAppMainLoop(app);

	/*NOTREACHED*/

	return 0;
}

