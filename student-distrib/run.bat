:: Run this on windows to open the test_debug
c:
cd "c:\qemu-1.5.0-win32-sdl\"
qemu-system-i386w.exe -hda Z:\_lacosintosshlerant\student-distrib\mp3.img -m 256 -gdb tcp:127.0.0.1:1234 -S -name mp3