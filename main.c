#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <docker_containers.h>
#include <docker_connection_util.h>
#include <curl/curl.h>
#include <iup.h>

DockerContainersList* list_containers() {
	curl_global_init(CURL_GLOBAL_ALL);

	printf("Docker containers list");

	DockerContainersList* containers = docker_containers_list();

	curl_global_cleanup();
	return containers;
}

int main(int argc, char **argv) {
	Ihandle *dlg, *labelC, *labelF, *tempC, *tempF, *hbox, *list1;

	IupOpen(&argc, &argv);

//  labelC = IupLabel("Celsius =");
//  labelF = IupLabel("Fahrenheit");
//
//  tempC = IupText(NULL);
//  IupSetAttribute(tempC, "SIZE", "50");
//  IupSetAttribute(tempC, "NAME", "CELCIUS");
//  IupSetAttribute(tempC, "MASK", IUP_MASK_FLOAT);
//  tempF = IupText(NULL);
//  IupSetAttribute(tempF, "SIZE", "50");
//  IupSetAttribute(tempF, "NAME", "FAHRENHEIT");
//  IupSetAttribute(tempF, "MASK", IUP_MASK_FLOAT);

	list1 = IupList(NULL);
	IupSetAttributes(list1,
			"MULTIPLE=YES, TIP=Edit+Drop, VALUE=\"Edit Here\", NAME=list1");
	//IupSetStrAttribute(list1, "1", "5000");

	DockerContainersList* cl = list_containers();
	for(int i = 0; i < cl->num_containers; i++) {
		char*id = (char*)malloc(10*sizeof(char));
		sprintf(id, "%d", i+1);
		IupSetStrf(list1, id,  cl->containers[i]->id);
	}

	hbox = IupHbox(
//    tempC,
//    labelC,
//    tempF,
//    labelF,
			list1,
			NULL);
	IupSetAttribute(hbox, "MARGIN", "10x10");
	IupSetAttribute(hbox, "GAP", "10");
	IupSetAttribute(hbox, "ALIGNMENT", "ACENTER");
	dlg = IupDialog(hbox);
	IupSetAttribute(dlg, "TITLE", "TempConv");

//  IupSetAttribute(tempC, "VALUE", "");
//  IupSetAttribute(tempF, "VALUE", "");
//
//  /* Registers callbacks */
//  IupSetCallback(tempC, "VALUECHANGED_CB", (Icallback)txt_celcius_cb);
//  IupSetCallback(tempF, "VALUECHANGED_CB", (Icallback)txt_fahrenheit_cb);

	IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

	IupMainLoop();

	IupClose();
	return EXIT_SUCCESS;
}
