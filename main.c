#ifdef HAVE_CONFIG_H
#include <XbaeConfig.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <curl/curl.h>
#ifdef USE_EDITRES
#include <X11/Intrinsic.h>
#include <X11/Xmu/Editres.h>
#endif

#include <Xm/MainW.h>
#include <Xm/Label.h>
#include <Xbae/Matrix.h>
#include <Xm/CascadeB.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>

#include <docker_containers.h>
#include <log.h>

#define USE_RENDER_TABLE 0

#define VREX_X_HAS_NO_THREADS -2

void handle_error(docker_result* res) {
	docker_simple_error_handler_log(res);
}

void add_column(Widget mw, char* name, int num, int width) {
	XbaeMatrixSetColumnLabel(mw, num, name);
	XbaeMatrixSetColumnWidth(mw, num, width);
}

void list_containers(Widget* matrix_w) {
	Widget mw = *matrix_w;
	char* id;
	docker_context* ctx;
	docker_result* res;
	docker_containers_list_filter* filter;
	docker_containers_list* containers;

	XtAppLock(matrix_w);
	curl_global_init(CURL_GLOBAL_ALL);

//	if (make_docker_context_socket(&ctx, "/var/run/docker.sock") == E_SUCCESS) {
	if (make_docker_context_url(&ctx, "http://192.168.1.33:2376/")
			== E_SUCCESS) {
		printf("Docker containers list.\n\n");
		docker_containers_list_filter* filter;
		make_docker_containers_list_filter(&filter);
//		containers_filter_add_name(filter, "/registryui");
//		containers_filter_add_id(filter, id);
		docker_containers_list* containers;
		docker_container_list(ctx, &res, &containers, 0, -1, 1, filter);
		handle_error(res);
		docker_log_info("Read %d containers.\n",
				docker_containers_list_length(containers));
		int col_num = 0;
		add_column(mw, "Name", col_num, 20);
		col_num++;
//		add_column(mw, "Id", col_num, 30);
//		col_num++;
		add_column(mw, "Image", col_num, 20);
		col_num++;
		add_column(mw, "Command", col_num, 20);
		col_num++;
		add_column(mw, "FS Size", col_num, 10);
		col_num++;
		add_column(mw, "State", col_num, 30);
//		col_num++;
//		add_column(mw, "Status", col_num, 20);
		for (int i = 0; i < docker_containers_list_length(containers); i++) {
			col_num = 0;
			char** rows;
			rows = (char**) XtCalloc(10, sizeof(String));

			docker_container_list_item* item = docker_containers_list_get_idx(
					containers, i);
			rows[col_num++] = docker_container_list_item_names_get_idx(item, 0);
//			rows[1] = docker_container_list_item_get_id(item);
			rows[col_num++] = docker_container_list_item_get_image(item);
			rows[col_num++] = docker_container_list_item_get_command(item);
//			char* root_fs_size = (char*) XtCalloc(10, sizeof(char));
			if (docker_container_list_item_ports_length(item) > 0) {
				docker_container_ports* first_port =
						docker_container_list_item_ports_get_idx(item, 0);
				if (first_port
						&& docker_container_ports_get_public_port(first_port)
								> 0
						&& docker_container_ports_get_private_port(first_port)
								> 0) {
					char* ports_str = (char*) XtCalloc(128, sizeof(char));
					sprintf(ports_str, "%ld:%ld",
							docker_container_ports_get_public_port(first_port),
							docker_container_ports_get_private_port(
									first_port));
					rows[col_num++] = ports_str;
				}
			}
//			sprintf(root_fs_size, "%lld",
//					docker_container_list_item_get_size_root_fs(item));
//			rows[col_num++] = root_fs_size;
			char* status = (char*) XtCalloc(1024, sizeof(char));
			strcpy(status, docker_container_list_item_get_state(item));
			strcat(status, ":");
			strcat(status, docker_container_list_item_get_status(item));
			rows[col_num++] = status;

			XbaeMatrixAddRows(mw, XbaeMatrixNumRows(mw), rows, NULL, NULL, 1);
			free(rows);
		}
	}
	curl_global_cleanup();
	Widget top = XtParent(mw);
	docker_log_info("Found parent %s.", XtName(top));
	Widget toolbar = XtNameToWidget(top, "toolbar");
	docker_log_info("Found toolbar %s.", XtName(toolbar));
	Widget refresh = XtNameToWidget(toolbar, "Refresh");
	docker_log_info("Found refresh %s.", XtName(refresh));
	XtSetSensitive(refresh, True);
	XmUpdateDisplay(top);
	XtAppUnlock(matrix_w);
	int ret = 0;
	pthread_exit(&ret);
}

void load_containers_list(Widget matrix_w) {
	int num_rows = XbaeMatrixNumRows(matrix_w);
	if (num_rows > 0) {
		XbaeMatrixDeleteRows(matrix_w, 0, num_rows);
	}

	pthread_t thread_id;
	pthread_create(&thread_id, NULL, list_containers, &matrix_w);
}

static String fallback[] = { "V-Rex*main_w.width:		1024",
		"V-Rex*.background:		#111111", "V-Rex*.foreground:		#D3D3D3",
		"V-Rex*main_w.height:		768", "V-Rex*mw.shadowType:		SHADOW_ETCHED_OUT",
		"V-Rex*mw.shadowThickness:		1", "V-Rex*mw.cellShadowThickness:	1",
		"V-Rex*mw.gridType:		GRID_CELL_LINE",
		"V-Rex*mw.cellShadowType:		shadow_in", "V-Rex*mw.visibleColumns:		5",
		"V-Rex*mw.visibleRows:		0", "V-Rex*mw.rows:			0",
		"V-Rex*mw.columns:			5", "V-Rex*mw.fixedRows:		0",
		"V-Rex*mw.fixedColumns:		1", "V-Rex*mw.trailingFixedRows:	0",
		"V-Rex*mw.trailingFixedColumns:	0", "V-Rex*mw.traverseFixedCells:	True",
		"V-Rex*mw.multiLineCell:	True",
		"V-Rex*mw.wrapType: 	wrap_continuous", //other option is wrap_word
		"V-Rex*mw.fill: 	False", "V-Rex*mw.horzFill: 	False",
		"V-Rex*mw.vertFill: 	False", "V-Rex*mw.height: 	800",
		"V-Rex*mw.width: 	1024", "V-Rex*mw.gridLineColor: 	#A0A0A0",
		"V-Rex*mw.background: 	#111111", "V-Rex*mw.foreground: 	#D3D3D3",
		"V-Rex*mw.columnLabelColor: 	#a0a0ff",
		"V-Rex*mw.highlightColor: 	#6495ED", "V-Rex*mw.rowHeight: 	200",

//				"V-Rex*mw.columnWidths:		10, 5, 10, 5, 10, 5,"
//						"					10, 5, 10, 5, 10, 5",
//				"V-Rex*mw.columnLabels:		Zero, One, Two, Three, Four,"
//						"					Five, Six, Seven, Eight, Nine",
//				"V-Rex*mw.rowLabels:		0, 1, 2, 3, 4, 5, 6, 7, 8, 9",
		"V-Rex*mw.fontList: -*-terminus-medium-r-*-*-10-*-*-*-*-*-*-*,"
				"-*-terminus-bold-r-*-*-10-*-*-*-*-*-*-*=bold,"
				"-*-terminus-medium-r-*-*-10-*-*-*-*-*-*-*=italic",
		"V-Rex*mw.labelFont: -*-terminus-bold-r-*-*-14-*-*-*-*-*-*-*",
		"V-Rex*mw.cellShadowThickness:		0", "V-Rex*mw.textShadowThickness:		0",
		"V-Rex*mw.cellHighlightThickness:		2", "V-Rex*mw.cellMarginHeight:		0",
		"V-Rex*mw.cellMarginWidth:		1",
		NULL };

void labelCB(Widget mw, XtPointer cd, XtPointer cb) {
	XbaeMatrixLabelActivateCallbackStruct *cbs =
			(XbaeMatrixLabelActivateCallbackStruct *) cb;

	if (cbs->row_label)
		if (XbaeMatrixIsRowSelected(mw, cbs->row))
			XbaeMatrixDeselectRow(mw, cbs->row);
		else
			XbaeMatrixSelectRow(mw, cbs->row);
	else if (XbaeMatrixIsColumnSelected(mw, cbs->column))
		XbaeMatrixDeselectColumn(mw, cbs->column);
	else
		XbaeMatrixSelectColumn(mw, cbs->column);
}

void cellCB(Widget mw, XtPointer cd, XtPointer cb) {
	XbaeMatrixEnterCellCallbackStruct *cbs =
			(XbaeMatrixEnterCellCallbackStruct*) cb;
	cbs->map = True;
	cbs->doit = False;
	cbs->select_text = True;
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

void refresh_call(Widget widget, XtPointer client_data, XtPointer call_data) {
	XtSetSensitive(widget, False);
	Widget top = XtParent(XtParent(widget));
	load_containers_list(XtNameToWidget(top, "mw"));
	docker_log_debug("Refresh button name - %s", XtName(widget));
}

void create_menubar(Widget main_w) {
	Widget menu_bar, quit, help;
	Arg arg[1];
	menu_bar = XmCreateMenuBar(main_w, "main_list", NULL, 0);
	XtManageChild(menu_bar);

	/* create quit widget + callback */

	quit = XtVaCreateManagedWidget("Quit", xmCascadeButtonWidgetClass, menu_bar,
	XmNmnemonic, 'Q',
	NULL);

	XtAddCallback(quit, XmNactivateCallback, quit_call, NULL);

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

void create_toolbar(Widget main_w) {
	Widget toolbar, refreshButton, showRunningButton, showAllButton;
	toolbar = XtVaCreateManagedWidget("toolbar", xmRowColumnWidgetClass, main_w,
	XmNorientation, XmHORIZONTAL,
	NULL);

	XtManageChild(toolbar);
	refreshButton = XtVaCreateManagedWidget("Refresh", xmPushButtonWidgetClass,
			toolbar,
			NULL);
	XtManageChild(refreshButton);

	XtAddCallback(refreshButton, XmNactivateCallback, refresh_call, NULL);

	showRunningButton = XtVaCreateManagedWidget("Show Running",
			xmPushButtonWidgetClass, toolbar,
			NULL);

	showAllButton = XtVaCreateManagedWidget("Show All", xmPushButtonWidgetClass,
			toolbar,
			NULL);

	XtVaSetValues(main_w,
	XmNcommandWindow, toolbar,
	NULL);

	Widget refresh = XtNameToWidget(main_w, "toolbar");
	docker_log_info("Found toolbar %s.", XtName(refresh));
}

int main(int argc, char *argv[]) {
	Widget toplevel, main_w, matrix_w;
	XtAppContext app;
	int row, column, n_rows, n_columns;
	docker_log_set_level(LOG_DEBUG);

	if (XInitThreads() != True) {
		docker_log_error("X could not initialize threads, will exit now.");
		exit(VREX_X_HAS_NO_THREADS);
	} else {
		docker_log_info("XInitThreads successful.");
	}
	Boolean has_threads = XtToolkitThreadInitialize();
	if (has_threads) {
		docker_log_info("Threading is enabled.");
	} else {
		docker_log_debug("Threading is disabled. Exit.");
		exit(VREX_X_HAS_NO_THREADS);
	}

	toplevel = XtVaAppInitialize(&app, "V-Rex",
	NULL, 0, &argc, argv, fallback,
	NULL);

	XFontStruct *plain_font = XLoadQueryFont(XtDisplay(toplevel),
			"-*-terminus-medium-r-*-*-14-*-*-*-*-*-*-*");
	XmFontListEntry font_list_entry = XmFontListEntryCreate(
	XmFONTLIST_DEFAULT_TAG, XmFONT_IS_FONT, plain_font);

	XmFontList plain_font_list = XmFontListAppendEntry(
	NULL, font_list_entry);

	docker_log_info("plain font: %d plain font list %d", plain_font == NULL,
			plain_font_list == NULL);

	main_w = XtVaCreateManagedWidget("main_w", xmMainWindowWidgetClass,
			toplevel,
			NULL);

	matrix_w = XtVaCreateManagedWidget("mw", xbaeMatrixWidgetClass, main_w,
	XmNlabelFont, plain_font_list,
	XmNfontList, plain_font_list,
	NULL);

	XtVaSetValues(main_w, XmNworkWindow, matrix_w,
	NULL);

	XmFontListEntryFree(&font_list_entry);
	XmFontListFree(plain_font_list);

	XtAddCallback(matrix_w, XmNlabelActivateCallback, labelCB, NULL);
	XtAddCallback(matrix_w, XmNenterCellCallback, cellCB, NULL);

	create_toolbar(main_w);
	create_menubar(main_w);

//	load_containers_list(matrix_w);
	XtRealizeWidget(toplevel);

	pthread_t thread_id;
	pthread_create(&thread_id, NULL, list_containers, &matrix_w);

	XtAppMainLoop(app);

	/*NOTREACHED*/

	return 0;
}

