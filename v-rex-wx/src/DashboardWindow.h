#pragma once

#include <wx/wx.h>

#include "VRexContext.h"

class DashboardWindow: public wxPanel {
public:
    DashboardWindow(VRexContext* ctx, wxWindow* parent);

private:
	VRexContext* ctx;

	void handlerDockerConnect(wxCommandEvent& event);
};