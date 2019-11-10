// wxWidgets "Hello World" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/grid.h>
#include <wx/toolbar.h>
#include <wx/artprov.h>

#include <arraylist.h>
#include <docker_all.h>

//For Docker API
docker_context* ctx;
docker_result* res;
docker_version* version = NULL;
docker_info* info = NULL;

/**
 * A simple error handler suitable for programs
 * which just want to log the error (if any).
 */
wchar_t* docker_simple_error_handler_wsprintf(docker_result* res) {
	wchar_t* report = (wchar_t*)calloc(2048, sizeof(wchar_t));
	wsprintf(report,
		L"DOCKER_RESULT: For URL: %S\n DOCKER RESULT: Response error_code = %d, http_response = %ld\n",
		res->url,
		res->error_code,
		res->http_error_code);
	if (!is_ok(res)) {
		wsprintf(report, L"DOCKER RESULT: %S\n", res->message);
	}
	return report;
}

wchar_t* handle_error(docker_result* res) {
	return docker_simple_error_handler_wsprintf(res);
}


class VRexApp : public wxApp
{
public:
	virtual bool OnInit();
};
class VRexFrame : public wxFrame
{
public:
	VRexFrame();
private:
	void OnHello(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
};

enum
{
	ID_Hello = 1
};

wxIMPLEMENT_APP(VRexApp);

bool VRexApp::OnInit()
{
	VRexFrame* frame = new VRexFrame();
	frame->Show(true);

			// connect to docker
		d_err_t err = make_docker_context_default_local(&ctx);
		if (err == E_SUCCESS) {
			docker_system_version(ctx, &res, &version);
			wchar_t* report = handle_error(res);
			if (report != NULL && res->http_error_code == 200) {
				wchar_t* version_info = (wchar_t*)calloc(10240, sizeof(wchar_t));
				wsprintf(version_info, L"Docker: %S [%S]", ctx->socket == NULL? ctx->url : ctx->socket, version->os);
				//wxMessageBox(version_info,
				//	"Docker Version Info", wxOK | wxICON_INFORMATION);
				frame->SetStatusText("Connected", 0);
				frame->SetStatusText(version_info, 2);
				//MessageBox(NULL, version_info,
				//	L"Docker Version Info", MB_ICONINFORMATION);
			}
			free_docker_result(&res);

			docker_system_info(ctx, &res, &info);
			report = handle_error(res);
			if (report != NULL && res->http_error_code == 200) {
			}
		}

	return true;
}

VRexFrame::VRexFrame()
	: wxFrame(NULL, wxID_ANY, "V-Rex: Container GUI", wxPoint(0, 0),
		wxSize(1024, 768))
{
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
	SetStatusText("Welcome to wxWidgets!");

	Bind(wxEVT_MENU, &VRexFrame::OnHello, this, ID_Hello);
	Bind(wxEVT_MENU, &VRexFrame::OnAbout, this, wxID_ABOUT);
	Bind(wxEVT_MENU, &VRexFrame::OnExit, this, wxID_EXIT);

	// Create a wxGrid object
	wxGrid* grid = new wxGrid(this,
		-1,
		wxPoint(0, 0),
		wxSize(800, 600));
	// Then we call CreateGrid to set the dimensions of the grid
	// (100 rows and 10 columns in this example)
	grid->CreateGrid(100, 10);
	// We can set the sizes of individual rows and columns
	// in pixels
	grid->SetRowSize(0, 60);
	grid->SetColSize(0, 120);
	// And set grid cell contents as strings
	grid->SetCellValue(0, 0, "wxGrid is good");
	// We can specify that some cells are read->only
	grid->SetCellValue(0, 3, "This is read->only");
	grid->SetReadOnly(0, 3);
	// Colours can be specified for grid cell contents
	grid->SetCellValue(3, 3, "green on grey");
	grid->SetCellTextColour(3, 3, *wxGREEN);
	grid->SetCellBackgroundColour(3, 3, *wxLIGHT_GREY);
	// We can specify the some cells will store numeric
	// values rather than strings. Here we set grid column 5
	// to hold floating point values displayed with width of 6
	// and precision of 2
	grid->SetColFormatFloat(5, 6, 2);
	grid->SetCellValue(0, 6, "3.1415");

	wxToolBar* toolBar = CreateToolBar();
	wxBitmap open = wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_TOOLBAR);
	wxBitmap exit = wxArtProvider::GetBitmap(wxART_QUIT, wxART_TOOLBAR);
	wxBitmap save = wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_TOOLBAR);
	wxBitmap b_new = wxArtProvider::GetBitmap(wxART_NEW, wxART_TOOLBAR);

	toolBar->AddTool(3, "New file", b_new);
	toolBar->AddTool(4, "Open file", open);
	toolBar->AddTool(5, "Save file", save);
	toolBar->AddTool(wxID_EXIT, "Exit", exit);
	toolBar->Realize();
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
