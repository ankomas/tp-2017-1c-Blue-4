sudo ln -s /home/utnso/workspace/tp-2017-1c-Blue-4/commons/Debug/libcommons.so /usr/lib/libcommons.so
sudo ln -s /home/utnso/workspace/tp-2017-1c-Blue-4/blue4-lib/Debug/libblue4-lib.so /usr/lib/libblue4-lib.so
sudo ln -s /home/utnso/workspace/tp-2017-1c-Blue-4/parser/Debug/libparser.so /usr/lib/libparser.so
sudo ldconfig
export LD_LIBRARY_PATH=/home/utnso/workspace/tp-2017-1c-Blue-4/commons/Debug/:/home/utnso/workspace/tp-2017-1c-Blue-4/blue4-lib/Debug/:/home/utnso/workspace/tp-2017-1c-Blue-4/parser/Debug/