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
#include <Xm/Label.h>
#include <Xbae/Matrix.h>

#include <docker_containers.h>
#include <log.h>

#define USE_RENDER_TABLE 0

#define VREX_X_HAS_NO_THREADS -2

void handle_error(docker_result* res) {
	docker_simple_error_handler_log(res);
}

void* list_containers(Widget* win) {
	Widget mw = *win;
	char* id;
	docker_context* ctx;
	docker_result* res;
	docker_containers_list_filter* filter;
	docker_containers_list* containers;

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
		docker_container_list(ctx, &res, &containers, 1, 25, 1, filter);
		handle_error(res);
		docker_log_info("Read %d containers.\n",
				docker_containers_list_length(containers));
		XbaeMatrixSetColumnLabel(mw, 0, "Id");
		XbaeMatrixSetColumnWidth(mw, 0, 30);
		XbaeMatrixSetColumnLabel(mw, 1, "Name");
		XbaeMatrixSetColumnWidth(mw, 1, 30);
		XbaeMatrixSetColumnLabel(mw, 2, "Image");
		XbaeMatrixSetColumnWidth(mw, 2, 20);
		XbaeMatrixSetColumnLabel(mw, 3, "Command");
		XbaeMatrixSetColumnWidth(mw, 3, 30);
		XbaeMatrixSetColumnLabel(mw, 4, "Public Port");
		XbaeMatrixSetColumnWidth(mw, 4, 20);
		XbaeMatrixSetColumnLabel(mw, 5, "Private Port");
		XbaeMatrixSetColumnWidth(mw, 5, 20);
		XbaeMatrixSetColumnLabel(mw, 6, "FS Size");
		XbaeMatrixSetColumnWidth(mw, 6, 10);
		XbaeMatrixSetColumnLabel(mw, 7, "State");
		XbaeMatrixSetColumnWidth(mw, 7, 10);
		XbaeMatrixSetColumnLabel(mw, 8, "Status");
		XbaeMatrixSetColumnWidth(mw, 8, 20);
		for (int i = 0; i < docker_containers_list_length(containers); i++) {
			char** rows;
			rows = (char**) XtCalloc(10, sizeof(String));

			docker_container_list_item* item = docker_containers_list_get_idx(
					containers, i);
			rows[0] = docker_container_list_item_get_id(item);
			rows[1] = docker_container_list_item_names_get_idx(item, 0);
			rows[2] = docker_container_list_item_get_image(item);
			rows[3] = docker_container_list_item_get_command(item);
			char* port = (char*) XtCalloc(10, sizeof(char));
			if (docker_container_list_item_ports_length(item) > 0) {
				docker_container_ports* first_port =
						docker_container_list_item_ports_get_idx(item, 0);
				sprintf(port, "%ld",
						docker_container_ports_get_public_port(first_port));
				rows[4] = port;
				sprintf(port, "%ld",
						docker_container_ports_get_private_port(first_port));
				rows[5] = port;
			}
			sprintf(port, "%lld",
					docker_container_list_item_get_size_root_fs(item));
			rows[6] = port;
			rows[7] = docker_container_list_item_get_state(item);
			rows[8] = docker_container_list_item_get_status(item);

			XbaeMatrixAddRows(mw, XbaeMatrixNumRows(mw), rows, NULL, NULL, 1);
			free(rows);
		}
	}
	curl_global_cleanup();
//	return containers;
}

static String fallback[] =
		{ "Multifixed*mw.shadowType:		SHADOW_ETCHED_OUT",
				"Multifixed*mw.shadowThickness:		1",
				"Multifixed*mw.cellShadowThickness:	1",
				"Multifixed*mw.gridType:		GRID_CELL_LINE",
				"Multifixed*mw.cellShadowType:		shadow_in",
				"Multifixed*mw.visibleColumns:		9",
				"Multifixed*mw.visibleRows:		0", "Multifixed*mw.rows:			0",
				"Multifixed*mw.columns:			9", "Multifixed*mw.fixedRows:		0",
				"Multifixed*mw.fixedColumns:		1",
				"Multifixed*mw.trailingFixedRows:	0",
				"Multifixed*mw.trailingFixedColumns:	0",
				"Multifixed*mw.traverseFixedCells:	True",
				"Multifixed*mw.multiLineCell:	True",
				"Multifixed*mw.wrapType: 	wrap_continuous", //other option is wrap_word
				"Multifixed*mw.fill: 	False", "Multifixed*mw.horzFill: 	False",
				"Multifixed*mw.vertFill: 	False", "Multifixed*mw.height: 	800",
				"Multifixed*mw.width: 	1024",
				"Multifixed*mw.gridLineColor: 	#A0A0A0",
				"Multifixed*mw.background: 	#D3D3D3",
				"Multifixed*mw.foreground: 	#111111",
				"Multifixed*mw.columnLabelColor: 	#0000CD",
				"Multifixed*mw.highlightColor: 	#6495ED",
				"Multifixed*mw.rowHeight: 	200",
//				"Multifixed*mw.columnWidths:		10, 5, 10, 5, 10, 5,"
//						"					10, 5, 10, 5, 10, 5",
//				"Multifixed*mw.columnLabels:		Zero, One, Two, Three, Four,"
//						"					Five, Six, Seven, Eight, Nine",
//				"Multifixed*mw.rowLabels:		0, 1, 2, 3, 4, 5, 6, 7, 8, 9",
#if USE_RENDER_TABLE
				"Multifixed*mw.renderTable: labels, italic, bold",
				"Multifixed*mw.renderTable.fontType:        FONT_IS_FONT",
				"Multifixed*mw.renderTable.fontName:        -*-terminus-medium-r-*-*-12-*-*-*-*-*-*-*",
				"Multifixed*mw.renderTable.labels.fontType: FONT_IS_FONT",
				"Multifixed*mw.renderTable.labels.fontName: -*-terminus-bold-r-*-*-14-*-*-*-*-*-*-*",
				"Multifixed*mw.renderTable.italic.fontType: FONT_IS_FONT",
				"Multifixed*mw.renderTable.italic.fontName: -*-terminus-medium-r-*-*-12-*-*-*-*-*-*-*",
				"Multifixed*mw.renderTable.bold.fontType:   FONT_IS_FONT",
				"Multifixed*mw.renderTable.bold.fontName:   -*-terminus-bold-r-*-*-12-*-*-*-*-*-*-*",
#endif
				"Multifixed*mw.fontList: -*-terminus-medium-r-*-*-10-*-*-*-*-*-*-*,"
						"-*-terminus-bold-r-*-*-10-*-*-*-*-*-*-*=bold,"
						"-*-terminus-medium-r-*-*-10-*-*-*-*-*-*-*=italic",
				"Multifixed*mw.labelFont: -*-terminus-bold-r-*-*-14-*-*-*-*-*-*-*",
				"Multifixed*mw.cellShadowThickness:		1",
				"Multifixed*mw.textShadowThickness:		0",
				"Multifixed*mw.cellHighlightThickness:		2",
				"Multifixed*mw.cellMarginHeight:		0",
				"Multifixed*mw.cellMarginWidth:		1",
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
	XbaeMatrixEnterCellCallbackStruct *cbs = (XbaeMatrixEnterCellCallbackStruct*)cb;
	cbs->map = True;
	cbs->doit = False;
	cbs->select_text = True;
}

void *myThreadFun(int *i)
{
    sleep(1);
//    printf("Printing GeeksQuiz from Thread \n");
    printf("%d", *i);
    return NULL;
}

int main(int argc, char *argv[]) {
	Widget toplevel, mw;
	XtAppContext app;
	int row, column, n_rows, n_columns;
	if (XInitThreads() != True) {
		docker_log_error("X could not initialize threads, will exit now.");
		exit(VREX_X_HAS_NO_THREADS);
	} else {
		docker_log_info("XInitThreads successful.");
	}

	toplevel = XtVaAppInitialize(&app, "Multifixed",
	NULL, 0, &argc, argv, fallback,
	NULL);

	Boolean has_threads = XtToolkitThreadInitialize();
	printf("%d has thread\n", has_threads);

//    pthread_t thread_id;
//    printf("Before Thread\n");
//    int x = 100;
//    pthread_create(&thread_id, NULL, myThreadFun, &x);
//    pthread_join(thread_id, NULL);
//    printf("After Thread\n");

	docker_log_set_level(LOG_INFO);

	XFontStruct *plain_font = XLoadQueryFont(XtDisplay(toplevel),
			"-*-terminus-medium-r-*-*-14-*-*-*-*-*-*-*");
	XmFontListEntry font_list_entry = XmFontListEntryCreate(
	XmFONTLIST_DEFAULT_TAG, XmFONT_IS_FONT, plain_font);

	XmFontList plain_font_list = XmFontListAppendEntry(
	NULL, font_list_entry);

	printf("plain font: %d plain font list %d\n", plain_font == NULL,
			plain_font_list == NULL);

	mw = XtVaCreateManagedWidget("mw", xbaeMatrixWidgetClass, toplevel,
	XmNlabelFont, plain_font_list,
	XmNfontList, plain_font_list,
	NULL);

	XmFontListEntryFree(&font_list_entry);
	XmFontListFree(plain_font_list);

    pthread_t thread_id;
    printf("Before Thread\n");
    int x = 100;

	XtAddCallback(mw, XmNlabelActivateCallback, labelCB, NULL);
	XtAddCallback(mw, XmNenterCellCallback, cellCB, NULL);

	XtRealizeWidget(toplevel);
    pthread_create(&thread_id, NULL, list_containers, &mw);
	XtAppMainLoop(app);

//
//	list_containers(mw);
//	LoadMatrix(mw);
//	n_rows = XbaeMatrixNumRows(mw);
//	n_columns = XbaeMatrixNumColumns(mw);
//
//    for(row = 0; row < n_rows; row++) {
//        for(column = 0; column < n_columns; column++) {
//            if (column % 2) {
//                XbaeMatrixSetCellTag(mw, row, column, "italic");
//            }
//        }
//    }
//
//    bold(mw);

	/*NOTREACHED*/

	return 0;
}

