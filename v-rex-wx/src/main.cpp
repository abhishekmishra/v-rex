// wxWidgets "Hello World" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/cmdline.h>

#include <stdio.h>
#include <stdlib.h>

#include <wx/notebook.h>
#include <wx/toolbar.h>
#include <wx/artprov.h>

#include <arraylist.h>
#include <docker_all.h>

#include "VRexContext.h"
#include "DashboardWindow.h"
#include "ContainersWindow.h"

class VRexApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual void OnInitCmdLine(wxCmdLineParser& parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

private:
	VRexContext* ctx;
	bool silent_mode;
	char* docker_url = NULL;
};
class VRexFrame : public wxFrame
{
public:
	VRexFrame(VRexContext* ctx);
	void SetContext(VRexContext* ctx);
	void PostEventToCurrentTab(const wxCommandEvent& event);
	void PostEventToAllTabs(const wxCommandEvent& event);

private:
	void OnHello(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnNBPageChanged(wxBookCtrlEvent& event);

	VRexContext* ctx = NULL;
	wxNotebook* notebook;
};

enum
{
	ID_Hello = 1
};

static const wxCmdLineEntryDesc g_cmdLineDesc[] =
{
	{ wxCMD_LINE_SWITCH, "h", "help", "displays help on the command line parameters",
		  wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
	{ wxCMD_LINE_SWITCH, "s", "silent", "disables the GUI" },
	{ wxCMD_LINE_PARAM, NULL, NULL, "Docker URL", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_NONE }
};


wxIMPLEMENT_APP(VRexApp);

wxDEFINE_EVENT(DOCKER_CONNECT_EVENT, wxCommandEvent);
wxDEFINE_EVENT(PAGE_REFRESH_EVENT, wxCommandEvent);

bool VRexApp::OnInit()
{
	// call default behaviour (mandatory)
	if (!wxApp::OnInit())
		return false;

	ctx = new VRexContext();

	VRexFrame* frame = new VRexFrame(ctx);
	frame->Show(true);

	if (docker_url != NULL) {
		ctx->TryConnectURL(docker_url);
	}
	else {
		ctx->TryConnectLocal();
	}

	docker_log_debug("Connected is %d\n", ctx->isConnected());

	if (ctx->isConnected()) {
		docker_version* version = ctx->getDockerVersion();
		frame->SetStatusText("Connected", 0);
		char* version_info = (char*)calloc(1024, sizeof(char));
		if (version_info != NULL) {
			sprintf(version_info, "Docker v%s on %s [%s] @ %s",
				version->version, version->os, version->arch, ctx->getDockerContext()->url);
			frame->SetStatusText(version_info, 2);
		}

		wxCommandEvent event(DOCKER_CONNECT_EVENT);
		event.SetEventObject(this);
		// Give it some contents
		event.SetString("Hello");
		// Do send it
		frame->PostEventToCurrentTab(event);
	}
	else {
		frame->SetStatusText("Not Connected.", 0);
		frame->SetStatusText("Error: Unable to connect to a local docker server", 1);
	}

	return true;
}

VRexFrame::VRexFrame(VRexContext* ctx)
	: wxFrame(NULL, wxID_ANY, "V-Rex: Container GUI", wxPoint(0, 0),
		wxSize(1024, 768))
{
	SetContext(ctx);
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

	notebook = new wxNotebook(this, -1, wxPoint(0, 0));

	DashboardWindow* dashboardW = new DashboardWindow(this->ctx, notebook);
	notebook->AddPage(dashboardW, "Dashboard");

	ContainersWindow* containersW = new ContainersWindow(this->ctx, notebook);
	notebook->AddPage(containersW, "Containers");

	notebook->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &VRexFrame::OnNBPageChanged, this);

	wxToolBar* toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_HORIZONTAL | wxTB_HORZ_TEXT);
	SetToolBar(toolBar);
	wxBitmap exit = wxArtProvider::GetBitmap(wxART_QUIT, wxART_TOOLBAR);

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
	wxMessageBox("This is a wxWidgets Hello World example",
		"About Hello World", wxOK | wxICON_INFORMATION);
}

void VRexFrame::OnHello(wxCommandEvent& event)
{
	wxLogMessage("Hello world from wxWidgets!");
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


void VRexApp::OnInitCmdLine(wxCmdLineParser& parser)
{
	parser.SetDesc(g_cmdLineDesc);
	// must refuse '/' as parameter starter or cannot use "/path" style paths
	parser.SetSwitchChars(wxT("-"));
}

bool VRexApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
	silent_mode = parser.Found(wxT("s"));

	if (parser.GetParamCount() > 0) {
		wxCharBuffer buffer = parser.GetParam(0).ToUTF8();
		docker_url = str_clone(buffer.data());
	}

	// to get at your unnamed parameters use
	wxArrayString files;
	for (int i = 0; i < parser.GetParamCount(); i++)
	{
		files.Add(parser.GetParam(i));
	}

	// and other command line parameters

	// then do what you need with them.

	return true;
}

void VRexFrame::OnNBPageChanged(wxBookCtrlEvent& event) {
	
	wxCommandEvent page_refresh_event(PAGE_REFRESH_EVENT);
	page_refresh_event.SetEventObject(event.GetEventObject());
	page_refresh_event.SetString("RefreshContents");
	int page_id = event.GetSelection();
	wxPostEvent(this->notebook->GetPage(page_id), page_refresh_event);

	event.Skip();
}