cls
echo off
cls
ECHO *** Spartan-6 LX9 MicroBoard Flash Loader with Erase ***
ECHO ------------------------------------------------------------------------
ECHO
rem
rem This batch file will perform a sector erase of the specified length and
rem then write the specified file to the flash starting at address 0..
sfutil -d obp -cr -m N25Q128 -e -fi -w ../avnet_lx9board_ise/openadc_lx9.mcs -t
pause