# Ncorr console windows port
**This is the port of Ncorr BIC-analysis library converted to console application which can be run with parameters**

This program is based on C++ port of [Ncorr library](https://www.ncorr.com/index.php/c-port "Ncorr official website")
> Ncorr: open-source 2D digital image correlation matlab software.
> J Blaber, B Adair, A Antoniou.
> Experimental Mechanics 55 (6), 1105-1122


## VS solution

In folder `vs_project_with_precompiled_libs/` u can find C++ VisualStudio solution with precompiled external libraries ([Ncorr](https://github.com/justinblaber/ncorr_2D_cpp "Ncorr repository") v1.0.0, [OpenCV](https://opencv.org "OpenCV website") v4.5.5, [FFTW](http://fftw.org/ "FFTW website") v3.3.10 and [SuiteSparse with Metis](https://github.com/jlblancoc/suitesparse-metis-for-windows "SuiteSparse and Metis port repository") v1.5.0).
My own code is located in `vs_project_with_precompiled_libs/ncorr/program code/`

Solution will generate executable in `compiled_program/`. Here u can also find executable but better download it from link in the next paragraph

**It is highly recomended to build only Release x64**, otherwise u will have to solve some problems with vs parameters to tun the build


## Compiled program

Compiled executable can be found in `compiled_program/`. But better **u can download it from** [release page](https://github.com/jinekgames/ncorr_console_windows_port/releases/tag/v0.1 "download from github")

**To run it you are to copy ncorr.exe and also all the dll's located in the folder** (they are opencv dll's).
This is the console application which means you should run cmd.exe, cd to the exe directory and run it with its name
Program will console log which arguments u need or can add to the command

Sample data for test and txt with sample command can be found in `compiled_program/test_sample/`


## Contacts

For any questions use this e-mail: pussy_hunt3r@vk.com
Or any other contanct u have found

Thank you for opening this page, i really appreciate it

Have a nice day
***
***Eugene Kalinin***, ITMM, 2022
