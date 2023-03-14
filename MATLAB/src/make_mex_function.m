% Compiles C++ code into a .mexw64 file. Tested on cl.
mex COMPFLAGS='$COMPFLAGS -std:c++17 /Otx' ..\src\tsm_matlab.cpp ..\..\src\tsm.cpp -I..\..\src -output ..\bin\tsm.mex
