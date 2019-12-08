#pragma once
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "VRexContext.h"

// a thread class that will send events to the GUI thread
// after a docker request
class DockerRequestThread : public wxThread
{
	wxWindow* m_parent;

protected:
	VRexContext* ctx;

public:
	DockerRequestThread(wxWindow* parent, VRexContext* ctx)
	{
		m_parent = parent;
		this->ctx = ctx;
	}
	virtual const wxEventTypeTag<wxCommandEvent> DockerRequest(void** clientData);
	virtual ExitCode Entry();

private:
	void SendEventToParent(const wxEventTypeTag<wxCommandEvent> eventType, void* clientData);
};
