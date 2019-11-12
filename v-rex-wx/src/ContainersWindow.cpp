#include <wx/panel.h>
#include <wx/toolbar.h>
#include <wx/artprov.h>
#include <wx/grid.h>

#include <docker_all.h>
#include <arraylist.h>

#include "ContainersWindow.h"
#include "VRexContext.h"
#include "vrex_util.h"

ContainersWindow::ContainersWindow(VRexContext* ctx, wxWindow* parent)
	: wxPanel(parent) {
	this->ctx = ctx;

	//SetBackgroundColour(*(new wxColour("red")));

	Bind(DOCKER_CONNECT_EVENT, &ContainersWindow::HandleDockerConnect, this, 0);
	Bind(PAGE_REFRESH_EVENT, &ContainersWindow::HandlePageRefresh, this, 0);

	containersSizer = new wxFlexGridSizer(1);

	wxToolBar* toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_HORIZONTAL | wxTB_HORZ_TEXT);
	wxBitmap refresh = wxArtProvider::GetBitmap(wxART_REDO, wxART_TOOLBAR);
	wxBitmap show_run = wxArtProvider::GetBitmap(wxART_TICK_MARK, wxART_TOOLBAR);

	toolBar->AddCheckTool(3, "Show Running", show_run);
	toolBar->AddTool(4, "Refresh", refresh);

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

	containersSizer->Add(toolBar, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 5);
	containersSizer->Add(containerListGrid, 0, wxALL | wxEXPAND, 5);

	if (this->ctx->isConnected()) {
		this->RefreshContainers();
	}
	SetSizerAndFit(containersSizer);
	SetAutoLayout(true);
}

void ContainersWindow::HandleDockerConnect(wxCommandEvent& event) {
	if (this->ctx->isConnected()) {
		this->RefreshContainers();
	}
}

void ContainersWindow::RefreshContainers() {
	//Empty the grid by removing all current rows
	containerListGrid->DeleteRows(0, containerListGrid->GetNumberRows());

	bool show_running = true;
	long limit = 100;
	docker_containers_list* containers;
	docker_result* res;

	//Lookup containers
	docker_container_list(this->ctx->getDockerContext(), &res, &containers, show_running,
		limit, 1, NULL);
	char* report = this->ctx->handleDockerResult(res);
	docker_log_debug("Read %d containers.\n",
		docker_containers_list_length(containers));

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

		if (arraylist_length(item->ports) > 0) {
			docker_container_ports* first_port = (docker_container_ports*)
				arraylist_get(item->ports, 0);
			if (first_port
				&& first_port->public_port > 0
				&& first_port->private_port
							> 0) {
				char* ports_str = (char*)calloc(128, sizeof(char));
				sprintf(ports_str, "%ld:%ld",
					first_port->public_port,
					first_port->private_port);

				containerListGrid->SetCellValue(row_num, col_num, ports_str);
			}
		}
		col_num += 1;

		char* status = (char*)calloc(1024, sizeof(char));
		strcpy(status, item->state);
		strcat(status, ":");
		strcat(status, item->status);
		
		containerListGrid->SetCellValue(row_num, col_num, status);

		row_num += 1;
	}

	if (report != NULL && res->http_error_code == 200) {
		docker_log_debug(report);
	}
	if (res != NULL) {
		free_docker_result(&res);
	}

	containerListGrid->AutoSize();
	Layout();
}

void ContainersWindow::HandlePageRefresh(wxCommandEvent& event) {
	if (this->ctx->isConnected()) {
		this->RefreshContainers();
	}
}