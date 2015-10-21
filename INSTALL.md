# Prerequistes:
* Tulip Installed (we suggest installing from source if possible)
  * http://tulip.labri.fr/Documentation/current/tulip-dev/html/installation.html
* libibautils 
  * https://github.com/nateucar/libibautils
* cmake
* c++ compiler
* all prerequistes for Tulip

# Reference

http://tulip.labri.fr/Documentation/current/tulip-dev/html/tulip_tutorial.html

# Install Procedure
* Use Cmake to configure
```
cmake -DCMAKE_MODULE_PATH="<path to Tulip install/share/tulip directory>;<path to Infiniband directory>" -DRE2_INCLUDE_DIR=<path to re2 directory> -DRE2_LIBRARY=<path to libre2.so> -DCMAKE_BUILD_TYPE=Release <path to Infiniband directory>
```
* Compile and install
```
make
make install
```
