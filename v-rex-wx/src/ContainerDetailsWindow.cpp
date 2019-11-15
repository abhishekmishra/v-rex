#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/toolbar.h>
#include <wx/gbsizer.h>

#include "DockerRequestThread.h"
#include "ContainerDetailsWindow.h"
#include "VRexContext.h"
#include "vrex_util.h"

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

	virtual const wxEventTypeTag<wxCommandEvent> DockerRequest(docker_result** res, void** clientData);
};

const wxEventTypeTag<wxCommandEvent> ContainerLogsRequestThread::DockerRequest(docker_result** res, void** clientData) {
	char* log;
	docker_container_logs(ctx->getDockerContext(), res, &log, container_name_or_id, 0, 1, 1, -1, -1, 1, 0);
	int len = strlen(log);
	*clientData = log;
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

void ContainerLogsPanel::HandleLogs(wxCommandEvent& event) {
	char* logs = (char*)event.GetClientData();
	if (logs != NULL) {
		logs_text->AppendText(logs);
		//free(logs);
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


ContainerStatsPanel::ContainerStatsPanel(VRexContext* ctx, wxWindow* parent, char* container_name_or_id)
	: wxPanel(parent) {
	this->ctx = ctx;
	this->container_name_or_id = container_name_or_id;

}

ContainerPSPanel::ContainerPSPanel(VRexContext* ctx, wxWindow* parent, char* container_name_or_id)
	: wxPanel(parent) {
	this->ctx = ctx;
	this->container_name_or_id = container_name_or_id;

}