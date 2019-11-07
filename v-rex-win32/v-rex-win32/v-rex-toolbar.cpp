#include "framework.h"
#include "commctrl.h"
#include "v-rex-toolbar.h"

HIMAGELIST g_hImageList = NULL;
extern HINSTANCE hInst;

HWND CreateSimpleToolbar(HWND hWndParent)
{
	// Declare and initialize local constants.
	const int ImageListID = 0;
	const int numButtons = 4;
	const int bitmapSize = 16;

	const DWORD buttonStyles = BTNS_AUTOSIZE;

	// Create the toolbar.
	HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | TBSTYLE_WRAPABLE, 0, 0, 0, 0,
		hWndParent, NULL, hInst, NULL);

	if (hWndToolbar == NULL)
		return NULL;

	// Create the image list.
	g_hImageList = ImageList_Create(bitmapSize, bitmapSize,   // Dimensions of individual bitmaps.
		ILC_COLOR16 | ILC_MASK,   // Ensures transparent background.
		numButtons, 0);

	// Set the image list.
	SendMessage(hWndToolbar, TB_SETIMAGELIST,
		(WPARAM)ImageListID,
		(LPARAM)g_hImageList);

	// Load the button images.
	SendMessage(hWndToolbar, TB_LOADIMAGES,
		(WPARAM)IDB_STD_SMALL_COLOR,
		(LPARAM)HINST_COMMCTRL);

	// Initialize button info.
	// IDM_NEW, IDM_OPEN, and IDM_SAVE are application-defined command constants.

	const int cx_edit = 100;      // Dimensions of edit control.
	const int cy_edit = 35;

	TBBUTTON tbButtons[numButtons] =
	{
		{cx_edit, 0, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, -1},
		{ MAKELONG(STD_FILENEW,  ImageListID), IDM_NEW,  TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"New" },
		{ MAKELONG(STD_FILEOPEN, ImageListID), IDM_OPEN, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"Open"},
		{ MAKELONG(STD_FILESAVE, ImageListID), IDM_SAVE, 0,               buttonStyles, {0}, 0, (INT_PTR)L"Save"}
	};

	// Add buttons.
	SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM)numButtons, (LPARAM)&tbButtons);

	// Create the edit control child window.    
	HWND hWndEdit = CreateWindowEx(0L, L"Edit", NULL,
		WS_CHILD | WS_BORDER | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
		0, 0, cx_edit, cy_edit,
		hWndToolbar, (HMENU)IDM_EDIT, hInst, 0);

	if (!hWndEdit)
	{
		DestroyWindow(hWndToolbar);
		ImageList_Destroy(g_hImageList);

		return NULL;
	}

	// Resize the toolbar, and then show it.
	SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
	ShowWindow(hWndToolbar, TRUE);

	return hWndToolbar;
}