#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/toolbar.h>
#include <wx/gbsizer.h>

#include "DockerRequestThread.h"
#include "ContainerDetailsWindow.h"
#include "VRexContext.h"
#include "VRexUtil.h"

#include "docker_all.h"

ContainerDetailsDialog::ContainerDetailsDialog(VRexContext* ctx, const wxString& title, char* container_name_or_id)
	: wxDialog(NULL, -1, title, wxDefaultPosition, wxSize(500, 800)) {
	this->ctx = ctx;
	this->container_name_or_id = container_name_or_id;

	wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
	ContainerDetailsWindow* detWin = new ContainerDetailsWindow(ctx, this, container_name_or_id);

	main_sizer->Add(detWin);

	wxSizer* button_sizer = CreateSeparatedButtonSizer(wxCLOSE);
	if (button_sizer)
		main_sizer->Add(button_sizer, wxSizerFlags().Expand().Border());

	SetSizerAndFit(main_sizer);
	SetAutoLayout(true);
}

ContainerDetailsWindow::ContainerDetailsWindow(VRexContext* ctx, wxWindow* parent, char* container_name_or_id)
	: wxPanel(parent) {
	this->ctx = ctx;
	this->container_name_or_id = container_name_or_id;

	ctrDetSizer = new wxGridBagSizer();

	ContainerDetailsPanel* detPanel = new ContainerDetailsPanel(ctx, this, container_name_or_id);
	ContainerLogsPanel* logsPanel = new ContainerLogsPanel(ctx, this, container_name_or_id);
	ContainerStatsPanel* statsPanel = new ContainerStatsPanel(ctx, this, container_name_or_id);
	ContainerPSPanel* psPanel = new ContainerPSPanel(ctx, this, container_name_or_id);

	ctrDetSizer->Add(detPanel, wxGBPosition(0, 0), wxGBSpan(1, 4));
	ctrDetSizer->Add(logsPanel, wxGBPosition(1, 0), wxGBSpan(1, 4));
	ctrDetSizer->Add(statsPanel, wxGBPosition(2, 0), wxGBSpan(1, 4));
	ctrDetSizer->Add(psPanel, wxGBPosition(3, 0), wxGBSpan(1, 4));

	SetSizerAndFit(ctrDetSizer);
}

ContainerDetailsPanel::ContainerDetailsPanel(VRexContext* ctx, wxWindow* parent, char* container_name_or_id)
	: wxPanel(parent) {
	this->ctx = ctx;
	this->container_name_or_id = container_name_or_id;

}

wxDEFINE_EVENT(DOCKER_LOGS_EVENT, wxCommandEvent);

class ContainerLogsRequestThread : public DockerRequestThread {
private:
	char* container_name_or_id;
public:
	ContainerLogsRequestThread(wxWindow* parent, VRexContext* ctx, char* container_name_or_id)
		: DockerRequestThread(parent, ctx)
	{
		this->container_name_or_id = container_name_or_id;
	}

	virtual const wxEventTypeTag<wxCommandEvent> DockerRequest(void** clientData);
};

const wxEventTypeTag<wxCommandEvent> ContainerLogsRequestThread::DockerRequest(void** clientData) {
	DockerLog* dlog = new DockerLog();

	docker_container_logs(ctx->getDockerContext(), &dlog->log, &dlog->log_len, container_name_or_id, 0, 1, 1, -1, -1, 1, 0);
	*clientData = dlog;
	return DOCKER_LOGS_EVENT;
}


ContainerLogsPanel::ContainerLogsPanel(VRexContext* ctx, wxWindow* parent, char* container_name_or_id)
	: wxPanel(parent) {
	this->ctx = ctx;
	this->container_name_or_id = container_name_or_id;

	wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
	logs_text = new wxTextCtrl(this, 0, wxEmptyString, wxDefaultPosition, wxSize(800, 600), wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
	logs_text->SetBackgroundColour(*wxBLACK);
	logs_text->SetDefaultStyle(wxTextAttr(*wxGREEN, *wxBLACK));

	Bind(DOCKER_LOGS_EVENT, &ContainerLogsPanel::HandleLogs, this, 0);

	main_sizer->Add(logs_text);

	RefreshLogs();

	SetSizerAndFit(main_sizer);
	SetAutoLayout(true);
}

void vrex_log_line_handler(void* args, int stream_id, int line_num, char* line) {
	wxTextCtrl* logs_text = (wxTextCtrl*)args;
	if (line_num == 0) {
		docker_log_info("Stream %d, line# %d :: %s", stream_id, line_num, line);

		//TODO: change style based on stream type
		logs_text->AppendText(line);
	}
}

void ContainerLogsPanel::HandleLogs(wxCommandEvent& event) {
	DockerLog* dlog = (DockerLog*)event.GetClientData();
	if (dlog != NULL) {
		docker_container_logs_foreach(logs_text, dlog->log, dlog->log_len, &vrex_log_line_handler);
	}
}

void ContainerLogsPanel::RefreshLogs() {
	if (this->ctx->isConnected()) {
		// create the thread
		ContainerLogsRequestThread* t = new ContainerLogsRequestThread(this, this->ctx, container_name_or_id);
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

wxDEFINE_EVENT(DOCKER_STATS_EVENT, wxCommandEvent);

class ContainerStatsRequestThread : public DockerRequestThread {
private:
	char* container_name_or_id;
public:
	ContainerStatsRequestThread(wxWindow* parent, VRexContext* ctx, char* container_name_or_id)
		: DockerRequestThread(parent, ctx)
	{
		this->container_name_or_id = container_name_or_id;
	}

	virtual const wxEventTypeTag<wxCommandEvent> DockerRequest(void** clientData);
};

const wxEventTypeTag<wxCommandEvent> ContainerStatsRequestThread::DockerRequest(void** clientData) {
	docker_container_stats* stats;
	docker_container_get_stats(ctx->getDockerContext(), &stats, container_name_or_id);
	*clientData = stats;
	return DOCKER_STATS_EVENT;
}

#define VREX_CONTAINER_STATS_DEFAULT_COLUMN_WIDTH 60

ContainerStatsPanel::ContainerStatsPanel(VRexContext* ctx, wxWindow* parent, char* container_name_or_id)
	: wxPanel(parent) {
	this->ctx = ctx;
	this->container_name_or_id = container_name_or_id;

	wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
	Bind(DOCKER_STATS_EVENT, &ContainerStatsPanel::HandleStats, this, 0);

	memStatsListGrid = new wxGrid(this,
		-1,
		wxDefaultPosition,
		wxDefaultSize);
	memStatsListGrid->CreateGrid(1, 4);
	memStatsListGrid->SetDefaultCellOverflow(false);
	memStatsListGrid->SetRowSize(0, 20);
	int grid_col_count = 0;
	memStatsListGrid->SetColSize(grid_col_count++, VREX_CONTAINER_STATS_DEFAULT_COLUMN_WIDTH);
	memStatsListGrid->SetColSize(grid_col_count++, VREX_CONTAINER_STATS_DEFAULT_COLUMN_WIDTH);
	memStatsListGrid->SetColSize(grid_col_count++, VREX_CONTAINER_STATS_DEFAULT_COLUMN_WIDTH);
	memStatsListGrid->SetColSize(grid_col_count++, VREX_CONTAINER_STATS_DEFAULT_COLUMN_WIDTH);

	grid_col_count = 0;
	memStatsListGrid->SetColLabelValue(grid_col_count++, "Max Usage");
	memStatsListGrid->SetColLabelValue(grid_col_count++, "Usage");
	memStatsListGrid->SetColLabelValue(grid_col_count++, "Fail Count");
	memStatsListGrid->SetColLabelValue(grid_col_count++, "Limit");
	memStatsListGrid->HideRowLabels();

	memStatsListGrid->SetGridLineColour(VREX_WHITESMOKE);
	memStatsListGrid->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	memStatsListGrid->EnableEditing(false);

	cpuStatsListGrid = new wxGrid(this,
		-1,
		wxDefaultPosition,
		wxDefaultSize);
	cpuStatsListGrid->CreateGrid(1, 4);
	cpuStatsListGrid->SetDefaultCellOverflow(false);
	cpuStatsListGrid->SetRowSize(0, 20);
	grid_col_count = 0;
	cpuStatsListGrid->SetColSize(grid_col_count++, VREX_CONTAINER_STATS_DEFAULT_COLUMN_WIDTH);
	cpuStatsListGrid->SetColSize(grid_col_count++, VREX_CONTAINER_STATS_DEFAULT_COLUMN_WIDTH);
	cpuStatsListGrid->SetColSize(grid_col_count++, VREX_CONTAINER_STATS_DEFAULT_COLUMN_WIDTH);
	cpuStatsListGrid->SetColSize(grid_col_count++, VREX_CONTAINER_STATS_DEFAULT_COLUMN_WIDTH);

	grid_col_count = 0;
	cpuStatsListGrid->SetColLabelValue(grid_col_count++, "Total Usage");
	cpuStatsListGrid->SetColLabelValue(grid_col_count++, "Usage In UserMode");
	cpuStatsListGrid->SetColLabelValue(grid_col_count++, "Usage in KernelMode");
	cpuStatsListGrid->SetColLabelValue(grid_col_count++, "System CPU Usage");
	cpuStatsListGrid->HideRowLabels();

	cpuStatsListGrid->SetGridLineColour(VREX_WHITESMOKE);
	cpuStatsListGrid->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	cpuStatsListGrid->EnableEditing(false);

	main_sizer->Add(memStatsListGrid);
	main_sizer->Add(cpuStatsListGrid);
	memStatsListGrid->AutoSize();
	cpuStatsListGrid->AutoSize();

	RefreshStats();

	SetSizerAndFit(main_sizer);
	SetAutoLayout(true);
}

void ContainerStatsPanel::HandleStats(wxCommandEvent& event) {
	docker_container_stats* stats = (docker_container_stats*)event.GetClientData();
	//TODO fix display units
	docker_cpu_usage* cpu_usage = docker_container_cpu_stats_cpu_usage_get(docker_container_stats_cpu_stats_get(stats));
	cpuStatsListGrid->SetCellValue(0, 0,
		wxString::Format("%lld", docker_cpu_usage_total_get(cpu_usage)));
	cpuStatsListGrid->SetCellValue(0, 1,
		wxString::Format("%lld", docker_cpu_usage_usermode_get(cpu_usage)));
	cpuStatsListGrid->SetCellValue(0, 2,
		wxString::Format("%lld", docker_cpu_usage_kernelmode_get(cpu_usage)));
	cpuStatsListGrid->SetCellValue(0, 2,
		wxString::Format("%lld", docker_container_cpu_stats_system_cpu_usage_get(docker_container_stats_cpu_stats_get(stats))));
	memStatsListGrid->SetCellValue(0, 0,
		calculate_size(docker_container_mem_stats_max_usage_get(docker_container_stats_mem_stats_get(stats))));
	memStatsListGrid->SetCellValue(0, 1,
		calculate_size(docker_container_mem_stats_usage_get(docker_container_stats_mem_stats_get(stats))));
	memStatsListGrid->SetCellValue(0, 2,
		wxString::Format("%lld", docker_container_mem_stats_failcnt_get(docker_container_stats_mem_stats_get(stats))));
	memStatsListGrid->SetCellValue(0, 3,
		calculate_size(docker_container_mem_stats_limit_get(docker_container_stats_mem_stats_get(stats))));
}

void ContainerStatsPanel::RefreshStats() {
	if (this->ctx->isConnected()) {
		// create the thread
		ContainerStatsRequestThread* t = new ContainerStatsRequestThread(this, this->ctx, container_name_or_id);
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


ContainerPSPanel::ContainerPSPanel(VRexContext* ctx, wxWindow* parent, char* container_name_or_id)
	: wxPanel(parent) {
	this->ctx = ctx;
	this->container_name_or_id = container_name_or_id;

}