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

	docker_ping(this->ctx->getDockerContext(), &res);
	char* report = this->ctx->handleDockerResult(res);
	wxLogDebug("Docker ping done.\n");

	if (report != NULL && res->http_error_code == 200) {
		wxLogDebug(report);
	}
	if (res != NULL) {
		free_docker_result(&res);
	}

	// notify the main thread
	wxCommandEvent ping_event(DOCKER_PING_EVENT);
	ping_event.SetClientData(new wxString("Ping"));
	m_parent->GetEventHandler()->AddPendingEvent(ping_event);
	return 0;
}
