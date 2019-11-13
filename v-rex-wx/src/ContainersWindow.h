#pragma once

#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/toolbar.h>

#include "VRexContext.h"
#include "vrex_util.h"

class ContainersWindow: public wxPanel {
public:
    ContainersWindow(VRexContext* ctx, wxWindow* parent);
	bool isShowRunningEnabled();

private:
	VRexContext* ctx;
	wxFlexGridSizer* containersSizer;
	wxToolBar* toolBar;
	wxGrid* containerListGrid;
	bool showRunning;

	void HandleDockerConnect(wxCommandEvent& event);
	void HandleListContainers(wxCommandEvent& event);
	void HandlePageRefresh(wxCommandEvent& event);
	void HandleShowRunning(wxCommandEvent& event);
	void RefreshContainers();
	void UpdateContainers(docker_containers_list* containers);
};