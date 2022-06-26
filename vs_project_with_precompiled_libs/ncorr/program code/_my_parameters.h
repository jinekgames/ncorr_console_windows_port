#pragma once

#include <ctime>
#include <string>

#ifndef _MY_PARAMETERS_H

#define _MY_PARAMETERS_H



#define raise(condition, error) \
	if ( !(condition) ) {           \
		throw error;            \
	}

size_t CountOfSymbols(int number) {
	char buf[10];
	return strlen(itoa(number, buf, 10));
}



#define INTRO_MESSAGE \
"In command line arguements you should specify task you need to be done 'load' or 'calculate'\n\
- load\n\
  loads calculated bins and make plots in .avi files or logs\n\
  arguements:  -p  path to the folder with bins (.avi would be saved there)\n\
- calculate\n\
  read images and make DIC analisys, then save bins and .avi plots or logs of strains (exx, eyy, exy) and displacemants (u, v)\n\
  arguements:  -p, -n, -c, -f (optional), -s (optional), -m (optional)\n\
  images must be amed the by same name with number (from 0 to count-1) using 0 fills\n\
  example:\n\
     u have:  path\\image00.png .. path\\image99.png\n\
     ur arguements:  -p path -n image -c 100 -f tif\n\
  if images are not png u can specify format in the -f arg\n\
  -s specifies step between images to analyse (by default if reads every file, but u can ask to read only every 10th image or smth\n\
  -m unlocks multitheadig calculations, specify count of threads (TODO)\n\
  in folder u should have at least 2 images for analysis, where 0 is reference and additional roi.png which is the mask\n\
\n\
both comands can be run with optional arguements\n\
--plot\n\
   int from 0 to 3, responsible for items displaying on the plot (by default 0)\n\
--fps\n\
   fps in video (by default 15)\n\
--alpha\n\
   transperency of plot (0 - full transperent to 1 - not transperent) (by default 0.5)\n\
--mode\n\
   output type: 1 - only logs, 2 - only .avi's, not specified - all\n\
--pointi\n\
   if use logs mode or all mode u can specify index of dot u want to get strain values on (index = y*width + x, width u can get every time run in logs or all mode)\n\
\n\
arguements can be written in random order\n\
good luck\n\n"




#endif // !_MY_PARAMETERS_H
