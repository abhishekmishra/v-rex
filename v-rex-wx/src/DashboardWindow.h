#pragma once

#include <wx/wx.h>
#include <wx/commandlinkbutton.h>

#include "VRexContext.h"

class DashboardWindow: public wxPanel {
public:
    DashboardWindow(VRexContext* ctx, wxWindow* parent);

private:
	VRexContext* ctx;
	wxFlexGridSizer* dashSizer;
	wxCommandLinkButton* containersBtn;
	wxCommandLinkButton* imagesBtn;
	wxCommandLinkButton* volumesBtn;
	wxCommandLinkButton* networksBtn;

	void HandleDockerConnect(wxCommandEvent& event);
};