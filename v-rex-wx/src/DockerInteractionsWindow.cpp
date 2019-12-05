#include "DockerInteractionsWindow.h"
#include <wx/gbsizer.h>
#include "arraylist.h"
#include "vrex_util.h"
#include "docker_log.h"

wxDEFINE_EVENT(DOCKER_INTERACTION_RESULT_EVENT, wxCommandEvent);

//TODO: may need pretty print of json response string.
DockerInteractionsWindow::DockerInteractionsWindow(wxWindow* parent)
	: wxFrame(parent, -1, "Docker Interactions Window") {
	//TODO: removed background colour for now but need to figure out 
	//why the window has grey background in win 10
	//SetBackgroundColour(*(new wxColour(VREX_WHITESMOKE)));
	arraylist_new(&resList, NULL);
	limit = 10;

	wxGridBagSizer* main_sizer = new wxGridBagSizer();
	main_sizer->SetFlexibleDirection(wxBOTH);
	main_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	methodText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	urlText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, *(new wxSize(400, wxDefaultSize.GetHeight())), wxTE_READONLY);
	startTimeText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, *(new wxSize(200, wxDefaultSize.GetHeight())), wxTE_READONLY);
	endTimeText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, *(new wxSize(200, wxDefaultSize.GetHeight())), wxTE_READONLY);
	errorCodeText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	httpErrorCodeText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	requestText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxSize(800, 250), wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
	responseText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxSize(800, 250), wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
	interactionsList = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(800, 100), wxArrayString(), wxLB_SINGLE);

	int row = 0;

	main_sizer->Add(new wxStaticText(this, wxID_ANY, "Select Call"), wxGBPosition(row, 0), wxGBSpan(1, 1), wxALL, 5);
	main_sizer->Add(interactionsList, wxGBPosition(row, 1), wxGBSpan(1, 3), wxALL, 5);

	row += 1;

	main_sizer->Add(new wxStaticText(this, wxID_ANY, "Method"), wxGBPosition(row, 0), wxGBSpan(1, 1), wxALL, 5);
	main_sizer->Add(methodText, wxGBPosition(row, 1), wxGBSpan(1, 1), wxALL, 5);
	main_sizer->Add(new wxStaticText(this, wxID_ANY, "URL"), wxGBPosition(row, 2), wxGBSpan(1, 1), wxALL, 5);
	main_sizer->Add(urlText, wxGBPosition(row, 3), wxGBSpan(1, 1), wxALL, 5);

	row += 1;

	main_sizer->Add(new wxStaticText(this, wxID_ANY, "Start Time"), wxGBPosition(row, 0), wxGBSpan(1, 1), wxALL, 5);
	main_sizer->Add(startTimeText, wxGBPosition(row, 1), wxGBSpan(1, 1), wxALL, 5);
	main_sizer->Add(new wxStaticText(this, wxID_ANY, "End Time"), wxGBPosition(row, 2), wxGBSpan(1, 1), wxALL, 5);
	main_sizer->Add(endTimeText, wxGBPosition(row, 3), wxGBSpan(1, 1), wxALL, 5);

	row += 1;

	main_sizer->Add(new wxStaticText(this, wxID_ANY, "Error Code"), wxGBPosition(row, 0), wxGBSpan(1, 1), wxALL, 5);
	main_sizer->Add(errorCodeText, wxGBPosition(row, 1), wxGBSpan(1, 1), wxALL, 5);
	main_sizer->Add(new wxStaticText(this, wxID_ANY, "HTTP Error Code"), wxGBPosition(row, 2), wxGBSpan(1, 1), wxALL, 5);
	main_sizer->Add(httpErrorCodeText, wxGBPosition(row, 3), wxGBSpan(1, 1), wxALL, 5);

	row += 1;

	main_sizer->Add(new wxStaticText(this, wxID_ANY, "Request"), wxGBPosition(row, 0), wxGBSpan(1, 1), wxALL, 5);
	main_sizer->Add(requestText, wxGBPosition(row, 1), wxGBSpan(1, 3), wxALL, 5);

	row += 1;

	main_sizer->Add(new wxStaticText(this, wxID_ANY, "Response"), wxGBPosition(row, 0), wxGBSpan(1, 1), wxALL, 5);
	main_sizer->Add(responseText, wxGBPosition(row, 1), wxGBSpan(1, 3), wxALL, 5);

	Bind(wxEVT_CLOSE_WINDOW, &DockerInteractionsWindow::HandleCloseEvent, this);
	Bind(DOCKER_INTERACTION_RESULT_EVENT, &DockerInteractionsWindow::HandleDockerResultEvent, this, 0);
	Bind(wxEVT_LISTBOX, &DockerInteractionsWindow::HandleSelectionEvent, this);

	SetSizerAndFit(main_sizer);
	SetAutoLayout(true);
}

void DockerInteractionsWindow::HandleCloseEvent(wxCloseEvent& event) {
	Hide();
	wxCommandEvent event_to_parent(DOCKER_INTERACTION_HIDE_EVENT);
	GetParent()->GetEventHandler()->AddPendingEvent(event_to_parent);
}

void DockerInteractionsWindow::SetValuesForSelection() {
	int sel = interactionsList->GetSelection();
	docker_result* res = (docker_result*)arraylist_get(resList, sel);
	methodText->SetValue(res->method);
	urlText->SetValue(res->url);
	startTimeText->SetValue((new wxDateTime(res->start_time))->Format());
	endTimeText->SetValue((new wxDateTime(res->end_time))->Format());
	errorCodeText->SetValue(wxString::Format(wxT("%d"), (int)(res->error_code)));
	httpErrorCodeText->SetValue(wxString::Format(wxT("%ld"), res->http_error_code));
	requestText->SetValue(res->request_json_str);
	responseText->SetValue(res->response_json_str);
}

void DockerInteractionsWindow::HandleDockerResultEvent(wxCommandEvent& event) {
	docker_result* res = (docker_result*)event.GetClientData();
	size_t len = arraylist_length(resList);
	while (len >= limit) {
		docker_result* dropped_res = (docker_result*)arraylist_get(resList, 0);
		for (size_t i = 0; i < (len - 1); i++) {
			arraylist_set(resList, i, (docker_result*)arraylist_get(resList, i + 1));
		}
		arraylist_delete(resList, len - 1);
		len = arraylist_length(resList);
		free_docker_result(dropped_res);
		interactionsList->Delete(0);
	}
	if (res != NULL && res->url != NULL) {
		interactionsList->AppendString(wxString::Format(wxT("Request %s"), res->url));
		arraylist_add(resList, res);
		interactionsList->SetSelection(arraylist_length(resList) - 1);
		SetValuesForSelection();
	}
}

void DockerInteractionsWindow::HandleSelectionEvent(wxCommandEvent& event) {
	SetValuesForSelection();
}
