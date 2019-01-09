/*
* v-rex: container_window.h
* Created on: 27-Dec-2018
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

#ifndef SRC_CONTAINER_WINDOW_H_
#define SRC_CONTAINER_LIST_WINDOW_H_

#include "docker_connection_util.h"

// View methods

void create_container_toolbar(vrex_context* vrex, Widget toolbar_w);

/**
 * Create a new container list window
 *
 * \param parent window
 * \param container_ls_w handle to populate with created widget.
 * \return error code
 */
int make_container_list_window(Widget parent, Widget* container_ls_w, vrex_context* vrex);

/**
 * Set the docker container id for the table
 *
 * \param vrex already initialized vrex_context
 * \return error code
 */
int refresh_containers_list(vrex_context* vrex);

int list_containers(Widget mw, vrex_context* ctx);

#endif /* SRC_CONTAINER_LIST_WINDOW_H_ */
