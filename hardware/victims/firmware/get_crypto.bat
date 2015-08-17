REM Following required in case user doesn't have WinAVR setup on path
set PATH=%PATH%;C:\WinAVR-20100110\bin;C:\WinAVR-20100110\utils\bin
wget http://avrcryptolib.das-labor.org/avrcryptolib-current.tar.bz2
bunzip2 avrcryptolib-current.tar.bz2
tar -xvf avrcryptolib-current.tar
mkdir crypto\avr-crypto-lib
mkdir crypto\avr-crypto-lib\aes
mv avrcryptolib-nightly/avrcryptolib crypto/avrcryptolib
rm -rf avrcryptolib-nightly
rm -f avrcryptolib-current.tar.bz2
rm -f avrcryptolib-current.tar
pause
