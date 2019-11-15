#include "DockerInteractionsWindow.h"
#include <wx/gbsizer.h>


wxDEFINE_EVENT(DOCKER_INTERACTION_RESULT_EVENT, wxCommandEvent);

//d_err_t error_code;
//time_t start_time;
//time_t end_time;
//char* method;
//char* request_json_str;
//char* response_json_str;
//long http_error_code;
//char* url;
//char* message;

DockerInteractionsWindow::DockerInteractionsWindow(wxWindow* parent) 
	: wxFrame(parent, -1, "Docker Interactions Window") {
	wxGridBagSizer* main_sizer = new wxGridBagSizer();

	wxTextCtrl* methodText = new wxTextCtrl(this, wxID_ANY);
	wxTextCtrl* urlText = new wxTextCtrl(this, wxID_ANY);

	main_sizer->Add(new wxStaticText(this, wxID_ANY, "Method"), wxGBPosition(0, 0), wxGBSpan(1, 1));
	main_sizer->Add(methodText, wxGBPosition(0, 1), wxGBSpan(1, 1));
	main_sizer->Add(new wxStaticText(this, wxID_ANY, "URL"), wxGBPosition(0, 2), wxGBSpan(1, 1));
	main_sizer->Add(urlText, wxGBPosition(0, 3), wxGBSpan(1, 1));

	SetSizerAndFit(main_sizer);
}