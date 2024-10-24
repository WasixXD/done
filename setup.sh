#!/bin/bash
# Thanks @erickwendel

sudo apt-get install -y ccache
sudo apt-get install -y cmake
sudo apt-get install -y pkg-config

git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
export PATH=`pwd`/depot_tools:$PATH

# Change this to match your shell .rc
echo "export PATH=`pwd`/depot_tools:\$PATH" >> ~/.bashrc

gclient
fetch v8
cd v8

tools/dev/v8gen.py x64.release
echo 'cc_wrapper="ccache"' >> out.gn/x64.release/args.gn 

export CCACHE_CPP2=yes
export CCACHE_SLOPPINESS=time_macros

echo "export CCACHE_CPP2=yes" >> ~/.bashrc
echo "export CCACHE_SLOPPINESS=time_macros" >> ~/.bashrc

ninja -C out.gn/x64.release/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/v8/out.gn/x64.release/
