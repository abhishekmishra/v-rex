#pragma once

#include <wx/panel.h>

#include "VRexContext.h"
#include "vrex_util.h"

class ContainersWindow: public wxPanel {
public:
    ContainersWindow(VRexContext* ctx, wxWindow* parent);

private:
	VRexContext* ctx;
	wxFlexGridSizer* containersSizer;

	void handlerDockerConnect(wxCommandEvent& event);
};