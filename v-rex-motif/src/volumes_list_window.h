/*
* v-rex-motif: volumes_list_window.h
* Created on: 08-Jan-2019
* 
* v-rex-motif
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

#ifndef SRC_VOLUMES_LIST_WINDOW_H_
#define SRC_VOLUMES_LIST_WINDOW_H_

#include <Xm/Xm.h>
#include "vrex_util.h"

/**
 * Create a new docker volumes list window
 *
 * \param vrex the app context
 * \param parent_w the parent window widget
 * \return error code.
 */
vrex_err_t make_docker_volumes_list_window(vrex_context* vrex,
		Widget parent_w);

/**
 * Refresh the network list in the window.
 *
 * \param vrex the app context
 * \param msg to add to the messages list
 * \return error code
 */
vrex_err_t refresh_volumes_list(vrex_context* vrex);

#endif /* SRC_VOLUMES_LIST_WINDOW_H_ */
