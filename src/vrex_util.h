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

#include <Xm/MainW.h>
#include "docker_result.h"

void handle_error(docker_result* res);

void add_column(Widget mw, char* name, int num, int width);

#endif /* SRC_VREX_UTIL_H_ */
