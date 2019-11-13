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

#include "ContainersWindow.h"
#include "VRexContext.h"
#include "vrex_util.h"

wxDEFINE_EVENT(LIST_CONTAINERS_EVENT, wxCommandEvent);

#define VREX_CONTAINERS_TOOL_RUNNING_ONLY	201
#define VREX_CONTAINERS_TOOL_REFRESH		202
#define VREX_CONTAINERS_TOOL_START			203
#define VREX_CONTAINERS_TOOL_STOP			204
#define VREX_CONTAINERS_TOOL_KILL			205
#define VREX_CONTAINERS_TOOL_RESTART		206
#define VREX_CONTAINERS_TOOL_PAUSE			207
#define VREX_CONTAINERS_TOOL_RESUME			208
#define VREX_CONTAINERS_TOOL_REMOVE			209
#define VREX_CONTAINERS_TOOL_ADD			210

// a thread class that will periodically send events to the GUI thread
class ListContainersThread : public wxThread
{
	ContainersWindow* m_parent;
	VRexContext* ctx;

public:
	ListContainersThread(ContainersWindow* parent, VRexContext* ctx)
	{
		m_parent = parent;
		this->ctx = ctx;
	}

	virtual ExitCode Entry();
};

wxThread::ExitCode ListContainersThread::Entry()
{
	int all = m_parent->isShowRunningEnabled() == true ? 0 : 1;
	long limit = 0;
	docker_containers_list* containers;
	docker_result* res;

	//Lookup containers
	docker_container_list(this->ctx->getDockerContext(), &res, &containers, all,
		limit, 1, NULL);
	char* report = this->ctx->handleDockerResult(res);
	docker_log_debug("Read %d containers.\n",
		docker_containers_list_length(containers));

	if (report != NULL && res->http_error_code == 200) {
		docker_log_debug(report);
	}
	if (res != NULL) {
		free_docker_result(&res);
	}

	// notify the main thread
	wxCommandEvent list_containers_event(LIST_CONTAINERS_EVENT);
	list_containers_event.SetClientData(containers);
	m_parent->GetEventHandler()->AddPendingEvent(list_containers_event);
	return 0;
}

ContainersWindow::ContainersWindow(VRexContext* ctx, wxWindow* parent)
	: wxPanel(parent) {
	this->ctx = ctx;

	//SetBackgroundColour(*(new wxColour("red")));

	containersSizer = new wxFlexGridSizer(1);

	showRunning = true;
	toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_HORIZONTAL | wxTB_HORZ_TEXT);
	wxBitmap refresh = wxArtProvider::GetBitmap(wxART_REDO, wxART_TOOLBAR);
	wxBitmap show_run = wxArtProvider::GetBitmap(wxART_TICK_MARK, wxART_TOOLBAR);
	wxBitmap other = wxArtProvider::GetBitmap(wxART_CDROM, wxART_TOOLBAR);

	toolBar->AddCheckTool(VREX_CONTAINERS_TOOL_RUNNING_ONLY, "Show Running", show_run);
	toolBar->ToggleTool(VREX_CONTAINERS_TOOL_RUNNING_ONLY, true);
	toolBar->AddTool(VREX_CONTAINERS_TOOL_REFRESH, "Refresh", refresh);

	wxSize toolBarBtnSize = wxSize(60, toolBar->GetClientSize().GetHeight());
	wxButton* startBtn = new wxButton(toolBar, VREX_CONTAINERS_TOOL_START, "Start", wxDefaultPosition, toolBarBtnSize, wxBORDER_NONE);
	toolBar->AddControl(startBtn);
	wxButton* stopBtn = new wxButton(toolBar, VREX_CONTAINERS_TOOL_STOP, "Stop", wxDefaultPosition, toolBarBtnSize, wxBORDER_NONE);
	toolBar->AddControl(stopBtn);
	wxButton* killBtn = new wxButton(toolBar, VREX_CONTAINERS_TOOL_KILL, "Kill", wxDefaultPosition, toolBarBtnSize, wxBORDER_NONE);
	toolBar->AddControl(killBtn);
	wxButton* restartBtn = new wxButton(toolBar, VREX_CONTAINERS_TOOL_RESTART, "Restart", wxDefaultPosition, toolBarBtnSize, wxBORDER_NONE);
	toolBar->AddControl(restartBtn);
	wxButton* pauseBtn = new wxButton(toolBar, VREX_CONTAINERS_TOOL_PAUSE, "Pause", wxDefaultPosition, toolBarBtnSize, wxBORDER_NONE);
	toolBar->AddControl(pauseBtn);
	wxButton* resumeBtn = new wxButton(toolBar, VREX_CONTAINERS_TOOL_RESUME, "Resume", wxDefaultPosition, toolBarBtnSize, wxBORDER_NONE);
	toolBar->AddControl(resumeBtn);
	wxButton* removeBtn = new wxButton(toolBar, VREX_CONTAINERS_TOOL_REMOVE, "Remove", wxDefaultPosition, toolBarBtnSize, wxBORDER_NONE);
	toolBar->AddControl(removeBtn);
	wxButton* addBtn = new wxButton(toolBar, VREX_CONTAINERS_TOOL_ADD, "Add", wxDefaultPosition, toolBarBtnSize, wxBORDER_NONE);
	toolBar->AddControl(addBtn);
	toolBar->Realize();

	containerListGrid = new wxGrid(this,
		-1,
		wxPoint(0, 0));
	containerListGrid->CreateGrid(1, 5);
	containerListGrid->SetRowSize(0, 20);
	containerListGrid->SetColSize(0, 120);
	containerListGrid->SetColLabelValue(0, "Name");
	containerListGrid->SetColLabelValue(1, "Image");
	containerListGrid->SetColLabelValue(2, "Command");
	containerListGrid->SetColLabelValue(3, "Ports");
	containerListGrid->SetColLabelValue(4, "State");
	containerListGrid->HideRowLabels();

	containerListGrid->SetGridLineColour(wxTheColourDatabase->Find(wxT("VREX_WHITESMOKE")));
	containerListGrid->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	containerListGrid->EnableEditing(false);

	containersSizer->Add(toolBar, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 5);
	containersSizer->Add(containerListGrid, 0, wxALL | wxEXPAND, 5);

	Bind(DOCKER_CONNECT_EVENT, &ContainersWindow::HandleDockerConnect, this, 0);
	Bind(PAGE_REFRESH_EVENT, &ContainersWindow::HandlePageRefresh, this, 0);
	Bind(LIST_CONTAINERS_EVENT, &ContainersWindow::HandleListContainers, this, 0);
	Bind(wxEVT_TOOL, &ContainersWindow::HandlePageRefresh, this, VREX_CONTAINERS_TOOL_REFRESH);
	Bind(wxEVT_TOOL, &ContainersWindow::HandleShowRunning, this, VREX_CONTAINERS_TOOL_RUNNING_ONLY);
	containerListGrid->Bind(wxEVT_GRID_SELECT_CELL, &ContainersWindow::HandleCellSelection, this);
	containerListGrid->Bind(wxEVT_GRID_CELL_LEFT_CLICK, &ContainersWindow::HandleCellSelection, this);


	if (this->ctx->isConnected()) {
		this->RefreshContainers();
	}
	SetSizerAndFit(containersSizer);
	SetAutoLayout(true);
}

bool ContainersWindow::isShowRunningEnabled() {
	return showRunning;
}

void ContainersWindow::HandleShowRunning(wxCommandEvent& event) {
	if (event.GetId() == VREX_CONTAINERS_TOOL_RUNNING_ONLY) {
		showRunning = toolBar->GetToolState(VREX_CONTAINERS_TOOL_RUNNING_ONLY);
		RefreshContainers();
	}
	event.Skip();
}

void ContainersWindow::HandleDockerConnect(wxCommandEvent& event) {
	if (this->ctx->isConnected()) {
		this->RefreshContainers();
	}
	if (event.GetId() == VREX_CONTAINERS_TOOL_REFRESH) {
		event.Skip();
	}
}

void ContainersWindow::HandleListContainers(wxCommandEvent& event) {
	docker_containers_list* containers = (docker_containers_list*)event.GetClientData();
	UpdateContainers(containers);
}

void ContainersWindow::RefreshContainers() {
	if (this->ctx->isConnected()) {
		// create the thread
		ListContainersThread* t = new ListContainersThread(this, this->ctx);
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

void ContainersWindow::UpdateContainers(docker_containers_list* containers) {
	//Empty the grid by removing all current rows
	containerListGrid->DeleteRows(0, containerListGrid->GetNumberRows());

	//Add enough rows in the container grid
	containerListGrid->InsertRows(0, docker_containers_list_length(containers));

	int col_num = 0;
	int row_num = 0;
	char* first_id;
	for (int i = 0; i < docker_containers_list_length(containers); i++) {
		col_num = 0;
		docker_container_list_item* item = docker_containers_list_get_idx(
			containers, i);
		containerListGrid->SetCellValue(row_num, col_num, (char*)arraylist_get(item->names, 0));
		col_num += 1;

		if (i == 0) {
			first_id = (item->id);
		}

		containerListGrid->SetCellValue(row_num, col_num, item->image);
		col_num += 1;

		containerListGrid->SetCellValue(row_num, col_num, item->command);
		col_num += 1;

		//TODO: list all port pairs instead of only the first port
		//TODO: check if intermediate strings can be freed immediately
		if (arraylist_length(item->ports) > 0) {
			docker_container_ports* first_port = (docker_container_ports*)
				arraylist_get(item->ports, 0);
			if (first_port
				&& first_port->public_port > 0
				&& first_port->private_port
							> 0) {
				char* ports_str = (char*)calloc(128, sizeof(char));
				if (ports_str != NULL) {
					sprintf(ports_str, "%ld:%ld",
						first_port->public_port,
						first_port->private_port);

					containerListGrid->SetCellValue(row_num, col_num, ports_str);
				}
			}
		}
		col_num += 1;

		//TODO: size should be based on the size of state and status
		char* status = (char*)calloc(1024, sizeof(char));
		if (status != NULL) {
			strcpy(status, item->state);
			strcat(status, ":");
			strcat(status, item->status);

			containerListGrid->SetCellValue(row_num, col_num, status);
		}

		row_num += 1;
	}

	arraylist_free(containers);

	containerListGrid->AutoSize();
	Layout();
}

void ContainersWindow::HandlePageRefresh(wxCommandEvent& event) {
	if (this->ctx->isConnected()) {
		this->RefreshContainers();
	}
}

void ContainersWindow::HandleCellSelection(wxGridEvent& event) {
	int row = event.GetRow();
	wxString containerName = containerListGrid->GetCellValue(row, 0);
	//wxMessageBox(wxString::Format(wxT("Container Name is %S"), containerName));
	if (event.GetId() == wxEVT_GRID_SELECT_CELL) {
		containerListGrid->SelectRow(row);
	}
	event.Skip();
}