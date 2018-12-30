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
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/MessageB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/LabelG.h>
#include <Xm/RowColumn.h>
#include <Xm/Frame.h>
#include <X11/Xos.h>

#include "docker_connection_util.h"
#include "docker_system.h"
#include "vrex_util.h"
#include "container_list_window.h"
#include "interactions_window.h"
#include "docker_server_window.h"

#include <log.h>

#define USE_RENDER_TABLE 0

#define VREX_X_HAS_NO_THREADS -2

//#define VREX_USE_THREADS 0

static String fallback[] =
		{  "V-Rex*main_w.width:		1024", "V-Rex*main_w.height:		768",
				"V-Rex*main_w.borderWidth:		0",
				"V-Rex*main_w.shadowThickness:		0",
				"V-Rex*.background:		#DCDCDC", "V-Rex*.foreground:		#000000",
				"V-Rex*.borderColor:		#A9A9A9",
				"V-Rex*.highlightColor:		#008080",
				"V-Rex*.bottomShadowColor:		#696969",
				"V-Rex*.topShadowColor:		#696969", "V-Rex*.shadowThickness: 1",
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

//TODO: see https://gist.github.com/unix-junkie/68bdf8420d6c7b7925f4
// and https://stackoverflow.com/questions/34360066/xmstringgenerate-in-xmmultibyte-text-or-xmwidechar-text-mode
// for UTF-8
//UTF-8 font	"V-Rex*fontList: terminus-12",
				"V-Rex*fontList: -*-terminus-medium-r-*-*-12-*-*-*-*-*-*-*",
				"V-Rex*labelFont: -*-lucida-bold-r-*-*-12-*-*-*-*-*-*-*",
				"V-Rex*docker_server_summary_text.fontList: -*-terminus-bold-r-*-*-16-*-*-*-*-*-*-*",
//		"V-Rex*mw.cellShadowThickness:		0", "V-Rex*mw.textShadowThickness:		0",
//		"V-Rex*mw.cellHighlightThickness:		2", "V-Rex*mw.cellMarginHeight:		0",
//		"V-Rex*mw.cellMarginWidth:		1",
				NULL };

void docker_error_handler_log(docker_result* res) {
	docker_log_debug("DOCKER_RESULT: For URL: %s", get_docker_result_url(res));
	docker_log_debug(
			"DOCKER RESULT: Response error_code = %d, http_response = %ld",
			get_docker_result_error(res), get_docker_result_http_error(res));
	if (!is_ok(res)) {
		docker_log_error("DOCKER RESULT: %s", get_docker_result_message(res));
	}
}

void handle_error(vrex_context* vrex, docker_result* res) {
	docker_error_handler_log(res);
	add_interactions_entry(vrex, res);
}

Widget interactions_w(struct vrex_context_t* vrex) {
//	return XtNameToWidget(XtNameToWidget(*(vrex->main_w), "ClipWindow"), "interactions_pane");
	return XtNameToWidget(XtNameToWidget(*(vrex->main_w), "interactions_frame"),
			"interactions_pane");
}

void quit_call()

{
	docker_log_info("Quitting program\n");
	exit(0);
}

void help_call()

{
	docker_log_info("Sorry, I'm Not Much Help\n");
}

void docker_version_show(Widget widget, XtPointer client_data,
		XtPointer call_data) {
	vrex_context* vrex = (vrex_context*) client_data;
	docker_result* res;
	docker_version* version;
	Widget dialog;
	Arg arg[5];
	XmString xms;
	int n = 0;

	if (docker_system_version(vrex->d_ctx, &res, &version) == E_SUCCESS) {
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
	vrex->handle_error(vrex, res);
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

void create_docker_server_toolbar(Widget docker_server_w, vrex_context* vrex) {
	Widget docker_server_toolbar, runningToggleButton, refreshButton;
	docker_server_toolbar = XtVaCreateManagedWidget("docker_server_toolbar",
			xmRowColumnWidgetClass, docker_server_w,
			XmNorientation, XmHORIZONTAL,
			XmNtopAttachment, XmATTACH_POSITION,
			XmNtopPosition, 1,
			XmNleftAttachment, XmATTACH_POSITION,
			XmNleftPosition, 0,
			XmNbottomAttachment, XmATTACH_POSITION,
			XmNbottomPosition, 2,
			XmNrightAttachment, XmATTACH_POSITION,
			XmNrightPosition, 2,
			NULL);

	refreshButton = XtVaCreateManagedWidget("Server Prune", xmPushButtonWidgetClass,
			docker_server_toolbar, NULL);
//	XtVaSetValues(refreshButton, XmNlabelString, XmStringCreate("Sample Text \u0410\u0411\u0412\u0413\u0414\u0415\u0401 █ это - кошка: Prune", "UTF-8"), NULL);
	XtManageChild(refreshButton);

	refreshButton = XtVaCreateManagedWidget("Pull Image", xmPushButtonWidgetClass,
			docker_server_toolbar, NULL);
	XtManageChild(refreshButton);

	refreshButton = XtVaCreateManagedWidget("Run Container", xmPushButtonWidgetClass,
			docker_server_toolbar, NULL);
	XtManageChild(refreshButton);

	runningToggleButton = XtVaCreateManagedWidget("Show Running",
			xmToggleButtonWidgetClass, docker_server_toolbar,
			XmNset, XmSET,
			XmNselectColor, 0x00FF00,
			XmNunselectColor, 0xFFFFFF,
			NULL);
//	XtAddCallback(runningToggleButton, XmNvalueChangedCallback,
//			show_running_callback, vrex->d_ctx);

	refreshButton = XtVaCreateManagedWidget("Refresh", xmPushButtonWidgetClass,
			docker_server_toolbar, NULL);
	XtManageChild(refreshButton);

//	XtAddCallback(refreshButton, XmNactivateCallback, refresh_call, vrex);

	XtManageChild(docker_server_toolbar);
}

void create_menubar(Widget main_w, vrex_context* vrex) {
	Widget menu_bar, quit, docker_version, help, header_container;
	Arg arg[1];

	header_container = XmVaCreateManagedForm(main_w, "main_header_w",
	XmNfractionBase, 2,
	NULL);

	menu_bar = XmCreateMenuBar(header_container, "main_list",
	NULL, 0);
	XtManageChild(menu_bar);
	XtVaSetValues(menu_bar,
	XmNtopAttachment, XmATTACH_POSITION,
	XmNtopPosition, 0,
	XmNleftAttachment, XmATTACH_POSITION,
	XmNleftPosition, 0,
	XmNbottomAttachment, XmATTACH_POSITION,
	XmNbottomPosition, 1,
	XmNrightAttachment, XmATTACH_POSITION,
	XmNrightPosition, 2,
	NULL);

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
			vrex);

	XtVaSetValues(main_w,
	XmNmenuBar, header_container, NULL);

	create_docker_server_toolbar(header_container, vrex);
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
		docker_context** ctx, docker_result** res) {
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
		if (docker_ping((*ctx), res) != E_SUCCESS) {
			docker_log_fatal("Could not ping the server %s", url);
			connected = 0;
		} else {
			docker_log_info("%s is alive.", url);
		}
	}
	return connected;
}

int main(int argc, char *argv[]) {
	Widget toplevel, main_w, matrix_w, main_form_w, docker_server_w;
	XtAppContext app;
	docker_context* ctx;
	vrex_context* vrex;
	docker_result* res;
	docker_info* info;
	Widget text_w, search_w, text_output;
	Widget rowcol_v, rowcol_h, label_w;
	int i, n;
	void search_text(Widget, XtPointer, XtPointer);
	Arg args[10];

	char* url;
	int row, column, n_rows, n_columns;
	int connected = 0;
	docker_log_set_level(LOG_INFO);

	exit_if_no_threads();
//
//	toplevel = XtVaAppInitialize(&app, "V-Rex",
//	NULL, 0, &argc, argv, fallback,
//	NULL);

	XtSetLanguageProc(NULL, NULL, NULL);
	toplevel = XtVaOpenApplication(&app, "V-Rex", NULL, 0, &argc, argv,
			fallback, sessionShellWidgetClass, NULL);

	connected = extract_args_url_connection(argc, url, argv, &ctx, &res);
	if (!connected) {
		return E_PING_FAILED;
	}

	main_w = XtVaCreateManagedWidget("main_w", xmMainWindowWidgetClass,
			toplevel,
			XmNcommandWindowLocation, XmCOMMAND_BELOW_WORKSPACE,
//INFO: if we uncomment this change interactions_w function accordingly
// as this creates a new child window called ClipWindow
//			XmNscrollingPolicy, XmAUTOMATIC,
			XmNborderWidth, 0,
			NULL);

	docker_log_debug(XtName(XtParent(main_w)));

//	main_form_w = XtVaCreateManagedWidget("main_form_w", xmFormWidgetClass,
//			main_w,
//			NULL);
//	XtVaSetValues(main_w, XmNworkWindow, main_form_w,
//	NULL);

	vrex = (vrex_context*) malloc(sizeof(vrex_context));
	vrex->main_w = &main_w;
	vrex->d_ctx = ctx;
	vrex->handle_error = &handle_error;
	init_results_list(vrex);
	make_interactions_window(vrex);
	vrex->interactions_w = &interactions_w;

	//add the ping call to interactions history
	handle_error(vrex, res);

	create_menubar(main_w, vrex);

//	make_container_list_window(main_form_w, &matrix_w, vrex);
	make_docker_server_window(vrex, &docker_server_w);

	XtRealizeWidget(toplevel);
	XtAppMainLoop(app);

	/*NOTREACHED*/

	return 0;
}

