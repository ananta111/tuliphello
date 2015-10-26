# Prerequistes:
* Tulip Installed (we suggest installing from source if possible)
  * http://tulip.labri.fr/Documentation/current/tulip-dev/html/installation.html
* libibautils 
  * https://github.com/nateucar/libibautils
* Google's RE2
  * https://github.com/google/re2
* cmake
* c++ compiler
* all prerequistes for Tulip

# Reference

http://tulip.labri.fr/Documentation/current/tulip-dev/html/tulip_tutorial.html

# Install Procedure
* Use Cmake to configure
```
cmake -DCMAKE_MODULE_PATH="<path to Tulip install/share/tulip directory>;<path to Infiniband directory>" -DCMAKE_BUILD_TYPE=Release <path to Infiniband directory>
```
* Compile and install
```
make
make install
```

# Example: Compelete install on a Linux host including Tulip
* Compile and install Tulip (This assumes that all the Tulip prereqs are already installed).
```
svn checkout svn://svn.code.sf.net/p/auber/code/tulip tulip-src
cd tulip-src
mkdir build
cd build
cmake ..
make
sudo make install
```
*  Compile and install Googles RE2
```
git clone https://github.com/google/re2.git re2
cd re2
cmake .
make
sudo make install
```
* Compile and install libibautils
```
git clone https://github.com/nateucar/libibautils.git libibautils
mkdir libibautils/build
cd libibautils/build
cmake ..
make
sudo make install
```
* Compile and install Infinband Plugins into Tulip
git clone https://github.com/nateucar/tulip_infiniband.git tulip_infiniband
mkdir tulip_infiniband/build
cd tulip_infiniband/build
cmake -DCMAKE_MODULE_PATH=/usr/local/share/tulip/ ..
make
sudo make install
```
* Run Tulip (from Bash)
```
export PATH=/usr/local/bin/:$PATH LD_LIBRARY_PATH=/usr/local/lib/:$LD_LIBRARY_PATH
tulip
```
