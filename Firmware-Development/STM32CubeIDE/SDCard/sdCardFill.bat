@echo off
setlocal enabledelayedexpansion

fsutil file createnew E:\file1.bin 4096000000
fsutil file createnew E:\file2.bin 4096000000
fsutil file createnew E:\file3.bin 4096000000
fsutil file createnew E:\file4.bin 2048000000
fsutil file createnew E:\file5.bin 1610612736
fsutil file createnew E:\file6.bin 157286400

echo Done!
pause
