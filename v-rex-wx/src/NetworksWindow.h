#pragma once

#include <wx/panel.h>
#include <wx/grid.h>

#include "VRexContext.h"
#include "vrex_util.h"

class NetworksWindow : public wxPanel {
public:
	NetworksWindow(VRexContext* ctx, wxWindow* parent);

private:
	VRexContext* ctx;
	wxFlexGridSizer* networksSizer;
	wxGrid* networkListGrid;

	void HandleDockerConnect(wxCommandEvent& event);
	void HandleListNetworks(wxCommandEvent& event);
	void HandlePageRefresh(wxCommandEvent& event);
	void RefreshNetworks();
	void UpdateNetworks(docker_network_list* networks);
};