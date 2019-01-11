#ifdef HAVE_CONFIG_H
#include <XbaeConfig.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
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
#include "docker_images.h"
#include "vrex_util.h"
#include "container_list_window.h"
#include "interactions_window.h"
#include "docker_server_window.h"
#include "events_window.h"
#include "theme.h"
#include "statusbar_window.h"
#include "messages_window.h"
#include "images_list_window.h"
#include "network_list_window.h"
#include "volumes_list_window.h"

#include <log.h>

#define USE_RENDER_TABLE 0

#define VREX_X_HAS_NO_THREADS -2
//#define VREX_USE_THREADS 0

static pthread_mutex_t interactions_w_lock;
static int show_calls = 0;

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
//	docker_error_handler_log(res);
	if (show_calls) {
		int ret = pthread_mutex_lock(&interactions_w_lock);
		fflush(0);
		add_interactions_entry(vrex, res);
		pthread_mutex_unlock(&interactions_w_lock);
	}
}

Widget interactions_w(struct vrex_context_t* vrex) {
//	return XtNameToWidget(XtNameToWidget(*(vrex->main_w), "ClipWindow"), "interactions_pane");
	return XtNameToWidget(XtNameToWidget(*(vrex->main_w), "interactions_frame"),
			"interactions_pane");
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
	dialog = XmCreateInformationDialog(*vrex->main_w,
			"V-Rex: docker version info", arg, n);
	XtManageChild(dialog);
	XtUnmanageChild(XtNameToWidget(dialog, "Cancel"));
	XtUnmanageChild(XtNameToWidget(dialog, "Help"));
	XmStringFree(xms);
}

void* docker_ping_util(void* args) {
	vrex_context* vrex = (vrex_context*) args;
	docker_result* result;
	docker_ping(vrex->d_ctx, &result);
	vrex->handle_error(vrex, result);
//	free_docker_result(result);
	pthread_exit(0);
	return NULL;
}

void docker_ping_cb(Widget widget, XtPointer client_data, XtPointer call_data) {
	vrex_context* vrex = (vrex_context*) client_data;
	pthread_t docker_ping_thread;
	int thread_id = pthread_create(&docker_ping_thread, NULL, &docker_ping_util,
			vrex);
}

void refresh_all_cb(Widget widget, XtPointer client_data, XtPointer call_data) {
	vrex_context* vrex = (vrex_context*) client_data;
	refresh_images_list(vrex);
	refresh_containers_list(vrex);
	refresh_networks_list(vrex);
	refresh_volumes_list(vrex);
}

void create_server_toolbar(vrex_context* vrex, Widget toolbar_w) {
	Widget toolbarButton;
	Widget server_toolbar_frame_w, label;
	int n = 0;
	Arg args[10];
	n = 0;
	XtSetArg(args[n], XmNshadowType, XmSHADOW_OUT);
	n++;
	XtSetArg(args[n], XmNmarginWidth, 1);
	n++;
	XtSetArg(args[n], XmNmarginHeight, 1);
	n++;
	server_toolbar_frame_w = XmCreateFrame(toolbar_w, "server_toolbar_frame_w",
			args, n);
	XtVaSetValues(server_toolbar_frame_w, XmNmarginWidth, 0, XmNmarginHeight, 0,
	XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 0,
	XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 0,
	XmNbottomAttachment, XmATTACH_POSITION, XmNbottomPosition, 2,
	XmNrightAttachment, XmATTACH_POSITION, XmNrightPosition, 2, NULL);
	Widget server_toolbar_w = XtVaCreateManagedWidget("server_toolbar_w",
			xmRowColumnWidgetClass, server_toolbar_frame_w, XmNorientation,
			XmHORIZONTAL, XmNmarginWidth, 0, XmNmarginHeight, 0,
			XmNtopAttachment, XmATTACH_POSITION, XmNtopPosition, 0,
			XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 0,
			XmNbottomAttachment, XmATTACH_POSITION, XmNbottomPosition, 2,
			XmNrightAttachment, XmATTACH_POSITION, XmNrightPosition, 2, NULL);
	n = 0;
	label = XmCreateLabelGadget(server_toolbar_w, "Server", args, n);

	toolbarButton = XtVaCreateManagedWidget("Ping", xmPushButtonWidgetClass,
			server_toolbar_w, NULL);
	XtManageChild(toolbarButton);
	XtAddCallback(toolbarButton, XmNactivateCallback, docker_ping_cb, vrex);

	toolbarButton = XtVaCreateManagedWidget("Prune", xmPushButtonWidgetClass,
			server_toolbar_w, XmNsensitive, False, NULL);
	XtManageChild(toolbarButton);

	XtManageChild(label);
	XtManageChild(server_toolbar_frame_w);
}

void show_calls_callback(Widget widget, XtPointer client_data,
		XtPointer call_data) {
	vrex_context* vrex = (vrex_context*) client_data;
	XmToggleButtonCallbackStruct *state =
			(XmToggleButtonCallbackStruct *) call_data;
//	docker_log_debug("%s: %s\n", XtName(widget),
//			state->set == XmSET ? "on" :
//			state->set == XmOFF ? "off" : "indeterminate");
	if (state->set == XmSET) {
		show_calls = 1;
		show_interactions_window(vrex);
	} else {
		show_calls = 0;
		hide_interactions_window(vrex);
	}
}

void create_vrex_toolbar(Widget docker_server_w, vrex_context* vrex) {
	Widget toolbar_w, runningToggleButton, toolbarButton;
	toolbar_w = XtVaCreateManagedWidget("toolbar_w", xmRowColumnWidgetClass,
			docker_server_w,
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

	create_server_toolbar(vrex, toolbar_w);
	create_container_toolbar(vrex, toolbar_w);
	create_images_toolbar(vrex, toolbar_w);

	toolbarButton = XtVaCreateManagedWidget("Refresh All",
			xmPushButtonWidgetClass, toolbar_w, NULL);
	XtAddCallback(toolbarButton, XmNactivateCallback, refresh_all_cb, vrex);
	XtManageChild(toolbarButton);

	runningToggleButton = XtVaCreateManagedWidget("Calls",
			xmToggleButtonWidgetClass, toolbar_w,
			XmNset, XmSET,
			NULL);
	XtAddCallback(runningToggleButton, XmNvalueChangedCallback,
			show_calls_callback, vrex);
	if (show_calls == 0) {
		hide_interactions_window(vrex);
		XmToggleButtonSetState(runningToggleButton, False, False);
	} else {
		show_interactions_window(vrex);
		XmToggleButtonSetState(runningToggleButton, True, False);
	}

	XtManageChild(toolbar_w);
}

/* Any item the user selects from the File menu calls this function.
 ** It will either be "Open" (item_no == 0) or "Quit" (item_no == 1).
 */
void vrex_cb(Widget widget, XtPointer client_data, XtPointer call_data) {
	static Widget dialog;
	/* make it static for reuse */
	void
	load_pixmap(Widget, XtPointer, XtPointer);
	long item_no = (long) client_data;
	if (item_no == 1) /* the "quit" item */
		exit(0);
	XtManageChild(dialog);
}

#define ABOUT_MESSAGE \
	"V-Rex verson 0.1a\n\n"\
	"V-Rex is a fast and simple monitoring UI for docker. \n\n"\
	"License: GNU GPLv3\n"\
	"Author: Abhishek Mishra <abhishekmishra3@gmail.com>\n"\
	"Project Home: https://github.com/abhishekmishra/v-rex\n"

/* The help button in the help menu from the menubar was selected.
 ** Display help information defined above for how to use the program.
 ** This is done by creating a Motif information dialog box. Again,
 ** make the dialog static so we can reuse it.
 */
void help_cb(Widget widget, XtPointer client_data, XtPointer call_data) {
	vrex_context* vrex = (vrex_context*) client_data;
	static Widget dialog;
	if (!dialog) {
		Arg args[5];
		int n = 0;
		XmString msg = XmStringCreateLocalized(ABOUT_MESSAGE);
		XtSetArg(args[n], XmNmessageString, msg);
		n++;
		XtSetArg(args[n], XmNdialogType, XmDIALOG_INFORMATION);
		n++;
		dialog = XmCreateInformationDialog(XtParent(*vrex->main_w), "About",
				args, n);
		XtUnmanageChild(XtNameToWidget(dialog, "Cancel"));
		XtUnmanageChild(XtNameToWidget(dialog, "Help"));
	}
	XtManageChild(dialog);
}

void create_menubar(Widget main_w, vrex_context* vrex) {
	Widget widget, menu_bar, docker_version, header_container, vrex_menu,
			help_menu;
	XmString vrex_str, help_str, pref_str, quit_str, version_str, about_str;
	Arg arg[1];

	header_container = XmVaCreateManagedForm(main_w, "main_header_w",
	XmNfractionBase, 2,
	NULL);

	vrex_str = XmStringCreateLocalized("V-Rex");
	help_str = XmStringCreateLocalized("Help");
	version_str = XmStringCreateLocalized("Docker Version");
	about_str = XmStringCreateLocalized("About V-Rex");
	pref_str = XmStringCreateLocalized("Preferences...");
	menu_bar = XmVaCreateSimpleMenuBar(header_container, "menu_bar",
	XmVaCASCADEBUTTON, vrex_str, 'V',
	XmVaCASCADEBUTTON, help_str, 'H',
	XmNtopAttachment, XmATTACH_POSITION,
	XmNtopPosition, 0,
	XmNleftAttachment, XmATTACH_POSITION,
	XmNleftPosition, 0,
	XmNbottomAttachment, XmATTACH_POSITION,
	XmNbottomPosition, 1,
	XmNrightAttachment, XmATTACH_POSITION,
	XmNrightPosition, 2,
	NULL);

	if ((widget = XtNameToWidget(menu_bar, "button_1")) != (Widget) 0)
		XtVaSetValues(menu_bar, XmNmenuHelpWidget, widget, NULL);

	quit_str = XmStringCreateLocalized("Quit");
	vrex_menu = XmVaCreateSimplePulldownMenu(menu_bar, "vrex_menu", 0, vrex_cb,
	XmVaSEPARATOR,
	XmVaPUSHBUTTON, pref_str, 'R', NULL, NULL,
	XmVaPUSHBUTTON, quit_str, 'Q', NULL, NULL,
	NULL);
	XtVaSetValues(XtNameToWidget(vrex_menu, "button_0"), XmNsensitive, False,
	NULL);
	XmStringFree(pref_str);
	XmStringFree(quit_str);

	help_menu = XmVaCreateSimplePulldownMenu(menu_bar, "help_menu", 1, NULL,
	XmVaPUSHBUTTON, help_str, 'H', NULL, NULL,
	XmVaPUSHBUTTON, version_str, 'V', NULL, NULL,
	XmVaPUSHBUTTON, about_str, 'A', NULL, NULL, NULL);
	XmStringFree(help_str);
	XmStringFree(version_str);
	XmStringFree(about_str);

	XtAddCallback(XtNameToWidget(help_menu, "button_0"), XmNactivateCallback,
			help_cb, vrex);
	XtAddCallback(XtNameToWidget(help_menu, "button_1"), XmNactivateCallback,
			docker_version_show, vrex);
	XtAddCallback(XtNameToWidget(help_menu, "button_2"), XmNactivateCallback,
			help_cb, vrex);

//	docker_version = XtVaCreateManagedWidget("Docker Version",
//			xmCascadeButtonWidgetClass, menu_bar,
//			XmNmnemonic, 'V',
//			NULL);
//
//	XtAddCallback(docker_version, XmNactivateCallback, docker_version_show,
//			vrex);

	XtVaSetValues(main_w,
	XmNmenuBar, header_container, NULL);

	XtManageChild(menu_bar);
	create_vrex_toolbar(header_container, vrex);
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

int extract_args_url_connection(int argc, char** ret_url, char* argv[],
		docker_context** ctx, docker_result** res) {
	char* url;
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
	(*ret_url) = url;
	return connected;
}

int main(int argc, char *argv[]) {
	Widget toplevel, main_w, matrix_w, main_form_w, docker_server_w, events_w;
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
	docker_log_set_level(LOG_FATAL);

	exit_if_no_threads();

	pthread_mutexattr_t Attr;
	pthread_mutexattr_init(&Attr);
	pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
	if (pthread_mutex_init(&interactions_w_lock, &Attr) != 0) {
		printf("\n mutex init has failed\n");
		return 1;
	}

	curl_global_init(CURL_GLOBAL_ALL);

//	String* fallback = get_null_theme();
	String* fallback = get_dark_theme();
//	String* fallback = get_light_theme();

	XtSetLanguageProc(NULL, NULL, NULL);
	toplevel = XtVaOpenApplication(&app, "V-Rex", NULL, 0, &argc, argv,
			fallback, sessionShellWidgetClass,
			XmNtitle, "V-Rex verson 0.1a", NULL);

	connected = extract_args_url_connection(argc, &url, argv, &ctx, &res);
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
	create_statusbar(vrex, main_w);

	make_docker_server_window(vrex, &docker_server_w);
	make_docker_events_window(vrex, docker_server_w, &events_w);
	make_docker_messages_window(vrex, docker_server_w);
	make_container_list_window(docker_server_w, &matrix_w, vrex);
	make_docker_networks_list_window(vrex, docker_server_w);
	make_docker_images_list_window(vrex, docker_server_w);
	make_docker_volumes_list_window(vrex, docker_server_w);

	set_statusbar_message(vrex, "V-Rex Loaded.");
	set_statusbar_url(vrex, url);

	XtRealizeWidget(toplevel);
	XtAppMainLoop(app);

	// TODO: need to add to quit handler
	curl_global_cleanup();

	/*NOTREACHED*/

	return 0;
}

