#include <wx/richtext/richtextctrl.h>

#include "DashboardWindow.h"

DashboardWindow::DashboardWindow(VRexContext* ctx, wxWindow* parent)
	:wxRichTextCtrl(parent) {
	this->WriteText("V-Rex Dashboard.");
}