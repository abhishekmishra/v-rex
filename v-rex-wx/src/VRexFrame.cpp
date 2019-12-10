#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/notebook.h>
#include <wx/toolbar.h>
#include <wx/artprov.h>

#include <arraylist.h>
#include <docker_all.h>

#include "VRexContext.h"
#include "DashboardWindow.h"
#include "ContainersWindow.h"
#include "ImagesWindow.h"
#include "VolumesWindow.h"
#include "NetworksWindow.h"
#include "DockerInteractionsWindow.h"
#include "VRexUtil.h"

#include "VRexFrame.h"

wxDEFINE_EVENT(DOCKER_INTERACTION_HIDE_EVENT, wxCommandEvent);

#define VREX_FRAME_TOOL_INTERACTIONS	120

VRexFrame::VRexFrame(VRexContext* ctx)
	: wxFrame(NULL, wxID_ANY, "V-Rex: Container GUI", wxPoint(0, 0),
		wxSize(1024, 768))
{
	SetContext(ctx);
	interactionsW = new DockerInteractionsWindow(this);
	ctx->RegisterInteractionsWindow(interactionsW);
	wxMenu* menuFile = new wxMenu;
	menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
		"Help string shown in status bar for this menu item");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);
	wxMenu* menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);
	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);

	CreateStatusBar(3);
	SetStatusText("Welcome to V-Rex!");

	Bind(wxEVT_MENU, &VRexFrame::OnHello, this, ID_Hello);
	Bind(wxEVT_MENU, &VRexFrame::OnAbout, this, wxID_ABOUT);
	Bind(wxEVT_MENU, &VRexFrame::OnExit, this, wxID_EXIT);
	Bind(wxEVT_TOOL, &VRexFrame::HandleShowInteractions, this, VREX_FRAME_TOOL_INTERACTIONS);
	Bind(DOCKER_INTERACTION_HIDE_EVENT, &VRexFrame::HandleHideInteractions, this, 0);

	notebook = new wxNotebook(this, -1, wxPoint(0, 0));

	DashboardWindow* dashboardW = new DashboardWindow(this->ctx, notebook);
	notebook->AddPage(dashboardW, "Dashboard");

	ContainersWindow* containersW = new ContainersWindow(this->ctx, notebook);
	notebook->AddPage(containersW, "Containers");

	ImagesWindow* imagesW = new ImagesWindow(this->ctx, notebook);
	notebook->AddPage(imagesW, "Images");

	VolumesWindow* volumesW = new VolumesWindow(this->ctx, notebook);
	notebook->AddPage(volumesW, "Volumes");

	NetworksWindow* networksW = new NetworksWindow(this->ctx, notebook);
	notebook->AddPage(networksW, "Networks");

	//make containers the default page
	notebook->ChangeSelection(1);
	notebook->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &VRexFrame::OnNBPageChanged, this);

	wxToolBar* toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_HORIZONTAL | wxTB_HORZ_TEXT);
	SetToolBar(toolBar);
	wxBitmap interactions = wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_TOOLBAR);
	wxBitmap exit = wxArtProvider::GetBitmap(wxART_QUIT, wxART_TOOLBAR);

	toolBar->AddCheckTool(VREX_FRAME_TOOL_INTERACTIONS, "Interactions", interactions);
	toolBar->ToggleTool(VREX_FRAME_TOOL_INTERACTIONS, false);

	toolBar->AddTool(wxID_EXIT, "Exit", exit);
	toolBar->Realize();
}

void VRexFrame::SetContext(VRexContext* ctx) {
	this->ctx = ctx;
}

void VRexFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void VRexFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox("V-Rex: Container Management\nVersion: 0.1a\nAuthor: Abhishek Mishra <abhishekmishra3@gmail.com>",
		"About V-Rex", wxOK | wxICON_INFORMATION);
}

void VRexFrame::OnHello(wxCommandEvent& event)
{
	wxLogMessage("Hello world from V-Rex!");
}

void VRexFrame::PostEventToCurrentTab(const wxCommandEvent& event)
{
	wxPostEvent(this->notebook->GetCurrentPage(), event);
}

void VRexFrame::PostEventToAllTabs(const wxCommandEvent& event)
{
	for (int i = 0; i < notebook->GetPageCount(); i++) {
		wxPostEvent(this->notebook->GetPage(i), event);
	}
}

void VRexFrame::OnNBPageChanged(wxBookCtrlEvent& event) {
	
	wxCommandEvent page_refresh_event(PAGE_REFRESH_EVENT);
	page_refresh_event.SetEventObject(event.GetEventObject());
	page_refresh_event.SetString("RefreshContents");
	int page_id = event.GetSelection();
	wxPostEvent(this->notebook->GetPage(page_id), page_refresh_event);

	event.Skip();
}

void VRexFrame::HandleShowInteractions(wxCommandEvent& event) {
	bool showInteractions = GetToolBar()->GetToolState(VREX_FRAME_TOOL_INTERACTIONS);
	if (showInteractions) {
		interactionsW->Show();
	}
	else {
		interactionsW->Hide();
	}
}

void VRexFrame::HandleHideInteractions(wxCommandEvent& event) {
	GetToolBar()->ToggleTool(VREX_FRAME_TOOL_INTERACTIONS, false);
}