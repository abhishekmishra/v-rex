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
	int ret = pthread_mutex_lock(&interactions_w_lock);
	printf("thread lock returned %d\n", ret);
	fflush(0);
//	docker_error_handler_log(res);
	add_interactions_entry(vrex, res);
	pthread_mutex_unlock(&interactions_w_lock);
}

Widget interactions_w(struct vrex_context_t* vrex) {
//	return XtNameToWidget(XtNameToWidget(*(vrex->main_w), "ClipWindow"), "interactions_pane");
	return XtNameToWidget(XtNameToWidget(*(vrex->main_w), "interactions_frame"),
			"interactions_pane");
}

void quit_call() {
	docker_log_info("Quitting program\n");
	exit(0);
}

void help_call() {
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

void docker_pull_status_cb(docker_image_create_status* status, void* cbargs) {
	vrex_context* vrex = (vrex_context*) cbargs;
	add_messages_entry(vrex, status->status);
}

struct pull_args {
	vrex_context* vrex;
	char* image_name;
};

void* docker_pull_util(void* args) {
//	vrex_context* vrex = (vrex_context*) args;
	struct pull_args* pa = (struct pull_args*) args;
	docker_result* result;
	docker_image_create_from_image_cb(pa->vrex->d_ctx, &result,
			&docker_pull_status_cb, pa->vrex, pa->image_name, NULL, NULL);
	pa->vrex->handle_error(pa->vrex, result);
	free(pa->image_name);
	free(pa);
	pthread_exit(0);
	return NULL;
}

void docker_pull_cb(Widget widget, XtPointer client_data, XtPointer call_data) {
	vrex_context* vrex = (vrex_context*) client_data;
	pthread_t docker_pull_thread;
	struct pull_args* pa = (struct pull_args*)calloc(1, sizeof(struct pull_args));
	pa->vrex = vrex;
	String val;
	Widget imageWidget = XtNameToWidget(XtParent(widget), "Image Name");
	XtVaGetValues(imageWidget, XmNvalue, &val, NULL);
	pa->image_name = (char*) val;
	int thread_id = pthread_create(&docker_pull_thread, NULL, &docker_pull_util,
			pa);
}

void create_docker_server_toolbar(Widget docker_server_w, vrex_context* vrex) {
	Widget docker_server_toolbar, runningToggleButton, toolbarButton,
			request_time_text;
	docker_server_toolbar = XtVaCreateManagedWidget("toolbar_w",
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

	toolbarButton = XtVaCreateManagedWidget("Ping", xmPushButtonWidgetClass,
			docker_server_toolbar, NULL);
	XtManageChild(toolbarButton);
	XtAddCallback(toolbarButton, XmNactivateCallback, docker_ping_cb, vrex);

	toolbarButton = XtVaCreateManagedWidget("Server Prune",
			xmPushButtonWidgetClass, docker_server_toolbar, NULL);
//	XtVaSetValues(refreshButton, XmNlabelString, XmStringCreate("Sample Text \u0410\u0411\u0412\u0413\u0414\u0415\u0401 █ это - кошка: Prune", "UTF-8"), NULL);
	XtManageChild(toolbarButton);

	toolbarButton = XtVaCreateManagedWidget("Run Container",
			xmPushButtonWidgetClass, docker_server_toolbar, NULL);
	XtManageChild(toolbarButton);

	request_time_text = XtVaCreateManagedWidget("Image Name", xmTextWidgetClass,
			docker_server_toolbar,
			XmNeditable, True,
			XmNvalue, "alpine:latest",
			XmNcolumns, 40,
			NULL);

	toolbarButton = XtVaCreateManagedWidget("Pull Image",
			xmPushButtonWidgetClass, docker_server_toolbar, NULL);
	XtAddCallback(toolbarButton, XmNactivateCallback, docker_pull_cb, vrex);
	XtManageChild(toolbarButton);

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
	docker_log_set_level(LOG_DEBUG);

	exit_if_no_threads();

	pthread_mutexattr_t Attr;
	pthread_mutexattr_init(&Attr);
	pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
	if (pthread_mutex_init(&interactions_w_lock, &Attr) != 0) {
		printf("\n mutex init has failed\n");
		return 1;
	}

	curl_global_init(CURL_GLOBAL_ALL);

	String* fallback = get_light_theme();

	XtSetLanguageProc(NULL, NULL, NULL);
	toplevel = XtVaOpenApplication(&app, "V-Rex", NULL, 0, &argc, argv,
			fallback, sessionShellWidgetClass, NULL);

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

	set_statusbar_message(vrex, "woah! loaded.");
	set_statusbar_url(vrex, url);

	XtRealizeWidget(toplevel);
	XtAppMainLoop(app);

	// TODO: need to add to quit handler
	curl_global_cleanup();

	/*NOTREACHED*/

	return 0;
}

