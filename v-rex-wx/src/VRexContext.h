#pragma once
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <docker_connection_util.h>
#include <docker_system.h>

#include "VRexUtil.h"

class VRexContext {
public:
	VRexContext();
	docker_context* getDockerContext();
	docker_version* getDockerVersion();
	docker_info* getDockerInfo();
	void HandleDockerResult(docker_result* res);
	void DockerCallUpdate(wxString statusMessage);
	vrex_err_t TryConnectLocal();
	vrex_err_t TryConnectURL(const char* url);
	bool isConnected();
	void RegisterTopLevelWindow(wxWindow* toplevel);
	void RegisterInteractionsWindow(wxWindow* interactionsW);
	wxWindow* getInteractionsWindow();

private:
	docker_context* docker_ctx;
	docker_version* version;
	arraylist* results;
	bool connected;
	wxWindow* toplevel;
	wxWindow* interactionsW;
};

void handleDockerResult(docker_context* docker_ctx, docker_result* res);
