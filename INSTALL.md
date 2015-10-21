to configure type :

cmake -DCMAKE_MODULE_PATH="<path to Tulip install/share/tulip directory>;<path to Infiniband directory>" -DRE2_INCLUDE_DIR=<path to re2 directory> -DRE2_LIBRARY=<path to libre2.so> -DCMAKE_BUILD_TYPE=Release <path to Infiniband directory>
- then
make
make install
