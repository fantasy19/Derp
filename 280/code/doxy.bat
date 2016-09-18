@echo off
set course=cs280
set login=zihe.goh
set num=1
set savefolder=%course%_%login%_%num%

set file1=ObjectAllocator.cpp
set file2=ObjectAllocator.h

md %savefolder%
copy "%file2%" "./doxy/%file2%"
copy "%file2%" "./%savefolder%/%file2%"
copy "%file1%" "./doxy/%file1%"
copy "%file1%" "./%savefolder%/%file1%"

cd ./doxy
doxygen doxyfile

copy "html\index.chm" "../%savefolder%/index.chm"
REM html\index.chm
REM cd ..
REM 7z a -tz %savefolder%.zip %savefolder%
pause