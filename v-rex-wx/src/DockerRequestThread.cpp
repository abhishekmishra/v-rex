#pragma once
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "DockerRequestThread.h"
#include "VRexContext.h"

wxDEFINE_EVENT(DOCKER_PING_EVENT, wxCommandEvent);

wxThread::ExitCode DockerRequestThread::Entry()
{
	docker_result* res;
	void* clientData = DockerRequest(&res);

	// handler the requests of the docker request
	HandleDockerResult(res);

	// notify the main thread
	SendEventToParent(DOCKER_PING_EVENT, clientData);

	return 0;
}

void* DockerRequestThread::DockerRequest(docker_result** res) {
	docker_ping(this->ctx->getDockerContext(), res);
	return new wxString("Ping complete.");
}

void DockerRequestThread::SendEventToParent(const wxEventTypeTag<wxCommandEvent> eventType, void* clientData) {
	wxCommandEvent event_to_parent(eventType);
	event_to_parent.SetClientData(clientData);
	m_parent->GetEventHandler()->AddPendingEvent(event_to_parent);

}

void DockerRequestThread::HandleDockerResult(docker_result* res) {
	char* report = ctx->handleDockerResult(res);
	wxLogDebug("Docker request complete.\n");

	if (report != NULL && res->http_error_code == 200) {
		wxLogDebug(report);
	}
	if (res != NULL) {
		free_docker_result(&res);
	}
	free(report);
}
