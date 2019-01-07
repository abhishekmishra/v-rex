#include "stdafx.h"
#include <stdlib.h>
#include "vrex-util.h"
#include <docker_connection_util.h>
#include <docker_system.h>
#include <log.h>

int extract_args_url_connection(const char* url, docker_context** ctx, docker_result** res) {
	size_t url_len;
	int connected = 0;
//	char* url = (char*)calloc(2 * wcslen(urlw), sizeof(char));
//	wcstombs_s(&url_len, url, 1024, urlw, 1024);
	if (is_http_url((char*)url)) {
		if (make_docker_context_url(ctx, url) == E_SUCCESS) {
			connected = 1;
		}
	}
	else if (is_unix_socket((char*)url)) {
		if (make_docker_context_socket(ctx, url) == E_SUCCESS) {
			connected = 1;
		}
	}
	if (connected) {
		if (docker_ping((*ctx), res) != E_SUCCESS) {
			//			docker_log_fatal("Could not ping the server %s", url);
			connected = 0;
		}
		else {
			//			docker_log_info("%s is alive.", url);
		}
	}

	return connected;
}
