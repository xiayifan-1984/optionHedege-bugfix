#!/usr/bin/bash
if [ ! -d "bin" ]; then
  mkdir bin
  chmod 777 bin
fi
export CC=/usr/local/gcc-8.2.0/bin/gcc
export CXX=/usr/local/gcc-8.2.0/bin/g++
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH
echo $LD_LIBRARY_PATH
rm -rf build
mkdir build
chmod 777 build
cd build
mkdir journal
chmod 777 journal
mkdir socket
chmod 777 socket
mkdir journal/system
chmod 777 journal/system
mkdir journal/user
chmod 777 journal/user
mkdir journal/strategy
chmod 777 journal/strategy
mkdir journal/marketdata
chmod 777 journal/marketdata
mkdir journal/tradingdata
chmod 777 journal/tradingdata
cmake -DCMAKE_INSTALL_PREFIX=. ..
make 
make install
cp -f memoryManage/PageEngine/PageEngine ../bin
cp -f tradingEngines/TDEngine/TDEngine ../bin
cp -f tradingEngines/MDEngine/MDEngine ../bin
cp -f tradingEngines/DirectKafkaStrategy/directkafkastrategy ../bin
cp -f tradingEngines/ParkedKafkaStrategy/ParkedKafkaStrategy ../bin
cp -f tradingEngines/TwapKafkaStrategy/TwapKafkaStrategy ../bin
