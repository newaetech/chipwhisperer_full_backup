/* 
ChipWhisperer Artix Target - Register address definitions for ECC 256 pmul target.

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

`define REG_CLKSETTINGS                 'h00
`define REG_USER_LED                    'h01
`define REG_CRYPT_TYPE                  'h02
`define REG_CRYPT_REV                   'h03
`define REG_IDENTIFY                    'h04
`define REG_BUILDTIME                   'h0b
`define REG_CRYPT_GO                    'h05

`define REG_CRYPT_K                     'h06
`define REG_CRYPT_RX                    'h07
`define REG_CRYPT_RY                    'h08
`define REG_CRYPT_GX                    'h09
`define REG_CRYPT_GY                    'h0a

`define REG_BRAM_RD_MSB                 'h0c
`define REG_LFSR_GO                     'h0d
`define REG_LFSR_STATE                  'h0e
`define REG_LFSR_RD                     'h0f
`define REG_LFSR_RD_BANK                'h10
`define REG_NOISE_ENABLE                'h11
`define REG_KB                          'h12
`define REG_RB_SELECT                   'h13

