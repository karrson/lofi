shopt -s extglob
shopt -s globstar

pushd kernel
sudo make -k
sudo make -k clean
popd

rm -f root/NvVars

sudo mkdir -p ./OS-boot
sudo rm -rf ./OS-boot
sudo mkdir -p ./OS-boot
sudo cp -r ./root/* ./OS-boot

sudo truncate OS-boot.img --size 512M
sudo mkfs.vfat -F 32 -n ROOT OS-boot.img
sudo mkdir -p ./OS-boot /mnt/OS-boot
sudo mount OS-boot.img /mnt/OS-boot
sudo cp -r ./OS-boot /mnt
sudo umount OS-boot.img
sudo rm -rf /mnt/OS-boot

sudo mv OS-boot.img OS.img
sudo rm -rf OS-boot