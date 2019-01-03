/*
 * statusbar_w.h
 *
 *  Created on: Jan 3, 2019
 *      Author: abhishek
 */

#ifndef SRC_STATUSBAR_WINDOW_H_
#define SRC_STATUSBAR_WINDOW_H_

/**
 * Create a new status bar and register as the messageWindow of the main window provided.
 *
 * \param vrex the app context
 * \param main_w the main window widget
 * \return error code
 */
vrex_err_t create_statusbar(vrex_context* vrex, Widget main_w);

/**
 * Find and return the statusbar window.
 */
Widget statusbar_w(vrex_context* vrex);

/**
 * Set the message on the status bar.
 *
 * \param vrex app context
 * \param msg string message
 * \return error code.
 */
vrex_err_t set_statusbar_message(vrex_context* vrex, char* msg);

/**
 * Set the url on the status bar.
 *
 * \param vrex app context
 * \param msg the connection url
 * \return error code.
 */
vrex_err_t set_statusbar_url(vrex_context* vrex, char* url);

#endif /* SRC_STATUSBAR_WINDOW_H_ */
