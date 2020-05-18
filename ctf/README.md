### CFS Test Framework

This repository contains the scripts and plugins used to validate the cFE flight software.


### Getting Started

To get started, clone the CTF repository:

Steps TBD - CTF has not yet been open source released 

Note that the CTF repository must be cloned at the same directory level as the cFS repository.


### CTF Prerequisites

CTF has been developed and tested on Linux (CentOS 6 and 7) and requires Python 3.x. CTF_UI requires an insulation of NodeJS/NPM.


### Setup CTF enviroment

To setup ctf environment execute "source setup\_ctf\_env.sh"

If the Anaconda environment is corrupted, the environment can be reinstalled by executing "source setup\_ctf\_env.sh -u"


### Starting the editor

./start_editor.sh


### Running a test script

execute ./run_CTF_script followed by the name of the associated test script contained in the /scripts directory

Example:
./run_CTF_script.sh cFE_TBL_InvalidCommandCounter.json

Note that if no test script is passed into run_CTF_script.sh, it will execute all of the test scripts contained within /scripts 






