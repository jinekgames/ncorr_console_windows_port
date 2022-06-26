# Ncorr console windows port
**This is the port of Ncorr BIC-analyse library converted to console application which can be run with parameters**

This program is based on C++ port of [Ncorr library](https://www.ncorr.com/index.php/c-port "Ncorr official webpage")
> Ncorr: open-source 2D digital image correlation matlab software.
> J Blaber, B Adair, A Antoniou.
> Experimental Mechanics 55 (6), 1105-1122


## VS solution

In folder `vs_project_with_precompiled_libs/` u can find C++ VisualStudio solution with precompiled external libraries (Ncorr, OpenCV, FFTW and SuiteSparse).
My own code is located in `vs_project_with_precompiled_libs/ncorr/program code/`

Solution will generate executable in `compiled_program/`

**It is highly recomended to build only Release x64**, otherwise u will have to solve some problems with vs parameters to tun the build


## Compiled program

Compiled executable can be found in `compiled_program/`.
Also here is the [direct download link](https://github.com/jinekgames/ncorr_console_windows_port/raw/master/compiled_program/ncorr.exe "download from github")

**To run it you are to copy ncorr.exe and also all the dll's located in the folder** (they are opencv dll's).
This is the console application which means you should run cmd.exe, cd to the exe directory and run it with its name
Program will console log which arguements u need or can add to the command

Sample data for test and txt with sample command can be found in `compiled_program/test_sample/`


## Contacts

For any questions use this e-mail: pussy_hunt3r@vk.com
Or any other contanct u have found

Thank you for opening this page, i really appreciate it

Have a nice day
***
***Eugene Kalinin***, ITMM, 2022
