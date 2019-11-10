// wxWidgets "Hello World" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

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
				wsprintf(version_info, L"%S", res->response_json_str);
				wxMessageBox(version_info,
					"Docker Version Info", wxOK | wxICON_INFORMATION);
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
	: wxFrame(NULL, wxID_ANY, "Hello World")
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
	CreateStatusBar();
	SetStatusText("Welcome to wxWidgets!");
	Bind(wxEVT_MENU, &VRexFrame::OnHello, this, ID_Hello);
	Bind(wxEVT_MENU, &VRexFrame::OnAbout, this, wxID_ABOUT);
	Bind(wxEVT_MENU, &VRexFrame::OnExit, this, wxID_EXIT);
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
