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
	void* clientData;

	// Override this to make a docker request and send some data back to the parent.
	const wxEventTypeTag<wxCommandEvent> eventType = DockerRequest(&res, &clientData);

	// handler the requests of the docker request
	HandleDockerResult(res);

	// notify the main thread
	SendEventToParent(eventType, clientData);

	return 0;
}

const wxEventTypeTag<wxCommandEvent> DockerRequestThread::DockerRequest(docker_result** res, void** clientData) {
	docker_ping(this->ctx->getDockerContext(), res);
	wxString* resp = new wxString("Ping complete.");
	*clientData = resp;
	return DOCKER_PING_EVENT;
}

void DockerRequestThread::SendEventToParent(const wxEventTypeTag<wxCommandEvent> eventType, void* clientData) {
	wxCommandEvent event_to_parent(eventType);
	event_to_parent.SetClientData(clientData);
	m_parent->GetEventHandler()->AddPendingEvent(event_to_parent);
}

void DockerRequestThread::HandleDockerResult(docker_result* res) {
	char* report = ctx->HandleDockerResult(res);
	wxLogDebug("Docker request complete.\n");

	if (report != NULL && res->http_error_code == 200) {
		wxLogDebug(report);
	}
	//if (res != NULL) {
	//	free_docker_result(&res);
	//}
	free(report);
}
