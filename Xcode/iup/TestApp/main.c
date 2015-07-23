//
//  main.m
//  TestApp
//
//  Created by Eric Wing on 7/22/15.
//  Copyright (c) 2015 Tecgraf, PUC-Rio, Brazil. All rights reserved.
//

#include "iup.h"
#include <stdlib.h>

int main(int argc, const char* argv[])
{

	IupOpen(&argc, &argv);
	Ihandle* dialog = IupDialog(NULL);

	IupShowXY(dialog, IUP_CENTER, IUP_CENTER);
	
	IupMainLoop();
	IupClose();
	
	return 0;
}
