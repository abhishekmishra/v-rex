#pragma once

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
