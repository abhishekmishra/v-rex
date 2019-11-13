#pragma once

#include <wx/panel.h>
#include <wx/grid.h>

#include "VRexContext.h"
#include "vrex_util.h"

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
	void UpdateVolumes(arraylist* volumes);
};