#pragma once
#pragma once

#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/toolbar.h>
#include <wx/gbsizer.h>

#include "ContainerDetailsWindow.h"
#include "VRexContext.h"
#include "vrex_util.h"

ContainerDetailsDialog::ContainerDetailsDialog(VRexContext* ctx, const wxString& title, char* container_name_or_id)
	: wxDialog(NULL, -1, title, wxDefaultPosition, wxSize(250, 230)) {
	this->ctx = ctx;
	this->container_name_or_id = container_name_or_id;

	wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
	ContainerDetailsWindow* detWin = new ContainerDetailsWindow(ctx, this, container_name_or_id);

	main_sizer->Add(detWin);

	wxSizer* button_sizer = CreateSeparatedButtonSizer(wxCLOSE);
	if (button_sizer)
		main_sizer->Add(button_sizer, wxSizerFlags().Expand().Border());

	SetSizerAndFit(main_sizer);
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

ContainerLogsPanel::ContainerLogsPanel(VRexContext* ctx, wxWindow* parent, char* container_name_or_id)
	: wxPanel(parent) {
	this->ctx = ctx;
	this->container_name_or_id = container_name_or_id;

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