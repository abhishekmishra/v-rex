/*
* v-rex: vrex_util.c
* Created on: 27-Dec-2018
*
*/

#include <Xbae/Matrix.h>
#include "vrex_util.h"

void add_column(Widget mw, char* name, int num, int width) {
	XbaeMatrixSetColumnLabel(mw, num, name);
	XbaeMatrixSetColumnWidth(mw, num, width);
}

