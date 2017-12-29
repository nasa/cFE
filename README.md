# cFE

This repository contains the code for cFE, NASA's core Flight Executive. It also contains a number of submodules including OSAL, example "lab" applications, and mission ready applications. Together, these repositories constitute the core Flight System (cFS). Please note that this configuration has not been verified as an operational system.

The official releases are tagged and assigned a version number. Applications (in the `apps` directory) that have the `_lab` postfix or `sample_` prefix are not intended to be used for flight.

## Intent

This repository is intend to:
* help foster a larger community of cFS users
* allow easier distribution of cFS
* provide a way for users to submit issues for suspected bugs

Active development of cFS is not performed with this repository. We are working on ways to improve communication between public users and developers (for now use GitHub's issue tracker here). We are also looking into ways to better keep the public abreast of cFS development.

## Getting Help

The cFS community page ([http://coreflightsystem.org](http://coreflightsystem.org)) should be your first stop for getting help. Please post questions to the [Q&A section](http://coreflightsystem.org/questions/). There is also a [forum](http://coreflightsystem.org/forums/) for more general discussions.

Official cFS page: [http://cfs.gsfc.nasa.gov](http://cfs.gsfc.nasa.gov)

## Setup

We suggest that users fork the repositories included here and modify as needed for their specific missions.

To setup cFS directly from the latest release:

    git clone https://github.com/nasa/cFE.git
    cd cFE
    git submodule init
    git submodule update

## Build and Run

The latest release include sample applications and an example build target that uses the pc-linux platform support package (should run on most Linux distributions).

To compile and run:

    . ./setvars
    cd build/cpu1
    make config
    make
    cd exe
    sudo ./core-linux.bin
