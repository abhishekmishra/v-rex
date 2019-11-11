#include "VRexContext.h"

#include <docker_log.h>
#include <vrex_util.h>

VRexContext::VRexContext() {
	this->docker_ctx = NULL;
	arraylist_new(&this->results, (void (*)(void*)) & free_docker_result);
	this->TryConnectLocal();
}

bool VRexContext::isConnected() {
	if (this->docker_ctx != NULL && (this->docker_ctx->url != NULL || this->docker_ctx->socket != NULL)) {
		return true;
	}
	return false;
}

docker_context* VRexContext::getDockerContext() {
	return this->docker_ctx;
}

docker_version* VRexContext::getDockerVersion() {
	return this->docker_version;
}

docker_info* VRexContext::getDockerInfo() {
	docker_info* info;
	docker_result* res;
	docker_system_info(this->docker_ctx, &res, &info);
	char* report = this->handleDockerResult(res);
	if (report != NULL && res->http_error_code == 200) {
		docker_log_debug(report);
	}
	if (res != NULL) {
		free_docker_result(&res);
	}
	return info;
}

vrex_err_t VRexContext::TryConnectLocal() {
	docker_result* res;

	// connect to docker
	d_err_t err = make_docker_context_default_local(&this->docker_ctx);
	if (err == E_SUCCESS) {
		docker_system_version(this->docker_ctx, &res, &this->docker_version);
		char* report = this->handleDockerResult(res);
		if (report != NULL && res->http_error_code == 200) {
			char* version_info = (char*)calloc(10240, sizeof(char));
			if (version_info != NULL) {
				sprintf(version_info, "Docker: %s [%s]",
					this->docker_ctx->socket == NULL ?
					this->docker_ctx->url : this->docker_ctx->socket, this->docker_version->os);
			}
		}
		free_docker_result(&res);
		return VREX_SUCCESS;
	}

	free_docker_result(&res);
	return VREX_E_UNKNOWN;
}

vrex_err_t VRexContext::TryConnectURL(const char* url) {
	docker_result* res;

	// connect to docker
	d_err_t err = make_docker_context_url(&this->docker_ctx, url);
	if (err == E_SUCCESS) {
		docker_system_version(this->docker_ctx, &res, &this->docker_version);
		char* report = this->handleDockerResult(res);
		if (report != NULL && res->http_error_code == 200) {
			char* version_info = (char*)calloc(10240, sizeof(char));
			if (version_info != NULL) {
				sprintf(version_info, "Docker: %s [%s]",
					this->docker_ctx->socket == NULL ?
					this->docker_ctx->url : this->docker_ctx->socket, this->docker_version->os);
			}
		}
		free_docker_result(&res);
		return VREX_SUCCESS;
	}

	free_docker_result(&res);
	return VREX_E_UNKNOWN;
}

char* VRexContext::handleDockerResult(docker_result* res) {
	return handle_error(res);
}