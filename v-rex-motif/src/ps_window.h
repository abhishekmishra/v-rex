/*
 * ps_window.h
 *
 *  Created on: 25-Dec-2018
 *      Author: abhishek
 */

#ifndef SRC_PS_WINDOW_H_
#define SRC_PS_WINDOW_H_

#include <Xm/MainW.h>
#include <Xm/Label.h>
#include <Xbae/Matrix.h>
#include <docker_containers.h>
#include "vrex_util.h"

// View methods

/**
 * Create a new ps_window
 *
 * \param parent window
 * \param ps_w handle to populate with created widget.
 * \return error code
 */
int make_ps_window(Widget parent, Widget* ps_w);

/**
 * Set the docker container id for the table
 *
 * \param ps_w the window handle
 * \param vrex already initialized vrex_context
 * \param id container id
 * \return error code
 */
int set_ps_window_docker_id(Widget ps_w, vrex_context* vrex, char* id);

// Model methods

// None use clibdocker directly


#endif /* SRC_PS_WINDOW_H_ */
