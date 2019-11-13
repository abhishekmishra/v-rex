#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/panel.h>
#include <wx/toolbar.h>
#include <wx/artprov.h>
#include <wx/grid.h>

#include <docker_all.h>
#include <arraylist.h>

#include "VolumesWindow.h"
#include "VRexContext.h"
#include "vrex_util.h"

#define VREX_VOLUMES_TOOL_REFRESH		301

wxDEFINE_EVENT(LIST_VOLUMES_EVENT, wxCommandEvent);

// a thread class that will periodically send events to the GUI thread
class ListVolumesThread : public wxThread
{
	wxPanel* m_parent;
	VRexContext* ctx;

public:
	ListVolumesThread(wxPanel* parent, VRexContext* ctx)
	{
		m_parent = parent;
		this->ctx = ctx;
	}

	virtual ExitCode Entry();
};

wxThread::ExitCode ListVolumesThread::Entry()
{
	arraylist* volumes;
	arraylist* warnings;
	docker_result* res;

	//Lookup volumes
	docker_volumes_list(this->ctx->getDockerContext(), &res, &volumes, &warnings, 0, NULL, NULL, NULL);
	char* report = this->ctx->handleDockerResult(res);

	if (report != NULL && res->http_error_code == 200) {
		docker_log_debug(report);
		free(report);
	}
	if (res != NULL) {
		free_docker_result(&res);
	}

	// notify the main thread
	wxCommandEvent list_volumes_event(LIST_VOLUMES_EVENT);
	list_volumes_event.SetClientData(volumes);
	m_parent->GetEventHandler()->AddPendingEvent(list_volumes_event);

	arraylist_free(warnings);
	return 0;
}

VolumesWindow::VolumesWindow(VRexContext* ctx, wxWindow* parent)
	: wxPanel(parent) {
	this->ctx = ctx;

	//SetBackgroundColour(*(new wxColour("red")));

	Bind(DOCKER_CONNECT_EVENT, &VolumesWindow::HandleDockerConnect, this, 0);
	Bind(PAGE_REFRESH_EVENT, &VolumesWindow::HandlePageRefresh, this, 0);
	Bind(LIST_VOLUMES_EVENT, &VolumesWindow::HandleListVolumes, this, 0);
	Bind(wxEVT_TOOL, &VolumesWindow::HandlePageRefresh, this, VREX_VOLUMES_TOOL_REFRESH);

	volumesSizer = new wxFlexGridSizer(1);

	wxToolBar* toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_HORIZONTAL | wxTB_HORZ_TEXT);
	wxBitmap refresh = wxArtProvider::GetBitmap(wxART_REDO, wxART_TOOLBAR);
	wxBitmap show_run = wxArtProvider::GetBitmap(wxART_TICK_MARK, wxART_TOOLBAR);

	toolBar->AddTool(VREX_VOLUMES_TOOL_REFRESH, "Refresh", refresh);

	toolBar->Realize();

	volumeListGrid = new wxGrid(this,
		-1,
		wxPoint(0, 0));
	volumeListGrid->CreateGrid(1, 4);
	volumeListGrid->SetRowSize(0, 20);
	volumeListGrid->SetColSize(0, 120);
	volumeListGrid->SetColLabelValue(0, "Name");
	volumeListGrid->SetColLabelValue(1, "Driver");
	volumeListGrid->SetColLabelValue(2, "Scope");
	volumeListGrid->SetColLabelValue(3, "Mount Point");
	volumeListGrid->HideRowLabels();

	volumesSizer->Add(toolBar, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 5);
	volumesSizer->Add(volumeListGrid, 0, wxALL | wxEXPAND, 5);

	if (this->ctx->isConnected()) {
		this->RefreshVolumes();
	}
	SetSizerAndFit(volumesSizer);
	SetAutoLayout(true);
}

void VolumesWindow::HandleDockerConnect(wxCommandEvent& event) {
	if (this->ctx->isConnected()) {
		this->RefreshVolumes();
	}
}

void VolumesWindow::HandleListVolumes(wxCommandEvent& event) {
	arraylist* volumes = (arraylist*)event.GetClientData();
	UpdateVolumes(volumes);
}

void VolumesWindow::RefreshVolumes() {
	if (this->ctx->isConnected()) {
		// create the thread
		ListVolumesThread* t = new ListVolumesThread(this, this->ctx);
		wxThreadError err = t->Create();

		if (err != wxTHREAD_NO_ERROR)
		{
			wxMessageBox(_("Couldn't create thread!"));
		}

		err = t->Run();

		if (err != wxTHREAD_NO_ERROR)
		{
			wxMessageBox(_("Couldn't run thread!"));
		}
	}
	else {
		//TODO: show error here - send to frame's status bar
	}
}

void VolumesWindow::UpdateVolumes(arraylist* volumes) {
	//Empty the grid by removing all current rows
	volumeListGrid->DeleteRows(0, volumeListGrid->GetNumberRows());

	//Add enough rows in the volume grid
	volumeListGrid->InsertRows(0, arraylist_length(volumes));

	int col_num = 0, row_num = 0;
	int len_volumes = arraylist_length(volumes);
	for (int i = 0; i < len_volumes; i++) {
		docker_volume* vol = (docker_volume*)arraylist_get(volumes, i);
		col_num = 0;

		volumeListGrid->SetCellValue(row_num, col_num, vol->name);
		col_num += 1;

		volumeListGrid->SetCellValue(row_num, col_num, vol->driver);
		col_num += 1;

		volumeListGrid->SetCellValue(row_num, col_num, vol->scope);
		col_num += 1;

		volumeListGrid->SetCellValue(row_num, col_num, vol->mountpoint);
		col_num += 1;

		row_num += 1;
	}

	arraylist_free(volumes);

	volumeListGrid->AutoSize();
	Layout();
}

void VolumesWindow::HandlePageRefresh(wxCommandEvent& event) {
	if (this->ctx->isConnected()) {
		this->RefreshVolumes();
	}
	if (event.GetId() == VREX_VOLUMES_TOOL_REFRESH) {
		event.Skip();
	}
}
