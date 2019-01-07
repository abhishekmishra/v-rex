#include "stdafx.h"
#include <stdlib.h>
#include "vrex-util.h"
#include "docker_connection_util.h"
#include "docker_system.h"
#include "log.h"

int extract_args_url_connection(int argc, LPWSTR* url_ret, LPWSTR* argv,
	docker_context** ctx, docker_result** res) {
	size_t url_len;
	int connected = 0;
	if (argc > 0) {
		char* url = (char*)calloc(2 * wcslen(argv[0]), sizeof(char));
		wcstombs_s(&url_len, url, 1024, argv[0], 1024);
		//url = argv[1];
		if (is_http_url(url)) {
			if (make_docker_context_url(ctx, url) == E_SUCCESS) {
				connected = 1;
			}
		}
		else if (is_unix_socket(url)) {
			if (make_docker_context_socket(ctx, url) == E_SUCCESS) {
				connected = 1;
			}
		}
		size_t url_ret_len;
		mbstowcs_s(&url_ret_len, (*url_ret), 1024, url, 1024);
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
