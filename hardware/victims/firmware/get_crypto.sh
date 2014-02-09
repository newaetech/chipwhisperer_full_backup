#!/bin/sh
wget http://avrcryptolib.das-labor.org/avrcryptolib-current.tar.bz2
bunzip2 avrcryptolib-current.tar.bz2
tar -xvf avrcryptolib-current.tar
mkdir crypto/aes
cp avrcryptolib-nightly/avrcryptolib/aes/* crypto/aes/.
cp avrcryptolib-nightly/avrcryptolib/gf256mul/* crypto/aes/.
rm -rf avrcryptolib-nightly
rm -f avrcryptolib-current.tar.bz2
rm -f avrcryptolib-current.tar
