#pragma once

#include <wx/panel.h>
#include <wx/grid.h>

#include "VRexContext.h"
#include "VRexUtil.h"

class VolumesWindow : public wxPanel {
public:
	VolumesWindow(VRexContext* ctx, wxWindow* parent);

private:
	VRexContext* ctx;
	wxFlexGridSizer* volumesSizer;
	wxGrid* volumeListGrid;

	void HandleDockerConnect(wxCommandEvent& event);
	void HandleListVolumes(wxCommandEvent& event);
	void HandlePageRefresh(wxCommandEvent& event);
	void RefreshVolumes();
	void UpdateVolumes(docker_volume_list* volumes);
};