#pragma once

#include <wx/panel.h>
#include <wx/grid.h>

#include "VRexContext.h"
#include "vrex_util.h"

class ContainersWindow: public wxPanel {
public:
    ContainersWindow(VRexContext* ctx, wxWindow* parent);

private:
	VRexContext* ctx;
	wxFlexGridSizer* containersSizer;
	wxGrid* containerListGrid;

	void handlerDockerConnect(wxCommandEvent& event);
	void RefreshContainers();
};