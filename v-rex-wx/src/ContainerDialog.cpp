#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dialog.h>

#include "ContainerDialog.h"

//Taken from wxwidgets dialog samples
class EnterHandlingTextCtrl : public wxTextCtrl
{
public:
	EnterHandlingTextCtrl(wxWindow* parent, int id, const wxString& value)
		: wxTextCtrl(parent, id, value,
			wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER)
	{
		Bind(wxEVT_TEXT_ENTER, &EnterHandlingTextCtrl::OnEnter, this);

		SetInitialSize(GetSizeFromTextSize(GetTextExtent(value).x));
	}

private:
	void OnEnter(wxCommandEvent& WXUNUSED(event))
	{
		wxLogMessage("Enter pressed");
	}
};


ContainerDialog::ContainerDialog(const wxString& title)
	: wxDialog(NULL, -1, title, wxDefaultPosition, wxSize(250, 230)) {

	wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
	wxFlexGridSizer* grid_sizer = new wxFlexGridSizer(2);

	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Name"));
	grid_sizer->Add(new EnterHandlingTextCtrl(this, wxID_ANY, "container0"));
	grid_sizer->Add(new wxStaticText(this, wxID_ANY, "Repository"));
	grid_sizer->Add(new EnterHandlingTextCtrl(this, wxID_ANY, "DockerHub"));

	main_sizer->Add(grid_sizer, wxSizerFlags().DoubleBorder());

	wxSizer* button_sizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	if (button_sizer)
		main_sizer->Add(button_sizer, wxSizerFlags().Expand().Border());

	SetSizerAndFit(main_sizer);
}
