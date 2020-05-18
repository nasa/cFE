#!/bin/bash
#replaced \r with \n
updateValue=0
ymlValue=0

while getopts "uy" option
do
 case "${option}"
 in
 y)         ymlValue=1;;
 u)         updateValue=1;;

esac
done

export INSTALL_DIR=`pwd`

export ANACONDA_VERSION='anaconda3'

platform='unknown'
unamestr=`uname`
unamemstr=`uname -m`
if [[ "$unamestr" == 'Linux' ]]; then
   platform='linux'
elif [[ "$unamestr" == 'Darwin' ]]; then
   platform='darwin'
else
   platform='windows'
fi


if [ "$updateValue" -eq 1 ]; then
  rm -rf $INSTALL_DIR/anaconda3
fi

if [ "$ymlValue" -eq 1 ]; then
echo 'Generating yml file'
if [[ "$platform" == 'darwin' ]]; then
 $INSTALL_DIR/$ANACONDA_VERSION/bin/conda env export > $INSTALL_DIR/pythonEnvironmentMac3.yml
elif [[ "$platform" == 'linux' ]]; then
 if [[ "$unamemstr" == 'i686' ]]; then
 $INSTALL_DIR/$ANACONDA_VERSION/bin/conda env export > $INSTALL_DIR/pythonEnvironmentLinuxX86.yml
else
 $INSTALL_DIR/$ANACONDA_VERSION/bin/conda env export > $INSTALL_DIR/pythonEnvironmentLinux3.yml
fi
else
 $INSTALL_DIR/$ANACONDA_VERSION/Scripts/conda env export > $INSTALL_DIR/pythonEnvironmentWindows3.yml
fi
else
if [ -d "$INSTALL_DIR/anaconda3" ]; then
if [ ! -d "$INSTALL_DIR/$ANACONDA_VERSION/envs/pythonEnv3" ]; then
if [[ "$platform" == 'darwin' ]]; then
 $INSTALL_DIR/$ANACONDA_VERSION/bin/conda env create -n pythonEnv3 -f=$INSTALL_DIR/pythonEnvironmentMac3.yml
elif [[ "$platform" == 'linux' ]]; then
 if [[ "$unamemstr" == 'i686' ]]; then
 $INSTALL_DIR/$ANACONDA_VERSION/bin/conda env create -n pythonEnv -f=$INSTALL_DIR/pythonEnvironmentLinuxX86.yml
else
 $INSTALL_DIR/$ANACONDA_VERSION/bin/conda env create -n pythonEnv -f=$INSTALL_DIR/pythonEnvironmentLinux3.yml
fi
else
 $INSTALL_DIR/$ANACONDA_VERSION/Scripts/conda env create -n pythonEnv3 -f=$INSTALL_DIR/pythonEnvironmentWindows3.yml
fi
else
echo 'Anaconda3 already installed.'
echo 'execute "source pythonEnvInstall3.sh -u" to update.'
fi
else
if [[ "$platform" == 'darwin' ]]; then
 curl https://repo.anaconda.com/archive/Anaconda3-2018.12-MacOSX-x86_64.sh -o Anaconda3-2018.12-MacOSX-x86_64.sh
 res=$?
 chmod 700 "Anaconda3-2018.12-MacOSX-x86_64.sh"
 ./Anaconda3-2018.12-MacOSX-x86_64.sh -b -p $INSTALL_DIR/$ANACONDA_VERSION
 rm ./Anaconda3-2018.12-MacOSX-x86_64.sh
elif  [[ "$platform" == 'linux' ]]; then
 if [[ "$unamemstr" == 'i686' ]]; then
 curl https://repo.anaconda.com/archive/Anaconda3-2018.12-Linux-x86.sh -o Anaconda3-2018.12-Linux-x86.sh
 res=$?
 chmod 700 "Anaconda3-2018.12-Linux-x86.sh"
 ./Anaconda3-2018.12-Linux-x86.sh -b -p $INSTALL_DIR/$ANACONDA_VERSION
 rm ./Anaconda3-2018.12-Linux-x86.sh
else
 curl https://repo.anaconda.com/archive/Anaconda3-2018.12-Linux-x86_64.sh -o Anaconda3-2018.12-Linux-x86_64.sh
 res=$?
 chmod 700 "Anaconda3-2018.12-Linux-x86_64.sh"
 ./Anaconda3-2018.12-Linux-x86_64.sh -b -p $INSTALL_DIR/$ANACONDA_VERSION
 rm ./Anaconda3-2018.12-Linux-x86_64.sh
fi
else
curl https://repo.anaconda.com/archive/Anaconda3-2018.12-Windows-x86_64.exe -o Anaconda3-2018.12-Windows-x86_64.exe
res=$?
 chmod 700 "Anaconda3-2018.12-Windows-x86_64.exe"
 ./Anaconda3-2018.12-Windows-x86_64.exe /S /D=$INSTALL_DIR/$ANACONDA_VERSION
 rm ./Anaconda3-2018.12-Windows-x86_64.exe
fi
if [[ "$res" == '0' ]]; then
$INSTALL_DIR/$ANACONDA_VERSION/bin/conda create --name pythonEnv3 -y

$INSTALL_DIR/$ANACONDA_VERSION/bin/conda update -n base -c defaults conda -y

source $INSTALL_DIR/$ANACONDA_VERSION/bin/activate pythonEnv3

conda install -c anaconda bcrypt -y
conda install -c anaconda cffi -y
conda install -c anaconda cryptography -y
conda install -c anaconda fabric -y
conda install -c conda-forge ftputil -y
conda install -c conda-forge invoke -y
conda install -c anaconda paramiko -y
conda install -c anaconda psutil -y
conda install -c conda-forge pycparser -y
conda install -c conda-forge PyNaCl -y
conda install -c anaconda six -y
conda install -c conda-forge pyelftools -y


if  [[ "$platform" == 'linux' ]]; then
conda install -c anaconda nodejs -y
if [[ "$unamemstr" == 'i686' ]]; then
conda install -c anaconda libxscrnsaver-cos6-i686 -y
conda install -c anaconda nodejs -y
else
conda install -c anaconda libxscrnsaver-devel-cos6-x86_64 -y
conda install -c conda-forge nodejs -y
fi
else
conda install -c conda-forge nodejs -y
fi

if [[ "$platform" == 'darwin' ]]; then
conda env export > $INSTALL_DIR/pythonEnvironmentMac3.yml
elif [[ "$platform" == 'linux' ]]; then
 if [[ "$unamemstr" == 'i686' ]]; then
conda env export > $INSTALL_DIR/pythonEnvironmentLinuxX86.yml
else
conda env export > $INSTALL_DIR/pythonEnvironmentLinux3.yml
fi
else
 $INSTALL_DIR/$ANACONDA_VERSION/Scripts/conda env export > $INSTALL_DIR/pythonEnvironmentWindows3.yml
fi

# See if python2 is already setup on this system
if which python2; then
echo "python2 exist"
elif which python2.7; then
export PYTHON_LOCATION=`which python2.7`
ln -sf "$PYTHON_LOCATION" $INSTALL_DIR/$ANACONDA_VERSION/envs/pythonEnv3/bin/python2
else
echo "python2.7 does not exist required for npm install"
fi

cd $INSTALL_DIR
else
echo 'Down load of Anaconda install script failed:'
echo $res
fi
fi
fi

cd tools/ctf_ui
npm install
cd $INSTALL_DIR