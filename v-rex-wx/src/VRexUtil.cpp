/*
 * v-rex: VRexUtil.cpp
 * Created on: 27-Dec-2018
 *
 */

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "VRexUtil.h"
#include <arraylist.h>

#include <docker_all.h>
#include <docker_system.h>
#include <docker_log.h>

vrex_err_t init_results_list(vrex_context* vrex) {
	if (vrex != NULL) {
		arraylist_new(&vrex->results, (void (*)(void *)) &free_docker_result);
		return VREX_SUCCESS;
	}
	return VREX_E_UNKNOWN;
}

vrex_err_t results_list_add(vrex_context* vrex, docker_result* res) {
	if (vrex != NULL && vrex->results != NULL) {
		arraylist_add(vrex->results, res);
		return VREX_SUCCESS;
	}
	return VREX_E_UNKNOWN;
}

int results_list_length(vrex_context* vrex) {
	if (vrex != NULL && vrex->results != NULL) {
		return arraylist_length(vrex->results);
	}
	return 0;
}

docker_result* results_list_get_idx(vrex_context* vrex, int i) {
	return (docker_result*) arraylist_get(vrex->results, i);
}

// see https://stackoverflow.com/questions/3898840/converting-a-number-of-bytes-into-a-file-size-in-c
#define DIM(x) (sizeof(x)/sizeof(*(x)))

static const char     *sizes[]   = { "EiB", "PiB", "TiB", "GiB", "MiB", "KiB", "B" };
static const uint64_t  exbibytes = 1024ULL * 1024ULL * 1024ULL *
                                   1024ULL * 1024ULL * 1024ULL;

char* calculate_size_str(uint64_t size)
{
    char     *result = (char *) malloc(sizeof(char) * 20);
    uint64_t  multiplier = exbibytes;
    int i;

    for (i = 0; i < DIM(sizes); i++, multiplier /= 1024)
    {
        if (size < multiplier)
            continue;
        if (size % multiplier == 0)
            sprintf(result, "%" PRIu64 " %s", size / multiplier, sizes[i]);
        else
            sprintf(result, "%.1f %s", (float) size / multiplier, sizes[i]);
        return result;
    }
    strcpy(result, "0");
    return result;
}

/**
 * A simple error handler suitable for programs
 * which just want to log the error (if any).
 */
char* docker_simple_error_handler_sprintf(docker_result* res) {
	docker_log_debug("Result error code is %d\n.", res->error_code);
	if(res->error_code == E_SUCCESS) {
		char* report = (char*)calloc(2048, sizeof(char));
		sprintf(report,
			"DOCKER_RESULT: For URL: %s\n DOCKER RESULT: Response error_code = %d, http_response = %ld\n",
			res->url,
			res->error_code,
			res->http_error_code);
		if (!is_ok(res)) {
			sprintf(report, "DOCKER RESULT: %s\n", res->message);
		}
		return report;
	}
	else {
		return NULL;
	}
}

char* handle_error(docker_result* res) {
	return docker_simple_error_handler_sprintf(res);
}

void registerX11Colours() {
	// generated from script web_colours_wx_decl.py (list of X11 colours)
	wxTheColourDatabase->AddColour(VREX_ALICEBLUE, wxColour(240, 248, 255)); //#F0F8FF
	wxTheColourDatabase->AddColour(VREX_ANTIQUEWHITE, wxColour(250, 235, 215)); //#FAEBD7
	wxTheColourDatabase->AddColour(VREX_AQUA, wxColour(0, 255, 255)); //#00FFFF
	wxTheColourDatabase->AddColour(VREX_AQUAMARINE, wxColour(127, 255, 212)); //#7FFFD4
	wxTheColourDatabase->AddColour(VREX_AZURE, wxColour(240, 255, 255)); //#F0FFFF
	wxTheColourDatabase->AddColour(VREX_BEIGE, wxColour(245, 245, 220)); //#F5F5DC
	wxTheColourDatabase->AddColour(VREX_BISQUE, wxColour(255, 228, 196)); //#FFE4C4
	wxTheColourDatabase->AddColour(VREX_BLACK, wxColour(0, 0, 0)); //#000000
	wxTheColourDatabase->AddColour(VREX_BLANCHEDALMOND, wxColour(255, 235, 205)); //#FFEBCD
	wxTheColourDatabase->AddColour(VREX_BLUE, wxColour(0, 0, 255)); //#0000FF
	wxTheColourDatabase->AddColour(VREX_BLUEVIOLET, wxColour(138, 43, 226)); //#8A2BE2
	wxTheColourDatabase->AddColour(VREX_BROWN, wxColour(165, 42, 42)); //#A52A2A
	wxTheColourDatabase->AddColour(VREX_BURLYWOOD, wxColour(222, 184, 135)); //#DEB887
	wxTheColourDatabase->AddColour(VREX_CADETBLUE, wxColour(95, 158, 160)); //#5F9EA0
	wxTheColourDatabase->AddColour(VREX_CHARTREUSE, wxColour(127, 255, 0)); //#7FFF00
	wxTheColourDatabase->AddColour(VREX_CHOCOLATE, wxColour(210, 105, 30)); //#D2691E
	wxTheColourDatabase->AddColour(VREX_CORAL, wxColour(255, 127, 80)); //#FF7F50
	wxTheColourDatabase->AddColour(VREX_CORNFLOWERBLUE, wxColour(100, 149, 237)); //#6495ED
	wxTheColourDatabase->AddColour(VREX_CORNSILK, wxColour(255, 248, 220)); //#FFF8DC
	wxTheColourDatabase->AddColour(VREX_CRIMSON, wxColour(220, 20, 60)); //#DC143C
	wxTheColourDatabase->AddColour(VREX_CYAN, wxColour(0, 255, 255)); //#00FFFF
	wxTheColourDatabase->AddColour(VREX_DARKBLUE, wxColour(0, 0, 139)); //#00008B
	wxTheColourDatabase->AddColour(VREX_DARKCYAN, wxColour(0, 139, 139)); //#008B8B
	wxTheColourDatabase->AddColour(VREX_DARKGOLDENROD, wxColour(184, 134, 11)); //#B8860B
	wxTheColourDatabase->AddColour(VREX_DARKGRAY, wxColour(169, 169, 169)); //#A9A9A9
	wxTheColourDatabase->AddColour(VREX_DARKGREEN, wxColour(0, 100, 0)); //#006400
	wxTheColourDatabase->AddColour(VREX_DARKKHAKI, wxColour(189, 183, 107)); //#BDB76B
	wxTheColourDatabase->AddColour(VREX_DARKMAGENTA, wxColour(139, 0, 139)); //#8B008B
	wxTheColourDatabase->AddColour(VREX_DARKOLIVEGREEN, wxColour(85, 107, 47)); //#556B2F
	wxTheColourDatabase->AddColour(VREX_DARKORANGE, wxColour(255, 140, 0)); //#FF8C00
	wxTheColourDatabase->AddColour(VREX_DARKORCHID, wxColour(153, 50, 204)); //#9932CC
	wxTheColourDatabase->AddColour(VREX_DARKRED, wxColour(139, 0, 0)); //#8B0000
	wxTheColourDatabase->AddColour(VREX_DARKSALMON, wxColour(233, 150, 122)); //#E9967A
	wxTheColourDatabase->AddColour(VREX_DARKSEAGREEN, wxColour(143, 188, 143)); //#8FBC8F
	wxTheColourDatabase->AddColour(VREX_DARKSLATEBLUE, wxColour(72, 61, 139)); //#483D8B
	wxTheColourDatabase->AddColour(VREX_DARKSLATEGRAY, wxColour(47, 79, 79)); //#2F4F4F
	wxTheColourDatabase->AddColour(VREX_DARKTURQUOISE, wxColour(0, 206, 209)); //#00CED1
	wxTheColourDatabase->AddColour(VREX_DARKVIOLET, wxColour(148, 0, 211)); //#9400D3
	wxTheColourDatabase->AddColour(VREX_DEEPPINK, wxColour(255, 20, 147)); //#FF1493
	wxTheColourDatabase->AddColour(VREX_DEEPSKYBLUE, wxColour(0, 191, 255)); //#00BFFF
	wxTheColourDatabase->AddColour(VREX_DIMGRAY, wxColour(105, 105, 105)); //#696969
	wxTheColourDatabase->AddColour(VREX_DODGERBLUE, wxColour(30, 144, 255)); //#1E90FF
	wxTheColourDatabase->AddColour(VREX_FIREBRICK, wxColour(178, 34, 34)); //#B22222
	wxTheColourDatabase->AddColour(VREX_FLORALWHITE, wxColour(255, 250, 240)); //#FFFAF0
	wxTheColourDatabase->AddColour(VREX_FORESTGREEN, wxColour(34, 139, 34)); //#228B22
	wxTheColourDatabase->AddColour(VREX_FUCHSIA, wxColour(255, 0, 255)); //#FF00FF
	wxTheColourDatabase->AddColour(VREX_GAINSBORO, wxColour(220, 220, 220)); //#DCDCDC
	wxTheColourDatabase->AddColour(VREX_GHOSTWHITE, wxColour(248, 248, 255)); //#F8F8FF
	wxTheColourDatabase->AddColour(VREX_GOLD, wxColour(255, 215, 0)); //#FFD700
	wxTheColourDatabase->AddColour(VREX_GOLDENROD, wxColour(218, 165, 32)); //#DAA520
	wxTheColourDatabase->AddColour(VREX_GRAY, wxColour(190, 190, 190)); //#BEBEBE
	wxTheColourDatabase->AddColour(VREX_WEBGRAY, wxColour(128, 128, 128)); //#808080
	wxTheColourDatabase->AddColour(VREX_GREEN, wxColour(0, 255, 0)); //#00FF00
	wxTheColourDatabase->AddColour(VREX_WEBGREEN, wxColour(0, 128, 0)); //#008000
	wxTheColourDatabase->AddColour(VREX_GREENYELLOW, wxColour(173, 255, 47)); //#ADFF2F
	wxTheColourDatabase->AddColour(VREX_HONEYDEW, wxColour(240, 255, 240)); //#F0FFF0
	wxTheColourDatabase->AddColour(VREX_HOTPINK, wxColour(255, 105, 180)); //#FF69B4
	wxTheColourDatabase->AddColour(VREX_INDIANRED, wxColour(205, 92, 92)); //#CD5C5C
	wxTheColourDatabase->AddColour(VREX_INDIGO, wxColour(75, 0, 130)); //#4B0082
	wxTheColourDatabase->AddColour(VREX_IVORY, wxColour(255, 255, 240)); //#FFFFF0
	wxTheColourDatabase->AddColour(VREX_KHAKI, wxColour(240, 230, 140)); //#F0E68C
	wxTheColourDatabase->AddColour(VREX_LAVENDER, wxColour(230, 230, 250)); //#E6E6FA
	wxTheColourDatabase->AddColour(VREX_LAVENDERBLUSH, wxColour(255, 240, 245)); //#FFF0F5
	wxTheColourDatabase->AddColour(VREX_LAWNGREEN, wxColour(124, 252, 0)); //#7CFC00
	wxTheColourDatabase->AddColour(VREX_LEMONCHIFFON, wxColour(255, 250, 205)); //#FFFACD
	wxTheColourDatabase->AddColour(VREX_LIGHTBLUE, wxColour(173, 216, 230)); //#ADD8E6
	wxTheColourDatabase->AddColour(VREX_LIGHTCORAL, wxColour(240, 128, 128)); //#F08080
	wxTheColourDatabase->AddColour(VREX_LIGHTCYAN, wxColour(224, 255, 255)); //#E0FFFF
	wxTheColourDatabase->AddColour(VREX_LIGHTGOLDENROD, wxColour(250, 250, 210)); //#FAFAD2
	wxTheColourDatabase->AddColour(VREX_LIGHTGRAY, wxColour(211, 211, 211)); //#D3D3D3
	wxTheColourDatabase->AddColour(VREX_LIGHTGREEN, wxColour(144, 238, 144)); //#90EE90
	wxTheColourDatabase->AddColour(VREX_LIGHTPINK, wxColour(255, 182, 193)); //#FFB6C1
	wxTheColourDatabase->AddColour(VREX_LIGHTSALMON, wxColour(255, 160, 122)); //#FFA07A
	wxTheColourDatabase->AddColour(VREX_LIGHTSEAGREEN, wxColour(32, 178, 170)); //#20B2AA
	wxTheColourDatabase->AddColour(VREX_LIGHTSKYBLUE, wxColour(135, 206, 250)); //#87CEFA
	wxTheColourDatabase->AddColour(VREX_LIGHTSLATEGRAY, wxColour(119, 136, 153)); //#778899
	wxTheColourDatabase->AddColour(VREX_LIGHTSTEELBLUE, wxColour(176, 196, 222)); //#B0C4DE
	wxTheColourDatabase->AddColour(VREX_LIGHTYELLOW, wxColour(255, 255, 224)); //#FFFFE0
	wxTheColourDatabase->AddColour(VREX_LIME, wxColour(0, 255, 0)); //#00FF00
	wxTheColourDatabase->AddColour(VREX_LIMEGREEN, wxColour(50, 205, 50)); //#32CD32
	wxTheColourDatabase->AddColour(VREX_LINEN, wxColour(250, 240, 230)); //#FAF0E6
	wxTheColourDatabase->AddColour(VREX_MAGENTA, wxColour(255, 0, 255)); //#FF00FF
	wxTheColourDatabase->AddColour(VREX_MAROON, wxColour(176, 48, 96)); //#B03060
	wxTheColourDatabase->AddColour(VREX_WEBMAROON, wxColour(128, 0, 0)); //#800000
	wxTheColourDatabase->AddColour(VREX_MEDIUMAQUAMARINE, wxColour(102, 205, 170)); //#66CDAA
	wxTheColourDatabase->AddColour(VREX_MEDIUMBLUE, wxColour(0, 0, 205)); //#0000CD
	wxTheColourDatabase->AddColour(VREX_MEDIUMORCHID, wxColour(186, 85, 211)); //#BA55D3
	wxTheColourDatabase->AddColour(VREX_MEDIUMPURPLE, wxColour(147, 112, 219)); //#9370DB
	wxTheColourDatabase->AddColour(VREX_MEDIUMSEAGREEN, wxColour(60, 179, 113)); //#3CB371
	wxTheColourDatabase->AddColour(VREX_MEDIUMSLATEBLUE, wxColour(123, 104, 238)); //#7B68EE
	wxTheColourDatabase->AddColour(VREX_MEDIUMSPRINGGREEN, wxColour(0, 250, 154)); //#00FA9A
	wxTheColourDatabase->AddColour(VREX_MEDIUMTURQUOISE, wxColour(72, 209, 204)); //#48D1CC
	wxTheColourDatabase->AddColour(VREX_MEDIUMVIOLETRED, wxColour(199, 21, 133)); //#C71585
	wxTheColourDatabase->AddColour(VREX_MIDNIGHTBLUE, wxColour(25, 25, 112)); //#191970
	wxTheColourDatabase->AddColour(VREX_MINTCREAM, wxColour(245, 255, 250)); //#F5FFFA
	wxTheColourDatabase->AddColour(VREX_MISTYROSE, wxColour(255, 228, 225)); //#FFE4E1
	wxTheColourDatabase->AddColour(VREX_MOCCASIN, wxColour(255, 228, 181)); //#FFE4B5
	wxTheColourDatabase->AddColour(VREX_NAVAJOWHITE, wxColour(255, 222, 173)); //#FFDEAD
	wxTheColourDatabase->AddColour(VREX_NAVYBLUE, wxColour(0, 0, 128)); //#000080
	wxTheColourDatabase->AddColour(VREX_OLDLACE, wxColour(253, 245, 230)); //#FDF5E6
	wxTheColourDatabase->AddColour(VREX_OLIVE, wxColour(128, 128, 0)); //#808000
	wxTheColourDatabase->AddColour(VREX_OLIVEDRAB, wxColour(107, 142, 35)); //#6B8E23
	wxTheColourDatabase->AddColour(VREX_ORANGE, wxColour(255, 165, 0)); //#FFA500
	wxTheColourDatabase->AddColour(VREX_ORANGERED, wxColour(255, 69, 0)); //#FF4500
	wxTheColourDatabase->AddColour(VREX_ORCHID, wxColour(218, 112, 214)); //#DA70D6
	wxTheColourDatabase->AddColour(VREX_PALEGOLDENROD, wxColour(238, 232, 170)); //#EEE8AA
	wxTheColourDatabase->AddColour(VREX_PALEGREEN, wxColour(152, 251, 152)); //#98FB98
	wxTheColourDatabase->AddColour(VREX_PALETURQUOISE, wxColour(175, 238, 238)); //#AFEEEE
	wxTheColourDatabase->AddColour(VREX_PALEVIOLETRED, wxColour(219, 112, 147)); //#DB7093
	wxTheColourDatabase->AddColour(VREX_PAPAYAWHIP, wxColour(255, 239, 213)); //#FFEFD5
	wxTheColourDatabase->AddColour(VREX_PEACHPUFF, wxColour(255, 218, 185)); //#FFDAB9
	wxTheColourDatabase->AddColour(VREX_PERU, wxColour(205, 133, 63)); //#CD853F
	wxTheColourDatabase->AddColour(VREX_PINK, wxColour(255, 192, 203)); //#FFC0CB
	wxTheColourDatabase->AddColour(VREX_PLUM, wxColour(221, 160, 221)); //#DDA0DD
	wxTheColourDatabase->AddColour(VREX_POWDERBLUE, wxColour(176, 224, 230)); //#B0E0E6
	wxTheColourDatabase->AddColour(VREX_PURPLE, wxColour(160, 32, 240)); //#A020F0
	wxTheColourDatabase->AddColour(VREX_WEBPURPLE, wxColour(128, 0, 128)); //#800080
	wxTheColourDatabase->AddColour(VREX_REBECCAPURPLE, wxColour(102, 51, 153)); //#663399
	wxTheColourDatabase->AddColour(VREX_RED, wxColour(255, 0, 0)); //#FF0000
	wxTheColourDatabase->AddColour(VREX_ROSYBROWN, wxColour(188, 143, 143)); //#BC8F8F
	wxTheColourDatabase->AddColour(VREX_ROYALBLUE, wxColour(65, 105, 225)); //#4169E1
	wxTheColourDatabase->AddColour(VREX_SADDLEBROWN, wxColour(139, 69, 19)); //#8B4513
	wxTheColourDatabase->AddColour(VREX_SALMON, wxColour(250, 128, 114)); //#FA8072
	wxTheColourDatabase->AddColour(VREX_SANDYBROWN, wxColour(244, 164, 96)); //#F4A460
	wxTheColourDatabase->AddColour(VREX_SEAGREEN, wxColour(46, 139, 87)); //#2E8B57
	wxTheColourDatabase->AddColour(VREX_SEASHELL, wxColour(255, 245, 238)); //#FFF5EE
	wxTheColourDatabase->AddColour(VREX_SIENNA, wxColour(160, 82, 45)); //#A0522D
	wxTheColourDatabase->AddColour(VREX_SILVER, wxColour(192, 192, 192)); //#C0C0C0
	wxTheColourDatabase->AddColour(VREX_SKYBLUE, wxColour(135, 206, 235)); //#87CEEB
	wxTheColourDatabase->AddColour(VREX_SLATEBLUE, wxColour(106, 90, 205)); //#6A5ACD
	wxTheColourDatabase->AddColour(VREX_SLATEGRAY, wxColour(112, 128, 144)); //#708090
	wxTheColourDatabase->AddColour(VREX_SNOW, wxColour(255, 250, 250)); //#FFFAFA
	wxTheColourDatabase->AddColour(VREX_SPRINGGREEN, wxColour(0, 255, 127)); //#00FF7F
	wxTheColourDatabase->AddColour(VREX_STEELBLUE, wxColour(70, 130, 180)); //#4682B4
	wxTheColourDatabase->AddColour(VREX_TAN, wxColour(210, 180, 140)); //#D2B48C
	wxTheColourDatabase->AddColour(VREX_TEAL, wxColour(0, 128, 128)); //#008080
	wxTheColourDatabase->AddColour(VREX_THISTLE, wxColour(216, 191, 216)); //#D8BFD8
	wxTheColourDatabase->AddColour(VREX_TOMATO, wxColour(255, 99, 71)); //#FF6347
	wxTheColourDatabase->AddColour(VREX_TURQUOISE, wxColour(64, 224, 208)); //#40E0D0
	wxTheColourDatabase->AddColour(VREX_VIOLET, wxColour(238, 130, 238)); //#EE82EE
	wxTheColourDatabase->AddColour(VREX_WHEAT, wxColour(245, 222, 179)); //#F5DEB3
	wxTheColourDatabase->AddColour(VREX_WHITE, wxColour(255, 255, 255)); //#FFFFFF
	wxTheColourDatabase->AddColour(VREX_WHITESMOKE, wxColour(245, 245, 245)); //#F5F5F5
	wxTheColourDatabase->AddColour(VREX_YELLOW, wxColour(255, 255, 0)); //#FFFF00
	wxTheColourDatabase->AddColour(VREX_YELLOWGREEN, wxColour(154, 205, 50)); //#9ACD32
}