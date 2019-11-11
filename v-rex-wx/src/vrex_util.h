/*
 * v-rex: vrex_util.h
 * Created on: 27-Dec-2018
 *
 * V-Rex
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
 *
 */

#ifndef SRC_VREX_UTIL_H_
#define SRC_VREX_UTIL_H_

#include <wx/wx.h>

#include "docker_result.h"
#include "docker_connection_util.h"
#include <arraylist.h>

typedef enum {
	VREX_SUCCESS = 0, VREX_E_UNKNOWN = 1
} vrex_err_t;

typedef struct vrex_context_t {
	docker_context* d_ctx;
	void (*handle_error)(struct vrex_context_t* vrex, docker_result* res);
	arraylist* results;
} vrex_context;

vrex_err_t init_results_list(vrex_context* vrex);
vrex_err_t results_list_add(vrex_context* vrex, docker_result* res);
int results_list_length(vrex_context* vrex);
docker_result* results_list_get_idx(vrex_context* vrex, int i);

/**
 * Get the size in bytes, kb, mb, gb, tb etc.
 * (higest possible unit)
 *
 * \param size
 * \return string with the description of the size
 */
char* calculate_size_str(uint64_t size);

char* handle_error(docker_result* res);

// it just declares DOCKER_CONNECT_EVENT event type
wxDECLARE_EVENT(DOCKER_CONNECT_EVENT, wxCommandEvent);

#endif /* SRC_VREX_UTIL_H_ */
