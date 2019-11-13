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

#include "VRexFrame.h"
#include "VRexContext.h"

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

	registerX11Colours();

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
