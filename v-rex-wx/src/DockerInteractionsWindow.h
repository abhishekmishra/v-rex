#pragma once
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "arraylist.h"

class DockerInteractionsWindow : public wxFrame
{
public:
	DockerInteractionsWindow(wxWindow* parent);
private:
	void HandleCloseEvent(wxCloseEvent& event);
	void HandleDockerResultEvent(wxCommandEvent& event);
	void HandleSelectionEvent(wxCommandEvent& event);
	void SetValuesForSelection();
	wxTextCtrl* methodText;
	wxTextCtrl* urlText;
	wxTextCtrl* startTimeText;
	wxTextCtrl* endTimeText;
	wxTextCtrl* errorCodeText;
	wxTextCtrl* httpErrorCodeText;
	wxTextCtrl* requestText;
	wxTextCtrl* responseText;
	wxListBox* interactionsList;
	arraylist* resList;
	int limit;
};
