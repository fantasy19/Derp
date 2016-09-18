CALL "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86
CALL "C:\MinGW\set_distro_paths.bat"
:START
DEL out_gnu.txt
DEL out_ms.txt

cl -Fems.exe driver-sample.cpp ObjectAllocator.cpp PRNG.cpp /EHa /MTd /W4 /WX /Za /D_CRT_SECURE_NO_DEPRECATE
ms.exe 0 0 0 0 0 >out_ms.txt
diff out_ms.txt output-sample-zeros-ec-ILP32.txt
pause
cls

g++ -o gnu.exe driver-sample.cpp ObjectAllocator.cpp PRNG.cpp -Wall -Wextra -Wconversion -std=c++11 -pedantic -Wl,--enable-auto-import
gnu.exe 0 0 0 0 0 >out_gnu.txt
diff out_gnu.txt output-sample-zeros-ec-LP64.txt

pause

DEL *.obj
cls
goto START