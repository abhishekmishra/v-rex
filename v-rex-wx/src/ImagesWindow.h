#pragma once

#include <wx/panel.h>
#include <wx/grid.h>

#include "VRexContext.h"
#include "vrex_util.h"

class ImagesWindow : public wxPanel {
public:
	ImagesWindow(VRexContext* ctx, wxWindow* parent);

private:
	VRexContext* ctx;
	wxFlexGridSizer* imagesSizer;
	wxGrid* imageListGrid;

	void HandleDockerConnect(wxCommandEvent& event);
	void HandleListImages(wxCommandEvent& event);
	void HandlePageRefresh(wxCommandEvent& event);
	void RefreshImages();
	void UpdateImages(arraylist* images);
};