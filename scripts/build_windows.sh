#!/bin/bash
echo -e "\033[0;32mHow many CPU cores do you want to be used in compiling process? (Default is 1. Press enter for default.)\033[0m"
read -e CPU_CORES
if [ -z "$CPU_CORES" ]
then
	CPU_CORES=1
fi

# Upgrade the system and install required dependencies
	sudo apt update
	sudo apt install git zip unzip build-essential libtool bsdmainutils autotools-dev autoconf pkg-config automake python3 curl g++-mingw-w64-x86-64 libqt5svg5-dev -y
	echo "1" | sudo update-alternatives --config x86_64-w64-mingw32-g++

  # Clone MCPC code from deviant official Github repository
  	git clone https://github.com/Deviantcoin/Wallet-4.1.x deviant

  # Entering deviant directory
  	cd deviant

# Compile dependencies
	cd depends
	chmod +x config.sub
	chmod +x config.guess
	make -j$(echo $CPU_CORES) HOST=x86_64-w64-mingw32
	cd ..

# Compile deviant components eg daemon, cli, tx & qt
	chmod +x share/genbuild.sh
	chmod +x autogen.sh
	./autogen.sh
	./configure --prefix=$(pwd)/depends/x86_64-w64-mingw32 --disable-debug --disable-tests --disable-bench --disable-online-rust CFLAGS="-O3" CXXFLAGS="-O3"
	make -j$(echo $CPU_CORES) HOST=x86_64-w64-mingw32
	cd ..

# Create zip file of binaries
# Create zip file of binaries
cp deviant/src/deviantd.exe deviant/src/deviant-cli.exe deviant/src/deviant-tx.exe deviant/src/qt/deviant-qt.exe .
zip deviant-Linux.zip deviantd.exe deviant-cli.exe deviant-tx.exe deviant-qt.exe
rm -f deviantd.exe deviant-cli.exe deviant-tx.exe deviant-qt.exe
