/*
 * v-rex: VRexUtil.h
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

// just declare event types
wxDECLARE_EVENT(DOCKER_CONNECT_EVENT, wxCommandEvent);
wxDECLARE_EVENT(PAGE_REFRESH_EVENT, wxCommandEvent);
wxDECLARE_EVENT(DOCKER_INTERACTION_RESULT_EVENT, wxCommandEvent);
wxDECLARE_EVENT(DOCKER_INTERACTION_HIDE_EVENT, wxCommandEvent);


// See X11 colours
// https://en.wikipedia.org/wiki/X11_color_names

#define VREX_ALICEBLUE wxT("VREX_ALICEBLUE")
#define VREX_ANTIQUEWHITE wxT("VREX_ANTIQUEWHITE")
#define VREX_AQUA wxT("VREX_AQUA")
#define VREX_AQUAMARINE wxT("VREX_AQUAMARINE")
#define VREX_AZURE wxT("VREX_AZURE")
#define VREX_BEIGE wxT("VREX_BEIGE")
#define VREX_BISQUE wxT("VREX_BISQUE")
#define VREX_BLACK wxT("VREX_BLACK")
#define VREX_BLANCHEDALMOND wxT("VREX_BLANCHEDALMOND")
#define VREX_BLUE wxT("VREX_BLUE")
#define VREX_BLUEVIOLET wxT("VREX_BLUEVIOLET")
#define VREX_BROWN wxT("VREX_BROWN")
#define VREX_BURLYWOOD wxT("VREX_BURLYWOOD")
#define VREX_CADETBLUE wxT("VREX_CADETBLUE")
#define VREX_CHARTREUSE wxT("VREX_CHARTREUSE")
#define VREX_CHOCOLATE wxT("VREX_CHOCOLATE")
#define VREX_CORAL wxT("VREX_CORAL")
#define VREX_CORNFLOWERBLUE wxT("VREX_CORNFLOWERBLUE")
#define VREX_CORNSILK wxT("VREX_CORNSILK")
#define VREX_CRIMSON wxT("VREX_CRIMSON")
#define VREX_CYAN wxT("VREX_CYAN")
#define VREX_DARKBLUE wxT("VREX_DARKBLUE")
#define VREX_DARKCYAN wxT("VREX_DARKCYAN")
#define VREX_DARKGOLDENROD wxT("VREX_DARKGOLDENROD")
#define VREX_DARKGRAY wxT("VREX_DARKGRAY")
#define VREX_DARKGREEN wxT("VREX_DARKGREEN")
#define VREX_DARKKHAKI wxT("VREX_DARKKHAKI")
#define VREX_DARKMAGENTA wxT("VREX_DARKMAGENTA")
#define VREX_DARKOLIVEGREEN wxT("VREX_DARKOLIVEGREEN")
#define VREX_DARKORANGE wxT("VREX_DARKORANGE")
#define VREX_DARKORCHID wxT("VREX_DARKORCHID")
#define VREX_DARKRED wxT("VREX_DARKRED")
#define VREX_DARKSALMON wxT("VREX_DARKSALMON")
#define VREX_DARKSEAGREEN wxT("VREX_DARKSEAGREEN")
#define VREX_DARKSLATEBLUE wxT("VREX_DARKSLATEBLUE")
#define VREX_DARKSLATEGRAY wxT("VREX_DARKSLATEGRAY")
#define VREX_DARKTURQUOISE wxT("VREX_DARKTURQUOISE")
#define VREX_DARKVIOLET wxT("VREX_DARKVIOLET")
#define VREX_DEEPPINK wxT("VREX_DEEPPINK")
#define VREX_DEEPSKYBLUE wxT("VREX_DEEPSKYBLUE")
#define VREX_DIMGRAY wxT("VREX_DIMGRAY")
#define VREX_DODGERBLUE wxT("VREX_DODGERBLUE")
#define VREX_FIREBRICK wxT("VREX_FIREBRICK")
#define VREX_FLORALWHITE wxT("VREX_FLORALWHITE")
#define VREX_FORESTGREEN wxT("VREX_FORESTGREEN")
#define VREX_FUCHSIA wxT("VREX_FUCHSIA")
#define VREX_GAINSBORO wxT("VREX_GAINSBORO")
#define VREX_GHOSTWHITE wxT("VREX_GHOSTWHITE")
#define VREX_GOLD wxT("VREX_GOLD")
#define VREX_GOLDENROD wxT("VREX_GOLDENROD")
#define VREX_GRAY wxT("VREX_GRAY")
#define VREX_WEBGRAY wxT("VREX_WEBGRAY")
#define VREX_GREEN wxT("VREX_GREEN")
#define VREX_WEBGREEN wxT("VREX_WEBGREEN")
#define VREX_GREENYELLOW wxT("VREX_GREENYELLOW")
#define VREX_HONEYDEW wxT("VREX_HONEYDEW")
#define VREX_HOTPINK wxT("VREX_HOTPINK")
#define VREX_INDIANRED wxT("VREX_INDIANRED")
#define VREX_INDIGO wxT("VREX_INDIGO")
#define VREX_IVORY wxT("VREX_IVORY")
#define VREX_KHAKI wxT("VREX_KHAKI")
#define VREX_LAVENDER wxT("VREX_LAVENDER")
#define VREX_LAVENDERBLUSH wxT("VREX_LAVENDERBLUSH")
#define VREX_LAWNGREEN wxT("VREX_LAWNGREEN")
#define VREX_LEMONCHIFFON wxT("VREX_LEMONCHIFFON")
#define VREX_LIGHTBLUE wxT("VREX_LIGHTBLUE")
#define VREX_LIGHTCORAL wxT("VREX_LIGHTCORAL")
#define VREX_LIGHTCYAN wxT("VREX_LIGHTCYAN")
#define VREX_LIGHTGOLDENROD wxT("VREX_LIGHTGOLDENROD")
#define VREX_LIGHTGRAY wxT("VREX_LIGHTGRAY")
#define VREX_LIGHTGREEN wxT("VREX_LIGHTGREEN")
#define VREX_LIGHTPINK wxT("VREX_LIGHTPINK")
#define VREX_LIGHTSALMON wxT("VREX_LIGHTSALMON")
#define VREX_LIGHTSEAGREEN wxT("VREX_LIGHTSEAGREEN")
#define VREX_LIGHTSKYBLUE wxT("VREX_LIGHTSKYBLUE")
#define VREX_LIGHTSLATEGRAY wxT("VREX_LIGHTSLATEGRAY")
#define VREX_LIGHTSTEELBLUE wxT("VREX_LIGHTSTEELBLUE")
#define VREX_LIGHTYELLOW wxT("VREX_LIGHTYELLOW")
#define VREX_LIME wxT("VREX_LIME")
#define VREX_LIMEGREEN wxT("VREX_LIMEGREEN")
#define VREX_LINEN wxT("VREX_LINEN")
#define VREX_MAGENTA wxT("VREX_MAGENTA")
#define VREX_MAROON wxT("VREX_MAROON")
#define VREX_WEBMAROON wxT("VREX_WEBMAROON")
#define VREX_MEDIUMAQUAMARINE wxT("VREX_MEDIUMAQUAMARINE")
#define VREX_MEDIUMBLUE wxT("VREX_MEDIUMBLUE")
#define VREX_MEDIUMORCHID wxT("VREX_MEDIUMORCHID")
#define VREX_MEDIUMPURPLE wxT("VREX_MEDIUMPURPLE")
#define VREX_MEDIUMSEAGREEN wxT("VREX_MEDIUMSEAGREEN")
#define VREX_MEDIUMSLATEBLUE wxT("VREX_MEDIUMSLATEBLUE")
#define VREX_MEDIUMSPRINGGREEN wxT("VREX_MEDIUMSPRINGGREEN")
#define VREX_MEDIUMTURQUOISE wxT("VREX_MEDIUMTURQUOISE")
#define VREX_MEDIUMVIOLETRED wxT("VREX_MEDIUMVIOLETRED")
#define VREX_MIDNIGHTBLUE wxT("VREX_MIDNIGHTBLUE")
#define VREX_MINTCREAM wxT("VREX_MINTCREAM")
#define VREX_MISTYROSE wxT("VREX_MISTYROSE")
#define VREX_MOCCASIN wxT("VREX_MOCCASIN")
#define VREX_NAVAJOWHITE wxT("VREX_NAVAJOWHITE")
#define VREX_NAVYBLUE wxT("VREX_NAVYBLUE")
#define VREX_OLDLACE wxT("VREX_OLDLACE")
#define VREX_OLIVE wxT("VREX_OLIVE")
#define VREX_OLIVEDRAB wxT("VREX_OLIVEDRAB")
#define VREX_ORANGE wxT("VREX_ORANGE")
#define VREX_ORANGERED wxT("VREX_ORANGERED")
#define VREX_ORCHID wxT("VREX_ORCHID")
#define VREX_PALEGOLDENROD wxT("VREX_PALEGOLDENROD")
#define VREX_PALEGREEN wxT("VREX_PALEGREEN")
#define VREX_PALETURQUOISE wxT("VREX_PALETURQUOISE")
#define VREX_PALEVIOLETRED wxT("VREX_PALEVIOLETRED")
#define VREX_PAPAYAWHIP wxT("VREX_PAPAYAWHIP")
#define VREX_PEACHPUFF wxT("VREX_PEACHPUFF")
#define VREX_PERU wxT("VREX_PERU")
#define VREX_PINK wxT("VREX_PINK")
#define VREX_PLUM wxT("VREX_PLUM")
#define VREX_POWDERBLUE wxT("VREX_POWDERBLUE")
#define VREX_PURPLE wxT("VREX_PURPLE")
#define VREX_WEBPURPLE wxT("VREX_WEBPURPLE")
#define VREX_REBECCAPURPLE wxT("VREX_REBECCAPURPLE")
#define VREX_RED wxT("VREX_RED")
#define VREX_ROSYBROWN wxT("VREX_ROSYBROWN")
#define VREX_ROYALBLUE wxT("VREX_ROYALBLUE")
#define VREX_SADDLEBROWN wxT("VREX_SADDLEBROWN")
#define VREX_SALMON wxT("VREX_SALMON")
#define VREX_SANDYBROWN wxT("VREX_SANDYBROWN")
#define VREX_SEAGREEN wxT("VREX_SEAGREEN")
#define VREX_SEASHELL wxT("VREX_SEASHELL")
#define VREX_SIENNA wxT("VREX_SIENNA")
#define VREX_SILVER wxT("VREX_SILVER")
#define VREX_SKYBLUE wxT("VREX_SKYBLUE")
#define VREX_SLATEBLUE wxT("VREX_SLATEBLUE")
#define VREX_SLATEGRAY wxT("VREX_SLATEGRAY")
#define VREX_SNOW wxT("VREX_SNOW")
#define VREX_SPRINGGREEN wxT("VREX_SPRINGGREEN")
#define VREX_STEELBLUE wxT("VREX_STEELBLUE")
#define VREX_TAN wxT("VREX_TAN")
#define VREX_TEAL wxT("VREX_TEAL")
#define VREX_THISTLE wxT("VREX_THISTLE")
#define VREX_TOMATO wxT("VREX_TOMATO")
#define VREX_TURQUOISE wxT("VREX_TURQUOISE")
#define VREX_VIOLET wxT("VREX_VIOLET")
#define VREX_WHEAT wxT("VREX_WHEAT")
#define VREX_WHITE wxT("VREX_WHITE")
#define VREX_WHITESMOKE wxT("VREX_WHITESMOKE")
#define VREX_YELLOW wxT("VREX_YELLOW")
#define VREX_YELLOWGREEN wxT("VREX_YELLOWGREEN")

void registerX11Colours();

#endif /* SRC_VREX_UTIL_H_ */
