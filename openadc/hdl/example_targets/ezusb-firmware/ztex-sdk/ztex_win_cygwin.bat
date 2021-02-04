@echo off

C:
chdir C:\cygwin\bin
 
bash --login -c "export PATH=$PATH:/cygdrive/c/Program\ Files/SDCC/bin; exec bash -i"
