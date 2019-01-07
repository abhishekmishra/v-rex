#pragma once
#include <docker_connection_util.h>

int extract_args_url_connection(const char* url, docker_context** ctx, docker_result** res);