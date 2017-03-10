# Run using gdb -x test_os.sh
exec-file bootimg
target remote 10.0.2.2:1234
continue