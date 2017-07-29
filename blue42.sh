mkdir workspace
cd /home/utnso/workspace && git clone https://github.com/sisoputnfrba/tp-2017-1c-Blue-4.git
cd /home/utnso/workspace/tp-2017-1c-Blue-4/ && tar -xzvf SADICA_FS_V2.tar.gz
mv /home/utnso/workspace/tp-2017-1c-Blue-4/SADICA_FS /home/utnso/FS_SADICA
cd /home/utnso/workspace/tp-2017-1c-Blue-4/commons/Debug && make clean && make
cd /home/utnso/workspace/tp-2017-1c-Blue-4/blue4-lib/Debug && make clean && make
cd /home/utnso/workspace/tp-2017-1c-Blue-4/parser/Debug && make clean && make
cd /home/utnso/workspace/tp-2017-1c-Blue-4/consola/Debug && make && make clean && make
cd /home/utnso/workspace/tp-2017-1c-Blue-4/cpu/Debug && make && make clean && make
cd /home/utnso/workspace/tp-2017-1c-Blue-4/kernel/Debug && make && make clean && make
cd /home/utnso/workspace/tp-2017-1c-Blue-4/memoria/Debug && make && make clean && make
cd /home/utnso/workspace/tp-2017-1c-Blue-4/fileSystem/Debug && make && make clean && make
sudo ln -s /home/utnso/workspace/tp-2017-1c-Blue-4/commons/Debug/libcommons.so /usr/lib/libcommons.so
sudo ln -s /home/utnso/workspace/tp-2017-1c-Blue-4/blue4-lib/Debug/libblue4-lib.so /usr/lib/libblue4-lib.so
sudo ln -s /home/utnso/workspace/tp-2017-1c-Blue-4/parser/Debug/libparser.so /usr/lib/libparser.so
sudo ldconfig
export LD_LIBRARY_PATH=/home/utnso/workspace/tp-2017-1c-Blue-4/commons/Debug/:/home/utnso/workspace/tp-2017-1c-Blue-4/blue4-lib/Debug/:/home/utnso/workspace/tp-2017-1c-Blue-4/parser/Debug/
nano /home/utnso/workspace/tp-2017-1c-Blue-4/consola/configConsola.cfg
nano /home/utnso/workspace/tp-2017-1c-Blue-4/kernel/config.cfg
nano /home/utnso/workspace/tp-2017-1c-Blue-4/cpu/Debug/config.cfg
nano /home/utnso/workspace/tp-2017-1c-Blue-4/fileSystem/config.cfg
nano /home/utnso/workspace/tp-2017-1c-Blue-4/memoria/config.cfg 
