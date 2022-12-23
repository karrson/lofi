wsl ./build.sh
pause
qemu-system-x86_64 -m 4G -vga std --no-reboot --no-shutdown ^
-monitor stdio -D log.txt -d int -M smm=off -machine q35 ^
-bios qemu-efi/OVMF.fd -drive file=OS.img ^
-cpu qemu64 -net none
pause