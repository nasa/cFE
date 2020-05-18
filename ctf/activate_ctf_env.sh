#!/bin/bash

export INSTALL_DIR=`pwd`
export ANACONDA_VERSION='anaconda3'

platform='unknown'
unamestr=`uname`
if [[ "$unamestr" == 'Linux' ]]; then
   platform='linux'
elif [[ "$unamestr" == 'Darwin' ]]; then
   platform='darwin'
else
   platform='windows'
fi

# See if conda is already setup on this system
if which conda; then
    if conda env list | grep pythonEnv3; then
        echo 'activate!'
	    if [[ "$platform" == "windows" ]]; then
		source $INSTALL_DIR/$ANACONDA_VERSION/Scripts/activate pythonEnv3
	    else
		source $INSTALL_DIR/$ANACONDA_VERSION/bin/activate pythonEnv3
	    fi
    else
        if [[ "$platform" == 'darwin' ]]; then
         $INSTALL_DIR/$ANACONDA_VERSION/bin/conda env create -n pythonEnv3 -f=$INSTALL_DIR/pythonEnvironmentMac3.yml
        elif [[ "$platform" == 'linux' ]]; then
        if [[ "$unamemstr" == 'i686' ]]; then
        $INSTALL_DIR/$ANACONDA_VERSION/bin/conda env create -n pythonEnv3 -f=$TOOLS_DIR/pythonEnvironmentLinuxX86.yml
        else
        $INSTALL_DIR/$ANACONDA_VERSION/bin/conda env create -n pythonEnv3 -f=$TOOLS_DIR/pythonEnvironmentLinux3.yml
        fi
        else
         $INSTALL_DIR/$ANACONDA_VERSION/Scripts/conda env create -n pythonEnv3 -f=$INSTALL_DIR/pythonEnvironmentWindows3.yml
        fi
	    if [[ "$platform" == "windows" ]]; then
		source $INSTALL_DIR/$ANACONDA_VERSION/Scripts/activate pythonEnv3
	    else
		source $INSTALL_DIR/$ANACONDA_VERSION/bin/activate pythonEnv3
	    fi
    fi
# If not in system, check if it's installed already
elif [ -d "$INSTALL_DIR/$ANACONDA_VERSION" ]; then
    # Install anaconda
    if [ ! -d "$INSTALL_DIR/$ANACONDA_VERSION/envs/pythonEnv" ]; then
        if [[ "$platform" == 'darwin' ]]; then
         $INSTALL_DIR/$ANACONDA_VERSION/bin/conda env create -n pythonEnv -f=$INSTALL_DIR/pythonEnvironmentMac3.yml
        elif [[ "$platform" == 'linux' ]]; then
        if [[ "$unamemstr" == 'i686' ]]; then
        $INSTALL_DIR/$ANACONDA_VERSION/bin/conda env create -n pythonEnv3 -f=$TOOLS_DIR/pythonEnvironmentLinuxX86.yml
        else
        $INSTALL_DIR/$ANACONDA_VERSION/bin/conda env create -n pythonEnv3 -f=$TOOLS_DIR/pythonEnvironmentLinux3.yml
        fi
        else
         $INSTALL_DIR/$ANACONDA_VERSION/Scripts/conda env create -n pythonEnv -f=$INSTALL_DIR/pythonEnvironmentWindows3.yml
        fi
    fi
    if [[ "$platform" == "windows" ]]; then
    source $INSTALL_DIR/$ANACONDA_VERSION/Scripts/activate pythonEnv3
    else
        source $INSTALL_DIR/$ANACONDA_VERSION/bin/activate pythonEnv3
    fi

# If not installed on the system, install it in the home directory
else
    echo 'Anaconda not installed'
fi

cd $INSTALL_DIR

alias ctf="$INSTALL_DIR/ctf"
alias ctf_editor="(cd $INSTALL_DIR/tools/ctf_ui && npm start)"
