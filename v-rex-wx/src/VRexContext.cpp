#include "VRexContext.h"
#include <docker_all.h>
#include <docker_log.h>
#include <VRexUtil.h>

void handleDockerResult(docker_context* docker_ctx, docker_result* res) {
	VRexContext* vrex_ctx = (VRexContext*)docker_context_client_args_get(docker_ctx);
	wxCommandEvent event_to_parent(DOCKER_INTERACTION_RESULT_EVENT);
	event_to_parent.SetClientData(docker_result_clone(res));
	vrex_ctx->getInteractionsWindow()->GetEventHandler()->AddPendingEvent(event_to_parent);
}

VRexContext::VRexContext() {
	interactionsW = NULL;
	toplevel = NULL;
	version = NULL;
	connected = false;
	docker_ctx = NULL;
	arraylist_new(&this->results, (void (*)(void*)) & free_docker_result);
}

docker_context* VRexContext::getDockerContext() {
	return this->docker_ctx;
}

docker_version* VRexContext::getDockerVersion() {
	return this->version;
}

docker_info* VRexContext::getDockerInfo() {
	docker_info* info;
	d_err_t err = docker_system_info(this->docker_ctx, &info);
	return info;
}

vrex_err_t VRexContext::TryConnectLocal() {
	this->connected = false;

	// connect to docker
	d_err_t err = make_docker_context_default_local(&this->docker_ctx);
	if (err == E_SUCCESS) {
		docker_context_client_args_set(this->docker_ctx, this);
		docker_context_result_handler_set(this->docker_ctx, &handleDockerResult);
		d_err_t err = docker_system_version(this->docker_ctx, &this->version);
		if (err == E_SUCCESS) {
			char* version_info = (char*)calloc(10240, sizeof(char));
			if (version_info != NULL) {
				sprintf(version_info, "Docker: %s [%s]",
					this->docker_ctx->url, docker_version_os_get(this->version));
			}
			this->connected = true;
			return VREX_SUCCESS;
		}
	}

	return VREX_E_UNKNOWN;
}

vrex_err_t VRexContext::TryConnectURL(const char* url) {
	this->connected = false;

	// connect to docker
	d_err_t err = make_docker_context_url(&this->docker_ctx, url);
	if (err == E_SUCCESS) {
		docker_context_client_args_set(this->docker_ctx, this);
		docker_context_result_handler_set(this->docker_ctx, &handleDockerResult);
		d_err_t err = docker_system_version(this->docker_ctx, &this->version);
		if (err == E_SUCCESS) {
			char* version_info = (char*)calloc(10240, sizeof(char));
			if (version_info != NULL) {
				sprintf(version_info, "Docker: %s [%s]",
					this->docker_ctx->url, docker_version_os_get(this->version));
			}
			this->connected = true;
			return VREX_SUCCESS;
		}
	}

	return VREX_E_UNKNOWN;
}

void VRexContext::HandleDockerResult(docker_result* res) {
	wxCommandEvent event_to_parent(DOCKER_INTERACTION_RESULT_EVENT);
	event_to_parent.SetClientData(res);
	interactionsW->GetEventHandler()->AddPendingEvent(event_to_parent);
	//return handle_error(res);
}

bool VRexContext::isConnected() {
	return this->connected;
}

void VRexContext::RegisterTopLevelWindow(wxWindow* toplevel) {
	this->toplevel = toplevel;
}

void VRexContext::RegisterInteractionsWindow(wxWindow* interactionsW) {
	this->interactionsW = interactionsW;
}

wxWindow* VRexContext::getInteractionsWindow() {
	return interactionsW;
}