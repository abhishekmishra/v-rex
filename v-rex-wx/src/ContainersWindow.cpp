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

#include "ContainerDialog.h"
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

#define VREX_CONTAINERS_DEFAULT_COLUMN_WIDTH 300

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

	wxSize toolBarBtnSize = wxSize(60, 30);
	startBtn = new wxButton(toolBar, VREX_CONTAINERS_TOOL_START, "Start", wxDefaultPosition, toolBarBtnSize, wxBORDER_NONE);
	startBtn->Disable();
	startBtn->SetBackgroundColour(wxTheColourDatabase->Find(VREX_LIGHTGREEN));
	toolBar->AddControl(startBtn);

	stopBtn = new wxButton(toolBar, VREX_CONTAINERS_TOOL_STOP, "Stop", wxDefaultPosition, toolBarBtnSize, wxBORDER_NONE);
	toolBar->AddControl(stopBtn);
	stopBtn->SetBackgroundColour(wxTheColourDatabase->Find(VREX_LIGHTSALMON));
	stopBtn->Disable();

	killBtn = new wxButton(toolBar, VREX_CONTAINERS_TOOL_KILL, "Kill", wxDefaultPosition, toolBarBtnSize, wxBORDER_NONE);
	toolBar->AddControl(killBtn);
	killBtn->SetBackgroundColour(wxTheColourDatabase->Find(VREX_LIGHTSALMON));
	killBtn->Disable();

	restartBtn = new wxButton(toolBar, VREX_CONTAINERS_TOOL_RESTART, "Restart", wxDefaultPosition, toolBarBtnSize, wxBORDER_NONE);
	toolBar->AddControl(restartBtn);
	restartBtn->SetBackgroundColour(wxTheColourDatabase->Find(VREX_LIGHTSALMON));
	restartBtn->Disable();

	pauseBtn = new wxButton(toolBar, VREX_CONTAINERS_TOOL_PAUSE, "Pause", wxDefaultPosition, toolBarBtnSize, wxBORDER_NONE);
	toolBar->AddControl(pauseBtn);
	pauseBtn->Disable();

	resumeBtn = new wxButton(toolBar, VREX_CONTAINERS_TOOL_RESUME, "Resume", wxDefaultPosition, toolBarBtnSize, wxBORDER_NONE);
	toolBar->AddControl(resumeBtn);
	resumeBtn->Disable();

	removeBtn = new wxButton(toolBar, VREX_CONTAINERS_TOOL_REMOVE, "Remove", wxDefaultPosition, toolBarBtnSize, wxBORDER_NONE);
	toolBar->AddControl(removeBtn);
	removeBtn->SetBackgroundColour(wxTheColourDatabase->Find(VREX_LIGHTSALMON));
	removeBtn->Disable();

	addBtn = new wxButton(toolBar, VREX_CONTAINERS_TOOL_ADD, "Add", wxDefaultPosition, toolBarBtnSize, wxBORDER_NONE);
	toolBar->AddControl(addBtn);
	addBtn->Enable();

	toolBar->Realize();

	containerListGrid = new wxGrid(this,
		-1,
		wxDefaultPosition,
		wxDefaultSize);
	containerListGrid->CreateGrid(1, 6);
	containerListGrid->SetDefaultCellOverflow(false);
	containerListGrid->SetRowSize(0, 20);
	int grid_col_count = 0;
	containerListGrid->SetColSize(grid_col_count++, VREX_CONTAINERS_DEFAULT_COLUMN_WIDTH);
	containerListGrid->SetColSize(grid_col_count++, VREX_CONTAINERS_DEFAULT_COLUMN_WIDTH);
	containerListGrid->SetColSize(grid_col_count++, VREX_CONTAINERS_DEFAULT_COLUMN_WIDTH);
	containerListGrid->SetColSize(grid_col_count++, VREX_CONTAINERS_DEFAULT_COLUMN_WIDTH);
	containerListGrid->SetColSize(grid_col_count++, VREX_CONTAINERS_DEFAULT_COLUMN_WIDTH);
	containerListGrid->SetColSize(grid_col_count++, VREX_CONTAINERS_DEFAULT_COLUMN_WIDTH);

	grid_col_count = 0;
	containerListGrid->SetColLabelValue(grid_col_count++, "State");
	containerListGrid->SetColLabelValue(grid_col_count++, "Name");
	containerListGrid->SetColLabelValue(grid_col_count++, "Image");
	containerListGrid->SetColLabelValue(grid_col_count++, "Command");
	containerListGrid->SetColLabelValue(grid_col_count++, "Ports");
	containerListGrid->SetColLabelValue(grid_col_count++, "Status");
	containerListGrid->HideRowLabels();

	containerListGrid->SetGridLineColour(VREX_WHITESMOKE);
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
	startBtn->Bind(wxEVT_BUTTON, &ContainersWindow::HandleContainerStart, this);
	stopBtn->Bind(wxEVT_BUTTON, &ContainersWindow::HandleContainerStop, this);
	killBtn->Bind(wxEVT_BUTTON, &ContainersWindow::HandleContainerKill, this);
	restartBtn->Bind(wxEVT_BUTTON, &ContainersWindow::HandleContainerRestart, this);
	pauseBtn->Bind(wxEVT_BUTTON, &ContainersWindow::HandleContainerPause, this);
	resumeBtn->Bind(wxEVT_BUTTON, &ContainersWindow::HandleContainerResume, this);
	removeBtn->Bind(wxEVT_BUTTON, &ContainersWindow::HandleContainerRemove, this);
	addBtn->Bind(wxEVT_BUTTON, &ContainersWindow::HandleContainerAdd, this);


	if (this->ctx->isConnected()) {
		this->RefreshContainers();
	}
	SetSizer(containersSizer);
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

		containerListGrid->SetCellValue(row_num, col_num, item->state);
		if (strcmp(item->state, "running") == 0) {
			containerListGrid->SetCellBackgroundColour(row_num, col_num, wxTheColourDatabase->Find(VREX_LIGHTGREEN));
		}
		if (strcmp(item->state, "created") == 0) {
			containerListGrid->SetCellBackgroundColour(row_num, col_num, wxTheColourDatabase->Find(VREX_ALICEBLUE));
		}
		if (strcmp(item->state, "exited") == 0) {
			containerListGrid->SetCellBackgroundColour(row_num, col_num, wxTheColourDatabase->Find(VREX_PAPAYAWHIP));
		}
		if (strcmp(item->state, "paused") == 0) {
			containerListGrid->SetCellBackgroundColour(row_num, col_num, wxTheColourDatabase->Find(VREX_BEIGE));
		}
		col_num += 1;

		char* name = (char*)arraylist_get(item->names, 0);
		containerListGrid->SetCellValue(row_num, col_num, name + 1);
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

	//containerListGrid->AutoSize();
	containerListGrid->AutoSizeColumn(0);
	containerListGrid->AutoSizeColumn(1);
	containerListGrid->AutoSizeColumn(4);
	containerListGrid->AutoSizeColumn(5);
	Layout();
}

void ContainersWindow::HandlePageRefresh(wxCommandEvent& event) {
	if (this->ctx->isConnected()) {
		this->RefreshContainers();
	}
}

void ContainersWindow::HandleCellSelection(wxGridEvent& event) {
	int row = event.GetRow();
	wxString state = containerListGrid->GetCellValue(row, 0);
	wxString containerName = containerListGrid->GetCellValue(row, 1);

	if (strcmp(state, "running") == 0) {
		startBtn->Disable();
		stopBtn->Enable();
		killBtn->Enable();
		restartBtn->Enable();
		pauseBtn->Enable();
		resumeBtn->Enable();
		removeBtn->Enable();
	}
	else if (strcmp(state, "exited") == 0) {
		startBtn->Enable();
		stopBtn->Disable();
		killBtn->Disable();
		restartBtn->Disable();
		pauseBtn->Disable();
		resumeBtn->Disable();
		removeBtn->Enable();
	}
	else if (strcmp(state, "created") == 0) {
		startBtn->Enable();
		stopBtn->Disable();
		killBtn->Disable();
		restartBtn->Disable();
		pauseBtn->Disable();
		resumeBtn->Disable();
		removeBtn->Enable();
	}
	else if (strcmp(state, "paused") == 0) {
		startBtn->Disable();
		stopBtn->Enable();
		killBtn->Enable();
		restartBtn->Disable();
		pauseBtn->Disable();
		resumeBtn->Enable();
		removeBtn->Enable();
	}
	else {
		startBtn->Disable();
		stopBtn->Disable();
		killBtn->Disable();
		restartBtn->Disable();
		pauseBtn->Disable();
		resumeBtn->Disable();
		removeBtn->Disable();
	}
	//wxMessageBox(wxString::Format(wxT("Container Name is %S"), containerName));
	if (event.GetId() == wxEVT_GRID_SELECT_CELL) {
		containerListGrid->SelectRow(row);
	}
	event.Skip();
}

d_err_t ContainersWindow::RunContainerCommand(const char* container_name_or_id, int command) {
	d_err_t error = E_UNKNOWN_ERROR;
	if (container_name_or_id != NULL) {
		char* ctr_name_id = str_clone(container_name_or_id);
		if (ctr_name_id != NULL) {
			docker_result* res;
			switch (command) {
			case VREX_CONTAINERS_TOOL_START:
				error = docker_start_container(ctx->getDockerContext(), &res, ctr_name_id, NULL);
				break;
			case VREX_CONTAINERS_TOOL_STOP:
				error = docker_stop_container(ctx->getDockerContext(), &res, ctr_name_id, NULL);
				break;
			case VREX_CONTAINERS_TOOL_KILL:
				error = docker_kill_container(ctx->getDockerContext(), &res, ctr_name_id, NULL);
				break;
			case VREX_CONTAINERS_TOOL_RESTART:
				error = docker_restart_container(ctx->getDockerContext(), &res, ctr_name_id, NULL);
				break;
			case VREX_CONTAINERS_TOOL_PAUSE:
				error = docker_pause_container(ctx->getDockerContext(), &res, ctr_name_id);
				break;
			case VREX_CONTAINERS_TOOL_RESUME:
				error = docker_unpause_container(ctx->getDockerContext(), &res, ctr_name_id);
				break;
			case VREX_CONTAINERS_TOOL_REMOVE:
				//TODO remove container might require a dialog
				error = docker_remove_container(ctx->getDockerContext(), &res, ctr_name_id, 0, 0, 0);
				break;
			default:
				break;
			}
		}
	}
	return error;
}

char* getSelectedRowContainerName(wxGrid* containerGrid) {
	wxArrayInt selections = containerGrid->GetSelectedRows();
	if (selections.GetCount() > 0) {
		int row = selections[0];
		wxString containerName = containerGrid->GetCellValue(row, 1);
		return str_clone(containerName.ToUTF8().data());
	}
	return NULL;
}

d_err_t ContainersWindow::HandleContainerBtnEvent(wxCommandEvent& event, int eventCommand, wxString success, wxString failure) {
	char* containerName = getSelectedRowContainerName(containerListGrid);
	if (containerName != NULL) {
		d_err_t error = RunContainerCommand(containerName, eventCommand);
		if (error == E_SUCCESS) {
			RefreshContainers();
			wxMessageBox(wxString::Format(success, containerName));
		}
		else {
			wxMessageBox(wxString::Format(failure, containerName));
		}
		free(containerName);
	}
	return E_UNKNOWN_ERROR;
}

void ContainersWindow::HandleContainerStart(wxCommandEvent& event) {
	HandleContainerBtnEvent(
		event, 
		VREX_CONTAINERS_TOOL_START,
		wxT("%s is started."),
		wxT("Error starting %s."));
}

void ContainersWindow::HandleContainerStop(wxCommandEvent& event) {
	HandleContainerBtnEvent(
		event,
		VREX_CONTAINERS_TOOL_STOP,
		wxT("%s is stopped."),
		wxT("Error stopping %s."));
}

void ContainersWindow::HandleContainerKill(wxCommandEvent& event) {
	HandleContainerBtnEvent(
		event,
		VREX_CONTAINERS_TOOL_KILL,
		wxT("%s is killed."),
		wxT("Error killing %s."));
}

void ContainersWindow::HandleContainerRestart(wxCommandEvent& event) {
	HandleContainerBtnEvent(
		event,
		VREX_CONTAINERS_TOOL_RESTART,
		wxT("%s is restarted."),
		wxT("Error restarting %s."));
}

void ContainersWindow::HandleContainerPause(wxCommandEvent& event) {
	HandleContainerBtnEvent(
		event,
		VREX_CONTAINERS_TOOL_PAUSE,
		wxT("%s is paused."),
		wxT("Error pausing %s."));
}

void ContainersWindow::HandleContainerResume(wxCommandEvent& event) {
	HandleContainerBtnEvent(
		event,
		VREX_CONTAINERS_TOOL_RESUME,
		wxT("%s has resumed."),
		wxT("Error resuming %s."));
}

void ContainersWindow::HandleContainerRemove(wxCommandEvent& event) {
	HandleContainerBtnEvent(
		event,
		VREX_CONTAINERS_TOOL_REMOVE,
		wxT("%s is removed."),
		wxT("Error removing %s."));
}

void ContainersWindow::HandleContainerAdd(wxCommandEvent& event) {
	ContainerDialog("Add Container").ShowModal();
}