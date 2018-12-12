#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <docker_containers.h>
#include <docker_connection_util.h>
#include <curl/curl.h>
#include <iup.h>
#include <iupcontrols.h>
#include <iupcbs.h>

DockerContainersList* list_containers() {
	curl_global_init(CURL_GLOBAL_ALL);

	printf("Docker containers list");

	DockerContainersList* containers = docker_containers_list();

	curl_global_cleanup();
	return containers;
}

Ihandle* create_matrix() {
	Ihandle* mat = IupMatrix(NULL);
	IupSetAttribute(mat, "NUMLIN", "20");
	IupSetAttribute(mat, "NUMCOL", "8");

	IupSetAttribute(mat, "0:0", "Id");
	IupSetAttribute(mat, "0:1", "Name");
	IupSetAttribute(mat, "0:2", "Image");
	IupSetAttribute(mat, "0:3", "Command");
	IupSetAttribute(mat, "0:4", "Public Port");
	IupSetAttribute(mat, "0:5", "Private Port");
	IupSetAttribute(mat, "0:6", "State");
	IupSetAttribute(mat, "0:7", "Status");

	IupSetAttribute(mat, "RESIZEMATRIX", "YES");
	IupSetAttribute(mat, "RESIZEDRAG", "Yes");
//
//	IupSetAttribute(mat, "FONT3:3", "Helvetica, 24");
//	IupSetAttribute(mat, "TOGGLECENTERED", "Yes");
//
////	IupSetAttribute(mat, "FRAMEVERTCOLOR1:2", "BGCOLOR");
////	IupSetAttribute(mat, "FRAMEHORIZCOLOR1:2", "0 0 255");
////	IupSetAttribute(mat, "FRAMEHORIZCOLOR1:3", "0 255 0");
////	IupSetAttribute(mat, "FRAMEVERTCOLOR2:2", "255 255 0");
////	IupSetAttribute(mat, "FRAMEVERTCOLOR*:4", "0 255 0");
////	IupSetAttribute(mat, "FRAMEVERTCOLOR*:5", "BGCOLOR");
//
	IupSetAttribute(mat, "NUMCOL_NOSCROLL", "1");
	IupSetAttribute(mat, "NOSCROLLASTITLE", "Yes");
//
	IupSetAttribute(mat, "NUMCOL_VISIBLE", "8");
	IupSetAttribute(mat, "NUMLIN_VISIBLE", "20");

	DockerContainersList* cl = list_containers();
	char*id = (char*)malloc(100 * sizeof(char));
	for (int i = 0; i < cl->num_containers; i++) {
		sprintf(id, "%d:0", i + 1);
		IupSetAttribute(mat, id, cl->containers[i]->id);
		sprintf(id, "%d:1", i + 1);
		IupSetAttribute(mat, id, cl->containers[i]->names[0]);
		sprintf(id, "%d:2", i + 1);
		IupSetAttribute(mat, id, cl->containers[i]->image);
		sprintf(id, "%d:3", i + 1);
		IupSetAttribute(mat, id, cl->containers[i]->command);
		sprintf(id, "%d:4", i + 1);
		IupSetInt(mat, id, cl->containers[i]->ports[0]->public_port);
		sprintf(id, "%d:5", i + 1);
		IupSetInt(mat, id, cl->containers[i]->ports[0]->private_port);
		sprintf(id, "%d:6", i + 1);
		IupSetAttribute(mat, id, cl->containers[i]->state);
		sprintf(id, "%d:7", i + 1);
		IupSetAttribute(mat, id, cl->containers[i]->status);
	}
	free(id);

	return mat;
}

int main(int argc, char **argv) {
	Ihandle *dlg, *vbox, *mat;

	IupOpen(&argc, &argv);
	IupControlsOpen();

	mat = create_matrix();

	vbox = IupVbox(mat, NULL);
	IupSetAttribute(vbox, "MARGIN", "10x10");
	IupSetAttribute(vbox, "GAP", "10");
	IupSetAttribute(vbox, "ALIGNMENT", "ACENTER");
	dlg = IupDialog(vbox);
	IupSetAttribute(dlg, "TITLE", "V-Rex: Docker UI");

	IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
	IupMainLoop();
	IupClose();
	return EXIT_SUCCESS;
}
