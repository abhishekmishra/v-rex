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

#include "vrex_util.h"
#include "docker_connection_util.h"
#include "log.h"

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
	}
	return VREX_SUCCESS;
}
