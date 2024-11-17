/* 
ChipWhisperer Artix Target - Register address definitions for reference target.

Copyright (c) 2020, NewAE Technology Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted without restriction. Note that modules within
the project may have additional restrictions, please carefully inspect
additional licenses.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of NewAE Technology Inc.
*/

// *** WARNING***  
// Two identical copies are maintained in this repo: 
// - one in software/chipwhisperer/capture/targets/defines/, used by CW305.py at runtime
// - one in hardware/victims/cw305_artixtarget/fpga/common/, used by Vivado when building the bitfile
// Ideally we could use a symlink but that doesn't work on Windows. There are solutions to that 
// (https://stackoverflow.com/questions/5917249/git-symlinks-in-windows) but they have their own risks.
// Since this is the only symlink candidate in this repo at this moment, it seems easier/less risky
// to deal with having two files.

`define REG_CLKSETTINGS                 'h00
`define REG_USER_LED                    'h01
`define REG_CRYPT_TYPE                  'h02
`define REG_CRYPT_REV                   'h03
`define REG_IDENTIFY                    'h04
`define REG_CRYPT_GO                    'h05
`define REG_CRYPT_TEXTIN                'h06
`define REG_CRYPT_CIPHERIN              'h07
`define REG_CRYPT_TEXTOUT               'h08
`define REG_CRYPT_CIPHEROUT             'h09
`define REG_CRYPT_KEY                   'h0a
`define REG_BUILDTIME                   'h0b

`define REG_CRYPT_PRE_EXPAND_KEY        'h0c
`define REG_FIFO_ERRORS                 'h0d
`define REG_CRYPT_WRITE_PT              'h0e
`define REG_KEY_GO                      'h0f
`define REG_FIFO_DEPTH                  'h10
`define REG_HALF_PIPE                   'h11


