#pragma once
#include "docker_connection_util.h"

int extract_args_url_connection(int argc, LPWSTR* url_ret, LPWSTR* argv, docker_context** ctx, docker_result** res);