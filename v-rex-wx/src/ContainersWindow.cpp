#include <wx/panel.h>
#include <wx/toolbar.h>
#include <wx/artprov.h>
#include <wx/grid.h>

#include "ContainersWindow.h"
#include "VRexContext.h"
#include "vrex_util.h"

ContainersWindow::ContainersWindow(VRexContext* ctx, wxWindow* parent)
	: wxPanel(parent) {
	this->ctx = ctx;

	Bind(DOCKER_CONNECT_EVENT, &ContainersWindow::handlerDockerConnect, this, 0);

	containersSizer = new wxFlexGridSizer(1);

	wxToolBar* toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_HORIZONTAL);
	wxBitmap open = wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_TOOLBAR);
	wxBitmap exit = wxArtProvider::GetBitmap(wxART_QUIT, wxART_TOOLBAR);
	wxBitmap save = wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_TOOLBAR);
	wxBitmap b_new = wxArtProvider::GetBitmap(wxART_NEW, wxART_TOOLBAR);

	toolBar->AddTool(3, "New file", b_new);
	toolBar->AddTool(4, "Open file", open);
	toolBar->AddTool(5, "Save file", save);
	toolBar->AddTool(wxID_EXIT, "Exit", exit);
	toolBar->Realize();

	// Create a wxGrid object
	wxGrid* grid = new wxGrid(this,
		-1,
		wxPoint(0, 0),
		wxSize(800, 600));
	// Then we call CreateGrid to set the dimensions of the grid
	// (100 rows and 10 columns in this example)
	grid->CreateGrid(1, 4);
	// We can set the sizes of individual rows and columns
	// in pixels
	grid->SetRowSize(0, 20);
	grid->SetColSize(0, 120);
	grid->SetColLabelValue(0, "Name");
	grid->SetColLabelValue(1, "Image");
	grid->SetColLabelValue(2, "Command");
	grid->SetColLabelValue(3, "State");
	grid->HideRowLabels();

	containersSizer->Add(toolBar, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	containersSizer->Add(grid, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	SetSizerAndFit(containersSizer);
}

void ContainersWindow::handlerDockerConnect(wxCommandEvent& event) {
}