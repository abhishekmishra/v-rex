#include <wx/commandlinkbutton.h>
#include "DashboardWindow.h"

/*
* Using colour palette at https://coolors.co/e63946-f1faee-a8dadc-457b9d-1d3557
* Desire (reddish) -> #e63946
* Honeydew (background) -> #f1faee
* Light blue -> #a8dadc
* Queen blue -> #457b9d
* Space cadet -> #1d3557
*/

const char* dash_html =
"<html>"
"<head></head>"
"<body  bgcolor=\"#f1faee\">"
"<div align=\"center\"><h1>V-Rex Dashboard</h1></div>"
"<div align=\"center\">"
"<table width=\"80%\" align=\"center\" border=\"0.5\">"
"<tr>"
"<td width=\"25%\"><span style=\"color: #457b9d\">%d Containers [%d Running, %d Stopped, %d Paused]</span></td>"
"<td width=\"25%\"><span style=\"color: #457b9d\">%d Images</span></td>"
"<td width=\"25%\"><span style=\"color: #457b9d\">%d Volumes</span></td>"
"<td width=\"25%\"><span style=\"color: #457b9d\">%d Networks</span></td>"
"</tr>"
"</table>"
"</div>"
"</body>";

DashboardWindow::DashboardWindow(VRexContext* ctx, wxWindow* parent)
	:wxPanel(parent) {
	this->ctx = ctx;

	Bind(DOCKER_CONNECT_EVENT, &DashboardWindow::HandleDockerConnect, this, 0);

	dashSizer = new wxFlexGridSizer(3);
	containersBtn = new wxCommandLinkButton(this, wxID_ANY, "Containers");
	imagesBtn = new wxCommandLinkButton(this, wxID_ANY, "Images");
	volumesBtn = new wxCommandLinkButton(this, wxID_ANY, "Volumes");
	networksBtn = new wxCommandLinkButton(this, wxID_ANY, "Networks");

	dashSizer->Add(
		containersBtn,
		1,            // make vertically stretchable
		wxEXPAND |    // make horizontally stretchable
		wxALL,        //   and make border all around
		10);         // set border width to 10
	dashSizer->Add(
		imagesBtn,
		1,            // make vertically stretchable
		wxEXPAND |    // make horizontally stretchable
		wxALL,        //   and make border all around
		10);         // set border width to 10
	dashSizer->Add(
		volumesBtn,
		1,            // make vertically stretchable
		wxEXPAND |    // make horizontally stretchable
		wxALL,        //   and make border all around
		10);         // set border width to 10
	dashSizer->Add(
		networksBtn,
		1,            // make vertically stretchable
		wxEXPAND |    // make horizontally stretchable
		wxALL,        //   and make border all around
		10);         // set border width to 10

	SetSizerAndFit(dashSizer);
}

void DashboardWindow::HandleDockerConnect(wxCommandEvent& event) {
	docker_info* info = this->ctx->getDockerInfo();

	wxString containersNote;
	containersNote.Printf(
		wxT("Total:%lu Running:%lu Paused:%lu Stopped:%lu"),
		info->containers,
		info->containers_running,
		info->containers_paused,
		info->containers_stopped);
	containersBtn->SetNote(containersNote);

	wxString imagesNote;
	imagesNote.Printf(
		wxT("Total:%lu"),
		info->images);
	imagesBtn->SetNote(imagesNote);

	Fit();
}