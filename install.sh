#!/bin/bash

git clone https://github.com/sna1lBoy/audioShare.git audioShare > /dev/null 2>&1
cd audioShare

sudo mv ./bin/audioShare /bin/audioShare
sudo chmod +x /bin/audioShare
sudo mkdir /etc/audioShare/
sudo mv ./resources/config.ini /etc/audioShare/config.ini
sudo mv ./resources/icon.png /etc/audioShare/icon.png

cd ..
rm -rf audioShare

echo "audio share is now installed! happy screen sharing! :)" 