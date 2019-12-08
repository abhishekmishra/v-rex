#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "DockerRequestThread.h"
#include "VRexContext.h"

wxDEFINE_EVENT(DOCKER_PING_EVENT, wxCommandEvent);

wxThread::ExitCode DockerRequestThread::Entry()
{
	void* clientData;

	// Override this to make a docker request and send some data back to the parent.
	const wxEventTypeTag<wxCommandEvent> eventType = DockerRequest(&clientData);

	// notify the main thread
	SendEventToParent(eventType, clientData);

	return 0;
}

const wxEventTypeTag<wxCommandEvent> DockerRequestThread::DockerRequest(void** clientData) {
	d_err_t err = docker_ping(this->ctx->getDockerContext());
	wxString* resp = new wxString("Ping complete.");
	*clientData = resp;
	return DOCKER_PING_EVENT;
}

void DockerRequestThread::SendEventToParent(const wxEventTypeTag<wxCommandEvent> eventType, void* clientData) {
	wxCommandEvent event_to_parent(eventType);
	event_to_parent.SetClientData(clientData);
	m_parent->GetEventHandler()->AddPendingEvent(event_to_parent);
}