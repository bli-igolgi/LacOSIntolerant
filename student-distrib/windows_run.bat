:: Run this on windows to open the test_debug
c:
cd "C:\Users\Cody\Sync\My Documents\School\UIUC\ece391\qemu_win"
qemu-system-i386w.exe -hda "C:\Users\Cody\Sync\My Documents\School\UIUC\ece391\ece391_share\work\mp3\student-distrib\mp3.img" -m 256 -gdb tcp:127.0.0.1:1234 -S -name mp3