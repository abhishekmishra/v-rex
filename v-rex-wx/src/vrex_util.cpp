/*
 * v-rex: vrex_util.c
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
#include "vrex_util.h"
#include <arraylist.h>

#include <docker_connection_util.h>
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

void registerWebColours() {
	wxTheColourDatabase->AddColour(wxT("VREX_WHITESMOKE"), wxColour(245, 245, 245));
}