#pragma once

#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/toolbar.h>

#include "VRexContext.h"
#include "vrex_util.h"

class ContainersWindow: public wxPanel {
public:
    ContainersWindow(VRexContext* ctx, wxWindow* parent);
	bool isShowRunningEnabled();

private:
	VRexContext* ctx;
	wxFlexGridSizer* containersSizer;
	wxToolBar* toolBar;
	wxGrid* containerListGrid;
	bool showRunning;

	wxButton* startBtn;
	wxButton* stopBtn;
	wxButton* killBtn;
	wxButton* restartBtn;
	wxButton* pauseBtn;
	wxButton* resumeBtn;
	wxButton* removeBtn;
	wxButton* addBtn;

	void HandleDockerConnect(wxCommandEvent& event);
	void HandleListContainers(wxCommandEvent& event);
	void HandlePageRefresh(wxCommandEvent& event);
	void HandleShowRunning(wxCommandEvent& event);
	void RefreshContainers();
	void UpdateContainers(docker_ctr_list* containers);
	void HandleCellSelection(wxGridEvent& event);

	d_err_t RunContainerCommand(const char* container_name_or_id, int command);
	d_err_t HandleContainerBtnEvent(wxCommandEvent& event, int eventCommand, wxString success, wxString failure);
	void HandleContainerStart(wxCommandEvent& event);
	void HandleContainerStop(wxCommandEvent& event);
	void HandleContainerKill(wxCommandEvent& event);
	void HandleContainerRestart(wxCommandEvent& event);
	void HandleContainerPause(wxCommandEvent& event);
	void HandleContainerResume(wxCommandEvent& event);
	void HandleContainerRemove(wxCommandEvent& event);
	void HandleContainerAdd(wxCommandEvent& event);
	void HandleContainerDetails(wxCommandEvent& event);
};