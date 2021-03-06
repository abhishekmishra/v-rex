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

#include "NetworksWindow.h"
#include "VRexContext.h"
#include "VRexUtil.h"

#define VREX_NETWORKS_TOOL_REFRESH		301

wxDEFINE_EVENT(LIST_NETWORKS_EVENT, wxCommandEvent);

// a thread class that will periodically send events to the GUI thread
class ListNetworksThread : public wxThread
{
	wxPanel* m_parent;
	VRexContext* ctx;

public:
	ListNetworksThread(wxPanel* parent, VRexContext* ctx)
	{
		m_parent = parent;
		this->ctx = ctx;
	}

	virtual ExitCode Entry();
};

wxThread::ExitCode ListNetworksThread::Entry()
{
	docker_network_list* networks;

	//Lookup networks
	docker_networks_list(this->ctx->getDockerContext(), &networks, NULL, NULL, NULL, NULL, NULL, NULL);

	// notify the main thread
	wxCommandEvent list_networks_event(LIST_NETWORKS_EVENT);
	list_networks_event.SetClientData(networks);
	m_parent->GetEventHandler()->AddPendingEvent(list_networks_event);

	return 0;
}

NetworksWindow::NetworksWindow(VRexContext* ctx, wxWindow* parent)
	: wxPanel(parent) {
	this->ctx = ctx;

	//SetBackgroundColour(*(new wxColour("red")));

	Bind(DOCKER_CONNECT_EVENT, &NetworksWindow::HandleDockerConnect, this, 0);
	Bind(PAGE_REFRESH_EVENT, &NetworksWindow::HandlePageRefresh, this, 0);
	Bind(LIST_NETWORKS_EVENT, &NetworksWindow::HandleListNetworks, this, 0);
	Bind(wxEVT_TOOL, &NetworksWindow::HandlePageRefresh, this, VREX_NETWORKS_TOOL_REFRESH);

	networksSizer = new wxFlexGridSizer(1);

	wxToolBar* toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_HORIZONTAL | wxTB_HORZ_TEXT);
	wxBitmap refresh = wxArtProvider::GetBitmap(wxART_REDO, wxART_TOOLBAR);
	wxBitmap show_run = wxArtProvider::GetBitmap(wxART_TICK_MARK, wxART_TOOLBAR);

	toolBar->AddTool(VREX_NETWORKS_TOOL_REFRESH, "Refresh", refresh);

	toolBar->Realize();

	networkListGrid = new wxGrid(this,
		-1,
		wxPoint(0, 0));
	networkListGrid->CreateGrid(1, 4);
	networkListGrid->SetRowSize(0, 20);
	networkListGrid->SetColSize(0, 120);
	networkListGrid->SetColLabelValue(0, "Name");
	networkListGrid->SetColLabelValue(1, "Driver");
	networkListGrid->SetColLabelValue(2, "Scope");
	networkListGrid->SetColLabelValue(3, "Attachable");
	networkListGrid->HideRowLabels();

	networksSizer->Add(toolBar, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 5);
	networksSizer->Add(networkListGrid, 0, wxALL | wxEXPAND, 5);

	if (this->ctx->isConnected()) {
		this->RefreshNetworks();
	}
	SetSizerAndFit(networksSizer);
	SetAutoLayout(true);
}

void NetworksWindow::HandleDockerConnect(wxCommandEvent& event) {
	if (this->ctx->isConnected()) {
		this->RefreshNetworks();
	}
}

void NetworksWindow::HandleListNetworks(wxCommandEvent& event) {
	docker_network_list* networks = (docker_network_list*)event.GetClientData();
	UpdateNetworks(networks);
	ctx->DockerCallUpdate("Networks list loaded.");
}

void NetworksWindow::RefreshNetworks() {
	if (this->ctx->isConnected()) {
		// create the thread
		ListNetworksThread* t = new ListNetworksThread(this, this->ctx);
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

void NetworksWindow::UpdateNetworks(docker_network_list* networks) {
	//Empty the grid by removing all current rows
	networkListGrid->DeleteRows(0, networkListGrid->GetNumberRows());

	//Add enough rows in the network grid
	networkListGrid->InsertRows(0, docker_network_list_length(networks));

	int col_num = 0, row_num = 0;
	int len_networks = docker_network_list_length(networks);
	for (int i = 0; i < len_networks; i++) {
		docker_network* net = (docker_network*)docker_network_list_get_idx(networks, i);
		col_num = 0;

		networkListGrid->SetCellValue(row_num, col_num, docker_network_name_get(net));
		col_num += 1;

		networkListGrid->SetCellValue(row_num, col_num, docker_network_driver_get(net));
		col_num += 1;

		networkListGrid->SetCellValue(row_num, col_num, docker_network_scope_get(net));
		col_num += 1;

		networkListGrid->SetCellValue(row_num, col_num, wxString::Format(wxT("%d"), docker_network_attachable_get(net)));
		col_num += 1;

		row_num += 1;
	}

	free_docker_network_list(networks);

	networkListGrid->AutoSize();
	Layout();
}

void NetworksWindow::HandlePageRefresh(wxCommandEvent& event) {
	if (this->ctx->isConnected()) {
		this->RefreshNetworks();
	}
	if (event.GetId() == VREX_NETWORKS_TOOL_REFRESH) {
		event.Skip();
	}
}

