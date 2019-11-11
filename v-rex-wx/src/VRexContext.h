#pragma once
#include <docker_connection_util.h>
#include <docker_system.h>

#include "vrex_util.h"

class VRexContext {
public:
	VRexContext();
	bool isConnected();
	docker_context* getDockerContext();
	docker_version* getDockerVersion();
	docker_info* getDockerInfo();
	char* handleDockerResult(docker_result* res);

	vrex_err_t TryConnectLocal();
	vrex_err_t TryConnectURL(const char* url);
private:
	docker_context* docker_ctx;
	docker_version* docker_version;
	arraylist* results;
};

