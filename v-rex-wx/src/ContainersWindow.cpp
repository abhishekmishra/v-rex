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

	grid = new wxGrid(this,
		-1,
		wxPoint(0, 0));
	grid->CreateGrid(1, 4);
	grid->SetRowSize(0, 20);
	grid->SetColSize(0, 120);
	grid->SetColLabelValue(0, "Name");
	grid->SetColLabelValue(1, "Image");
	grid->SetColLabelValue(2, "Command");
	grid->SetColLabelValue(3, "State");
	grid->HideRowLabels();

	containersSizer->Add(toolBar, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 5);
	containersSizer->Add(grid, 0, wxALL | wxEXPAND, 5);

	if (this->ctx->isConnected()) {
		this->RefreshContainers();
	}
	SetSizerAndFit(containersSizer);
	SetAutoLayout(true);
}

void ContainersWindow::handlerDockerConnect(wxCommandEvent& event) {
	if (this->ctx->isConnected()) {
		this->RefreshContainers();
	}
}

void ContainersWindow::RefreshContainers() {
	//Empty the grid by removing all current rows
	grid->DeleteRows(0, grid->GetNumberRows());

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
	grid->InsertRows(0, docker_containers_list_length(containers));

	int col_num = 0;
	int row_num = 0;
	char* first_id;
	for (int i = 0; i < docker_containers_list_length(containers); i++) {
		col_num = 0;
		docker_container_list_item* item = docker_containers_list_get_idx(
			containers, i);
		grid->SetCellValue(row_num, col_num, (char*)arraylist_get(item->names, 0));
		col_num += 1;

		if (i == 0) {
			first_id = (item->id);
		}

		grid->SetCellValue(row_num, col_num, item->image);
		col_num += 1;

		grid->SetCellValue(row_num, col_num, item->command);
		col_num += 1;

		//if (arraylist_length(item->ports) > 0) {
		//	docker_container_ports* first_port = (docker_container_ports*)
		//		arraylist_get(item->ports, 0);
		//	if (first_port
		//		&& first_port->public_port > 0
		//		&& first_port->private_port
		//					> 0) {
		//		char* ports_str = (char*)calloc(128, sizeof(char));
		//		sprintf(ports_str, "%ld:%ld",
		//			first_port->public_port,
		//			first_port->private_port);
		//	}
		//}
		char* status = (char*)calloc(1024, sizeof(char));
		strcpy(status, item->state);
		strcat(status, ":");
		strcat(status, item->status);
		grid->SetCellValue(row_num, col_num, status);

		row_num += 1;
	}

	if (report != NULL && res->http_error_code == 200) {
		docker_log_debug(report);
	}
	if (res != NULL) {
		free_docker_result(&res);
	}

	grid->AutoSize();
	Layout();
}