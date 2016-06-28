#!/usr/bin/env python
# -*- coding: latin-1 -*-

# encrypt.py
# Script for encrypting strings 

# The string being encrypted
input_str = "Don't forget to buy milk!"

print "p{}\\n".format(input_str.encode('rot13').encode('hex'))