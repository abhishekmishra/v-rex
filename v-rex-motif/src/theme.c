/*
 * v-rex-motif: theme.c
 * Created on: 02-Jan-2019
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
#include <X11/Intrinsic.h>
#include "theme.h"

static String null_fallback[] = { NULL };

static String dark_fallback[] =
		{ "V-Rex*main_w.width:		1024", "V-Rex*main_w.height:		768",
				"V-Rex*main_w.borderWidth:		10",
				"V-Rex*main_w.shadowThickness:		0",
				"V-Rex*.background:		#000000", "V-Rex*.foreground:		#C0C0C0",
				"V-Rex*.borderColor:		#000000",
				"V-Rex*.highlightColor:		#008080",
				"V-Rex*.bottomShadowColor:		#333333",
				"V-Rex*.topShadowColor:		#333333", "V-Rex*.shadowThickness: 1",
				"V-Rex*.selectColor: #2F4F4F", "V-Rex*.unselectColor: #000000",

// Frame label colors
				"V-Rex*Volumes.foreground:							#696969",
				"V-Rex*Networks.foreground:							#696969",
				"V-Rex*Images.foreground:							#696969",
				"V-Rex*Docker Events.foreground:					#696969",
				"V-Rex*Messages.foreground:							#696969",
				"V-Rex*Containers.foreground:						#696969",
				"V-Rex*Container PS.foreground:						#696969",
				"V-Rex*Docker API Interactions.foreground:			#696969",
				"V-Rex*Docker Server Summary.foreground:			#696969",

//TODO: see https://gist.github.com/unix-junkie/68bdf8420d6c7b7925f4
// and https://stackoverflow.com/questions/34360066/xmstringgenerate-in-xmmultibyte-text-or-xmwidechar-text-mode
// for UTF-8
//UTF-8 font	"V-Rex*fontList: terminus-12",
				"V-Rex*fontList: -*-terminus-medium-r-*-*-12-*-*-*-*-*-*-*",
				"V-Rex*labelFont: -*-terminus-medium-r-*-*-12-*-*-*-*-*-*-*",
				"V-Rex*docker_server_summary_text.fontList: -*-terminus-bold-r-*-*-14-*-*-*-*-*-*-*",
//For tables
				"V-Rex*columnLabelColor: #B0E0E6",
				NULL };

static String light_fallback[] =
		{ "V-Rex*main_w.width:		1024", "V-Rex*main_w.height:		768",
				"V-Rex*main_w.borderWidth:		0",
				"V-Rex*main_w.shadowThickness:		0",
				"V-Rex*.background:		#DCDCDC", "V-Rex*.foreground:		#000000",
				"V-Rex*.borderColor:		#A9A9A9",
				"V-Rex*.highlightColor:		#008080",
				"V-Rex*.bottomShadowColor:		#696969",
				"V-Rex*.topShadowColor:		#696969", "V-Rex*.shadowThickness: 1",
				"V-Rex*.selectColor: #B0E0E6", "V-Rex*.unselectColor: #FFFFFF",

//TODO: see https://gist.github.com/unix-junkie/68bdf8420d6c7b7925f4
// and https://stackoverflow.com/questions/34360066/xmstringgenerate-in-xmmultibyte-text-or-xmwidechar-text-mode
// for UTF-8
//UTF-8 font	"V-Rex*fontList: terminus-12",
				"V-Rex*fontList: -*-terminus-medium-r-*-*-12-*-*-*-*-*-*-*",
				"V-Rex*labelFont: -*-lucida-medium-r-*-*-12-*-*-*-*-*-*-*",
				"V-Rex*docker_server_summary_text.fontList: -*-terminus-bold-r-*-*-12-*-*-*-*-*-*-*",
//For tables
				"V-Rex*columnLabelColor: #F5DEB3",
				NULL };


String* get_null_theme() {
	return null_fallback;
}

String* get_dark_theme() {
	return dark_fallback;
}

String* get_light_theme() {
	return light_fallback;
}
