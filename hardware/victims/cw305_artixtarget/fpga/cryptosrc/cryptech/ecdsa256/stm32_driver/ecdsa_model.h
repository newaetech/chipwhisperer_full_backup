//------------------------------------------------------------------------------
//
// ecdsa_model.h
// --------------------------------------------
// Base point scalar multiplier model for ECDSA
//
// Authors: Pavel Shatov
//
// Copyright (c) 2015-2016, NORDUnet A/S
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
//
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// - Neither the name of the NORDUnet nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//
// Curve Selection
//
// USE_CURVE == 1  -> P-256
// USE_CURVE == 2  -> P-384
//
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Model Parameters
//------------------------------------------------------------------------------
#if USE_CURVE == 1
#define OPERAND_WIDTH	(256)	// largest supported operand width in bits
#elif USE_CURVE == 2
#define OPERAND_WIDTH	(384)	// largest supported operand width in bits
#else
#error USE_CURVE must be either 1 or 2!
#endif


//------------------------------------------------------------------------------
// P-256 Parameters and Test Vectors
//------------------------------------------------------------------------------

/* Field Size */
#define P_256_Q			{0xffffffff, 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff}

/* Generic Numbers */
#define P_256_ZERO		{0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000}
#define P_256_ONE		{0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001}

/* Division Factor  */
#define P_256_DELTA		{0x7fffffff, 0x80000000, 0x80000000, 0x00000000, 0x00000000, 0x80000000, 0x00000000, 0x00000000}

/* Base Point */
#define P_256_G_X		{0x6b17d1f2, 0xe12c4247, 0xf8bce6e5, 0x63a440f2, 0x77037d81, 0x2deb33a0, 0xf4a13945, 0xd898c296}
#define P_256_G_Y		{0x4fe342e2, 0xfe1a7f9b, 0x8ee7eb4a, 0x7c0f9e16, 0x2bce3357, 0x6b315ece, 0xcbb64068, 0x37bf51f5}

/* Doubled Base Point */
#define P_256_H_X		{0x29d05c19, 0x3da77b71, 0x0e863235, 0x38b77e1b, 0x11f904fe, 0xa42998be, 0x16bd8d74, 0x4ece7ad0}
#define P_256_H_Y		{0xb01cbd1c, 0x01e58065, 0x711814b5, 0x83f061e9, 0xd431cca9, 0x94cea131, 0x3449bf97, 0xc840ae07}

/* Base Point Order */
#define P_256_N			{0xffffffff, 0x00000000, 0xffffffff, 0xffffffff, 0xbce6faad, 0xa7179e84, 0xf3b9cac2, 0xfc632551}

/* Private Key */
#define P_256_D			{0x70a12c2d, 0xb16845ed, 0x56ff68cf, 0xc21a472b, 0x3f04d7d6, 0x851bf634, 0x9f2d7d5b, 0x3452b38a}

/* Per-message Random Number */
#define P_256_K			{0x580ec00d, 0x85643433, 0x4cef3f71, 0xecaed496, 0x5b12ae37, 0xfa47055b, 0x1965c7b1, 0x34ee45d0}

/* Public Key */
#define P_256_Q_X		{0x8101ece4, 0x7464a6ea, 0xd70cf69a, 0x6e2bd3d8, 0x8691a326, 0x2d22cba4, 0xf7635eaf, 0xf26680a8}
#define P_256_Q_Y		{0xd8a12ba6, 0x1d599235, 0xf67d9cb4, 0xd58f1783, 0xd3ca43e7, 0x8f0a5aba, 0xa6240799, 0x36c0c3a9}

/* Part of Signature */
#define P_256_R_X		{0x7214bc96, 0x47160bbd, 0x39ff2f80, 0x533f5dc6, 0xddd70ddf, 0x86bb8156, 0x61e805d5, 0xd4e6f27c}
#define P_256_R_Y		{0x8b81e3e9, 0x77597110, 0xc7cf2633, 0x435b2294, 0xb7264298, 0x7defd3d4, 0x007e1cfc, 0x5df84541}


//------------------------------------------------------------------------------
// P-384 Parameters and Test Vectors
//------------------------------------------------------------------------------

/* Field Size */
#define P_384_Q			{0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe, 0xffffffff, 0x00000000, 0x00000000, 0xffffffff}

/* Generic Numbers */
#define P_384_ZERO		{0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000}
#define P_384_ONE		{0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001}

/* Division Factor  */
#define P_384_DELTA		{0x7fffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x7fffffff, 0x80000000, 0x0000000, 0x080000000}

/* Base Point */
#define P_384_G_X		{0xaa87ca22, 0xbe8b0537, 0x8eb1c71e, 0xf320ad74, 0x6e1d3b62, 0x8ba79b98, 0x59f741e0, 0x82542a38, 0x5502f25d, 0xbf55296c, 0x3a545e38, 0x72760ab7}
#define P_384_G_Y		{0x3617de4a, 0x96262c6f, 0x5d9e98bf, 0x9292dc29, 0xf8f41dbd, 0x289a147c, 0xe9da3113, 0xb5f0b8c0, 0x0a60b1ce, 0x1d7e819d, 0x7a431d7c, 0x90ea0e5f}

/* Doubled Base Point */
#define P_384_H_X		{0xaaf06bba, 0x82e9f590, 0xe29c71c2, 0x19bea517, 0x23c5893a, 0xe8b0c8cf, 0x4c117c3e, 0xfb57ab8d, 0x55fa1b42, 0x8155ad27, 0x8b574391, 0x1b13ea8a}
#define P_384_H_Y		{0xc9e821b5, 0x69d9d390, 0xa2616740, 0x6d6d23d6, 0x070be242, 0xd765eb83, 0x1625ceec, 0x4a0f473e, 0xf59f4e30, 0xe2817e62, 0x85bce284, 0x6f15f19d}

/* Base Point Order */
#define P_384_N			{0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xc7634d81, 0xf4372ddf, 0x581a0db2, 0x48b0a77a, 0xecec196a, 0xccc52973}

/* Private Key */
#define P_384_D			{0xc838b852, 0x53ef8dc7, 0x394fa580, 0x8a518398, 0x1c7deef5, 0xa69ba8f4, 0xf2117ffe, 0xa39cfcd9, 0x0e95f6cb, 0xc854abac, 0xab701d50, 0xc1f3cf24}

/* Per-message Random Number */
#define P_384_K			{0xdc6b4403, 0x6989a196, 0xe39d1cda, 0xc000812f, 0x4bdd8b2d, 0xb41bb33a, 0xf5137258, 0x5ebd1db6, 0x3f0ce827, 0x5aa1fd45, 0xe2d2a735, 0xf8749359}

/* Public Key */
#define P_384_Q_X		{0x1fbac8ee, 0xbd0cbf35, 0x640b39ef, 0xe0808dd7, 0x74debff2, 0x0a2a329e, 0x91713baf, 0x7d7f3c3e, 0x81546d88, 0x3730bee7, 0xe48678f8, 0x57b02ca0}
#define P_384_Q_Y		{0xeb213103, 0xbd68ce34, 0x3365a8a4, 0xc3d4555f, 0xa385f533, 0x0203bdd7, 0x6ffad1f3, 0xaffb9575, 0x1c132007, 0xe1b24035, 0x3cb0a4cf, 0x1693bdf9}

/* Part of Signature */
#define P_384_R_X		{0xa0c27ec8, 0x93092dea, 0x1e1bd2cc, 0xfed3cf94, 0x5c8134ed, 0x0c9f8131, 0x1a0f4a05, 0x942db8db, 0xed8dd59f, 0x267471d5, 0x462aa14f, 0xe72de856}
#define P_384_R_Y		{0x85564940, 0x9815bb91, 0x424eaca5, 0xfd76c973, 0x75d575d1, 0x422ec53d, 0x343bd33b, 0x847fdf0c, 0x11569685, 0xb528ab25, 0x49301542, 0x8d7cf72b}


//------------------------------------------------------------------------------
// Parameter and Test Vector Selection
//------------------------------------------------------------------------------
#if USE_CURVE == 1

#define ECDSA_Q			P_256_Q

#define ECDSA_ZERO		P_256_ZERO
#define ECDSA_ONE		P_256_ONE

#define ECDSA_DELTA		P_256_DELTA

#define ECDSA_G_X		P_256_G_X
#define ECDSA_G_Y		P_256_G_Y

#define ECDSA_H_X		P_256_H_X
#define ECDSA_H_Y		P_256_H_Y

#define ECDSA_N			P_256_N
#define ECDSA_D			P_256_D
#define ECDSA_K			P_256_K

#define ECDSA_Q_X		P_256_Q_X
#define ECDSA_Q_Y		P_256_Q_Y

#define ECDSA_R_X		P_256_R_X
#define ECDSA_R_Y		P_256_R_Y

#elif USE_CURVE == 2

#define ECDSA_Q			P_384_Q

#define ECDSA_ZERO		P_384_ZERO
#define ECDSA_ONE		P_384_ONE

#define ECDSA_DELTA		P_384_DELTA

#define ECDSA_G_X		P_384_G_X
#define ECDSA_G_Y		P_384_G_Y

#define ECDSA_H_X		P_384_H_X
#define ECDSA_H_Y		P_384_H_Y

#define ECDSA_N			P_384_N
#define ECDSA_D			P_384_D
#define ECDSA_K			P_384_K

#define ECDSA_Q_X		P_384_Q_X
#define ECDSA_Q_Y		P_384_Q_Y

#define ECDSA_R_X		P_384_R_X
#define ECDSA_R_Y		P_384_R_Y

#else

#error USE_CURVE must be either 1 or 2!

#endif


//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
