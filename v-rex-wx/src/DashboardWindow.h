#pragma once

#include <wx/wx.h>
#include <wx/richtext/richtextctrl.h>

#include "VRexContext.h"

class DashboardWindow: public wxRichTextCtrl {
public:
    DashboardWindow(VRexContext* ctx, wxWindow* parent);
};