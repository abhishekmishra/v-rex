#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dialog.h>

#include "ContainerDialog.h"

ContainerDialog::ContainerDialog(const wxString& title)
	: wxDialog(NULL, -1, title, wxDefaultPosition, wxSize(250, 230)) {

}
