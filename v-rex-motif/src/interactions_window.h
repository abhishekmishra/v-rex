/*
* v-rex: interactions_window.h
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

#ifndef SRC_INTERACTIONS_WINDOW_H_
#define SRC_INTERACTIONS_WINDOW_H_

#include "vrex_util.h"

vrex_err_t make_interactions_window(vrex_context* vrex);

vrex_err_t add_interactions_entry(vrex_context* vrex, docker_result* res);

vrex_err_t show_interactions_window(vrex_context* vrex);
vrex_err_t hide_interactions_window(vrex_context* vrex);

#endif /* SRC_INTERACTIONS_WINDOW_H_ */
