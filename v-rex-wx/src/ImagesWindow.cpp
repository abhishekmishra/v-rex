#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/panel.h>
#include <wx/toolbar.h>
#include <wx/artprov.h>
#include <wx/grid.h>

#include <docker_all.h>
#include <arraylist.h>

#include "ImagesWindow.h"
#include "VRexContext.h"
#include "VRexUtil.h"

#define VREX_IMAGES_TOOL_REFRESH		301

wxDEFINE_EVENT(LIST_IMAGES_EVENT, wxCommandEvent);

// a thread class that will periodically send events to the GUI thread
class ListImagesThread : public wxThread
{
	wxPanel* m_parent;
	VRexContext* ctx;

public:
	ListImagesThread(wxPanel* parent, VRexContext* ctx)
	{
		m_parent = parent;
		this->ctx = ctx;
	}

	virtual ExitCode Entry();
};

wxThread::ExitCode ListImagesThread::Entry()
{
	docker_image_list* images;

	//Lookup images
	docker_images_list(this->ctx->getDockerContext(), &images, 0, 1, NULL, 0, NULL, NULL,
		NULL);

	// notify the main thread
	wxCommandEvent list_images_event(LIST_IMAGES_EVENT);
	list_images_event.SetClientData(images);
	m_parent->GetEventHandler()->AddPendingEvent(list_images_event);
	return 0;
}

ImagesWindow::ImagesWindow(VRexContext* ctx, wxWindow* parent)
	: wxPanel(parent) {
	this->ctx = ctx;

	//SetBackgroundColour(*(new wxColour("red")));

	Bind(DOCKER_CONNECT_EVENT, &ImagesWindow::HandleDockerConnect, this, 0);
	Bind(PAGE_REFRESH_EVENT, &ImagesWindow::HandlePageRefresh, this, 0);
	Bind(LIST_IMAGES_EVENT, &ImagesWindow::HandleListImages, this, 0);
	Bind(wxEVT_TOOL, &ImagesWindow::HandlePageRefresh, this, VREX_IMAGES_TOOL_REFRESH);

	imagesSizer = new wxFlexGridSizer(1);

	wxToolBar* toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_HORIZONTAL | wxTB_HORZ_TEXT);
	wxBitmap refresh = wxArtProvider::GetBitmap(wxART_REDO, wxART_TOOLBAR);
	wxBitmap show_run = wxArtProvider::GetBitmap(wxART_TICK_MARK, wxART_TOOLBAR);

	toolBar->AddTool(VREX_IMAGES_TOOL_REFRESH, "Refresh", refresh);

	toolBar->Realize();

	imageListGrid = new wxGrid(this,
		-1,
		wxPoint(0, 0));
	imageListGrid->CreateGrid(1, 3);
	imageListGrid->SetRowSize(0, 20);
	imageListGrid->SetColSize(0, 120);
	imageListGrid->SetColLabelValue(0, "Tags");
	imageListGrid->SetColLabelValue(1, "Size");
	imageListGrid->SetColLabelValue(2, "Virtual Size");
	imageListGrid->HideRowLabels();

	imagesSizer->Add(toolBar, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 5);
	imagesSizer->Add(imageListGrid, 0, wxALL | wxEXPAND, 5);

	if (this->ctx->isConnected()) {
		this->RefreshImages();
	}
	SetSizerAndFit(imagesSizer);
	SetAutoLayout(true);
}

void ImagesWindow::HandleDockerConnect(wxCommandEvent& event) {
	if (this->ctx->isConnected()) {
		this->RefreshImages();
	}
}

void ImagesWindow::HandleListImages(wxCommandEvent& event) {
	docker_image_list* images = (docker_image_list*)event.GetClientData();
	UpdateImages(images);
	ctx->DockerCallUpdate("Images list loaded.");
}

void ImagesWindow::RefreshImages() {
	if (this->ctx->isConnected()) {
		// create the thread
		ListImagesThread* t = new ListImagesThread(this, this->ctx);
		wxThreadError err = t->Create();

		if (err != wxTHREAD_NO_ERROR)
		{
			wxMessageBox(_("Couldn't create thread!"));
		}

		err = t->Run();

		if (err != wxTHREAD_NO_ERROR)
		{
			wxMessageBox(_("Couldn't run thread!"));
		}
	}
	else {
		//TODO: show error here - send to frame's status bar
	}
}

char* get_image_tags_concat(docker_image* img) {
	char* tags = NULL;
	if (docker_image_repo_tags_get(img)) {
		int len_tags = docker_image_repo_tags_length(img);
		int tag_strlen = 0;
		for (int i = 0; i < len_tags; i++) {
			tag_strlen += strlen((char*)docker_image_repo_tags_get_idx(img, i));
			tag_strlen += 1; //for newline
		}
		tag_strlen += 1; //for null terminator
		tags = (char*)calloc(tag_strlen, sizeof(char));
		if (tags != NULL) {
			tags[0] = '\0';
			for (int i = 0; i < len_tags; i++) {
				strcat(tags, (char*)docker_image_repo_tags_get_idx(img, i));
				if (i != (len_tags - 1)) {
					strcat(tags, "\n");
				}
			}
		}
	}
	return tags;
}


void ImagesWindow::UpdateImages(docker_image_list* images) {
	//Empty the grid by removing all current rows
	imageListGrid->DeleteRows(0, imageListGrid->GetNumberRows());

	int len_images = docker_image_list_length(images);

	//Add enough rows in the image grid
	imageListGrid->InsertRows(0, len_images);

	int col_num = 0, row_num = 0;
	for (int i = 0; i < len_images; i++) {
		docker_image* img = (docker_image*)docker_image_list_get_idx(images, i);
		col_num = 0;

		char* tags = get_image_tags_concat(img);
		imageListGrid->SetCellValue(row_num, col_num, tags);
		col_num += 1;

		char* size = calculate_size(docker_image_size_get(img));
		imageListGrid->SetCellValue(row_num, col_num, size);
		col_num += 1;

		char* virtual_size = calculate_size(docker_image_virtual_size_get(img));
		imageListGrid->SetCellValue(row_num, col_num, virtual_size);
		col_num += 1;

		free(size);
		free(virtual_size);
		free(tags);

		row_num += 1;
	}

	//TODO: fix free images
	//arraylist_free(images);

	imageListGrid->AutoSize();
	Layout();
}

void ImagesWindow::HandlePageRefresh(wxCommandEvent& event) {
	if (this->ctx->isConnected()) {
		this->RefreshImages();
	}
	if (event.GetId() == VREX_IMAGES_TOOL_REFRESH) {
		event.Skip();
	}
}

