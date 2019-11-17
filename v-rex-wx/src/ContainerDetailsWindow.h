#pragma once

#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/toolbar.h>
#include <wx/gbsizer.h>
#include <wx/dialog.h>

#include "VRexContext.h"
#include "vrex_util.h"

class ContainerDetailsDialog : public wxDialog
{
public:
	ContainerDetailsDialog(VRexContext* ctx, const wxString& title, char* container_name_or_id);
private:
	VRexContext* ctx;
	char* container_name_or_id;
};

class ContainerDetailsWindow : public wxPanel {
public:
	ContainerDetailsWindow(VRexContext* ctx, wxWindow* parent, char* container_name_or_id);

private:
	VRexContext* ctx;
	wxGridBagSizer* ctrDetSizer;
	char* container_name_or_id;
};

class ContainerDetailsPanel : public wxPanel {
public:
	ContainerDetailsPanel(VRexContext* ctx, wxWindow* parent, char* container_name_or_id);

private:
	VRexContext* ctx;
	char* container_name_or_id;
};

class ContainerLogsPanel : public wxPanel {
public:
	ContainerLogsPanel(VRexContext* ctx, wxWindow* parent, char* container_name_or_id);

private:
	void HandleLogs(wxCommandEvent& event);
	void RefreshLogs();

	VRexContext* ctx;
	char* container_name_or_id;
	wxTextCtrl* logs_text;
};

class ContainerStatsPanel : public wxPanel {
public:
	ContainerStatsPanel(VRexContext* ctx, wxWindow* parent, char* container_name_or_id);

private:
	void HandleStats(wxCommandEvent& event);
	void RefreshStats();

	VRexContext* ctx;
	char* container_name_or_id;
	wxGrid* memStatsListGrid;
	wxGrid* cpuStatsListGrid;
};

class ContainerPSPanel : public wxPanel {
public:
	ContainerPSPanel(VRexContext* ctx, wxWindow* parent, char* container_name_or_id);

private:
	VRexContext* ctx;
	char* container_name_or_id;
};