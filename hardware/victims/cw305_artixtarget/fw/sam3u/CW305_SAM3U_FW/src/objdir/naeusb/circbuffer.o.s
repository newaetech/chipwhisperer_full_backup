	.cpu cortex-m3
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 1
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"circbuffer.c"
	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.section	.text.init_circ_buf,"ax",%progbits
	.align	1
	.global	init_circ_buf
	.arch armv7-m
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	init_circ_buf, %function
init_circ_buf:
.LVL0:
.LFB0:
	.file 1 "naeusb/circbuffer.c"
	.loc 1 38 1 view -0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	.loc 1 39 5 view .LVU1
	.loc 1 39 29 is_stmt 0 view .LVU2
	movs	r3, #0
	str	r3, [r0, #4]
	.loc 1 39 16 view .LVU3
	str	r3, [r0]
	.loc 1 40 5 is_stmt 1 view .LVU4
	.loc 1 40 19 is_stmt 0 view .LVU5
	str	r3, [r0, #8]
	.loc 1 41 1 view .LVU6
	bx	lr
	.cfi_endproc
.LFE0:
	.size	init_circ_buf, .-init_circ_buf
	.section	.text.add_to_circ_buf,"ax",%progbits
	.align	1
	.global	add_to_circ_buf
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	add_to_circ_buf, %function
add_to_circ_buf:
.LVL1:
.LFB1:
	.loc 1 50 1 is_stmt 1 view -0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	.loc 1 50 1 is_stmt 0 view .LVU8
	push	{r4}
.LCFI0:
	.cfi_def_cfa_offset 4
	.cfi_offset 4, -4
	.loc 1 52 5 is_stmt 1 view .LVU9
	.loc 1 52 18 is_stmt 0 view .LVU10
	ldr	r4, [r0]
.LVL2:
	.loc 1 53 5 is_stmt 1 view .LVU11
	.loc 1 53 12 is_stmt 0 view .LVU12
	adds	r4, r4, #1
.LVL3:
	.loc 1 54 5 is_stmt 1 view .LVU13
	.loc 1 54 8 is_stmt 0 view .LVU14
	cmp	r4, #199
	bhi	.L8
.LVL4:
.L5:
	.loc 1 56 11 is_stmt 1 view .LVU15
	.loc 1 56 27 is_stmt 0 view .LVU16
	ldr	r3, [r0, #4]
	.loc 1 56 11 view .LVU17
	cmp	r3, r4
	bne	.L10
	.loc 1 58 9 is_stmt 1 view .LVU18
	.loc 1 58 12 is_stmt 0 view .LVU19
	cmp	r2, #0
	bne	.L5
	.loc 1 60 13 is_stmt 1 view .LVU20
	.loc 1 60 17 is_stmt 0 view .LVU21
	ldr	r3, [r0, #8]
	.loc 1 60 26 view .LVU22
	adds	r3, r3, #1
	str	r3, [r0, #8]
	.loc 1 61 13 is_stmt 1 view .LVU23
	b	.L2
.L8:
	.loc 1 55 17 is_stmt 0 view .LVU24
	movs	r4, #0
.LVL5:
	.loc 1 55 17 view .LVU25
	b	.L5
.LVL6:
.L10:
	.loc 1 68 5 is_stmt 1 view .LVU26
	.loc 1 68 19 is_stmt 0 view .LVU27
	ldr	r3, [r0]
	.loc 1 68 27 view .LVU28
	add	r3, r3, r0
	strb	r1, [r3, #12]
	.loc 1 69 5 is_stmt 1 view .LVU29
	.loc 1 69 16 is_stmt 0 view .LVU30
	str	r4, [r0]
.L2:
	.loc 1 70 1 view .LVU31
	pop	{r4}
.LCFI1:
	.cfi_restore 4
	.cfi_def_cfa_offset 0
.LVL7:
	.loc 1 70 1 view .LVU32
	bx	lr
	.cfi_endproc
.LFE1:
	.size	add_to_circ_buf, .-add_to_circ_buf
	.section	.text.get_from_circ_buf,"ax",%progbits
	.align	1
	.global	get_from_circ_buf
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_from_circ_buf, %function
get_from_circ_buf:
.LVL8:
.LFB2:
	.loc 1 81 1 is_stmt 1 view -0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	.loc 1 81 1 is_stmt 0 view .LVU34
	mov	r2, r0
	.loc 1 84 5 is_stmt 1 view .LVU35
	.loc 1 84 18 is_stmt 0 view .LVU36
	ldr	r3, [r0, #4]
.LVL9:
	.loc 1 85 5 is_stmt 1 view .LVU37
	.loc 1 85 13 is_stmt 0 view .LVU38
	adds	r1, r0, r3
	ldrb	r0, [r1, #12]	@ zero_extendqisi2
.LVL10:
	.loc 1 87 5 is_stmt 1 view .LVU39
	.loc 1 87 24 is_stmt 0 view .LVU40
	ldr	r1, [r2]
	.loc 1 87 8 view .LVU41
	cmp	r1, r3
	beq	.L14
	.loc 1 90 5 is_stmt 1 view .LVU42
	.loc 1 90 12 is_stmt 0 view .LVU43
	adds	r3, r3, #1
.LVL11:
	.loc 1 91 5 is_stmt 1 view .LVU44
	.loc 1 91 8 is_stmt 0 view .LVU45
	cmp	r3, #199
	bls	.L13
	.loc 1 93 17 view .LVU46
	movs	r3, #0
.LVL12:
.L13:
	.loc 1 94 5 is_stmt 1 view .LVU47
	.loc 1 94 16 is_stmt 0 view .LVU48
	str	r3, [r2, #4]
	.loc 1 96 5 is_stmt 1 view .LVU49
	.loc 1 96 12 is_stmt 0 view .LVU50
	bx	lr
.L14:
	.loc 1 88 16 view .LVU51
	movs	r0, #255
.LVL13:
	.loc 1 97 1 view .LVU52
	bx	lr
	.cfi_endproc
.LFE2:
	.size	get_from_circ_buf, .-get_from_circ_buf
	.section	.text.circ_buf_has_char,"ax",%progbits
	.align	1
	.global	circ_buf_has_char
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	circ_buf_has_char, %function
circ_buf_has_char:
.LVL14:
.LFB3:
	.loc 1 108 1 is_stmt 1 view -0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	.loc 1 110 5 view .LVU54
	.loc 1 110 18 is_stmt 0 view .LVU55
	ldr	r3, [r0]
.LVL15:
	.loc 1 111 5 is_stmt 1 view .LVU56
	.loc 1 111 25 is_stmt 0 view .LVU57
	ldr	r0, [r0, #4]
.LVL16:
	.loc 1 112 1 view .LVU58
	subs	r0, r0, r3
	it	ne
	movne	r0, #1
	bx	lr
	.cfi_endproc
.LFE3:
	.size	circ_buf_has_char, .-circ_buf_has_char
	.section	.text.circ_buf_count,"ax",%progbits
	.align	1
	.global	circ_buf_count
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	circ_buf_count, %function
circ_buf_count:
.LVL17:
.LFB4:
	.loc 1 122 1 is_stmt 1 view -0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	.loc 1 123 5 view .LVU60
	.loc 1 125 5 view .LVU61
	.loc 1 125 17 is_stmt 0 view .LVU62
	ldr	r3, [r0]
.LVL18:
	.loc 1 126 5 is_stmt 1 view .LVU63
	.loc 1 126 18 is_stmt 0 view .LVU64
	ldr	r0, [r0, #4]
.LVL19:
	.loc 1 127 5 is_stmt 1 view .LVU65
	.loc 1 127 8 is_stmt 0 view .LVU66
	subs	r0, r3, r0
.LVL20:
	.loc 1 127 8 view .LVU67
	bmi	.L19
.LVL21:
.L18:
	.loc 1 129 5 is_stmt 1 view .LVU68
	.loc 1 130 1 is_stmt 0 view .LVU69
	bx	lr
.L19:
	.loc 1 128 9 is_stmt 1 view .LVU70
	.loc 1 128 15 is_stmt 0 view .LVU71
	adds	r0, r0, #200
.LVL22:
	.loc 1 128 15 view .LVU72
	b	.L18
	.cfi_endproc
.LFE4:
	.size	circ_buf_count, .-circ_buf_count
	.text
.Letext0:
	.file 2 "c:\\program files (x86)\\gnu arm embedded toolchain\\9 2020-q2-update\\arm-none-eabi\\include\\machine\\_default_types.h"
	.file 3 "c:\\program files (x86)\\gnu arm embedded toolchain\\9 2020-q2-update\\arm-none-eabi\\include\\sys\\_stdint.h"
	.file 4 "naeusb/circbuffer.h"
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.4byte	0x263
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF651
	.byte	0xc
	.4byte	.LASF652
	.4byte	.LASF653
	.4byte	.Ldebug_ranges0+0
	.4byte	0
	.4byte	0
	.4byte	.Ldebug_line0
	.4byte	.Ldebug_macro0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.4byte	.LASF626
	.uleb128 0x3
	.4byte	.LASF635
	.byte	0x2
	.byte	0x2b
	.byte	0x18
	.4byte	0x40
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF627
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.4byte	.LASF628
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.4byte	.LASF629
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.4byte	.LASF630
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF631
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.4byte	.LASF632
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.4byte	.LASF633
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF634
	.uleb128 0x5
	.4byte	0x78
	.uleb128 0x3
	.4byte	.LASF636
	.byte	0x3
	.byte	0x18
	.byte	0x13
	.4byte	0x34
	.uleb128 0x6
	.byte	0xd4
	.byte	0x4
	.byte	0x2d
	.byte	0x9
	.4byte	0xd6
	.uleb128 0x7
	.4byte	.LASF637
	.byte	0x4
	.byte	0x2e
	.byte	0x1b
	.4byte	0x7f
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x7
	.4byte	.LASF638
	.byte	0x4
	.byte	0x2f
	.byte	0x1b
	.4byte	0x7f
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x7
	.4byte	.LASF639
	.byte	0x4
	.byte	0x30
	.byte	0x1b
	.4byte	0x7f
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x8
	.ascii	"buf\000"
	.byte	0x4
	.byte	0x31
	.byte	0xd
	.4byte	0xd6
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.byte	0
	.uleb128 0x9
	.4byte	0x84
	.4byte	0xe6
	.uleb128 0xa
	.4byte	0x78
	.byte	0xc7
	.byte	0
	.uleb128 0x3
	.4byte	.LASF640
	.byte	0x4
	.byte	0x32
	.byte	0x3
	.4byte	0x90
	.uleb128 0xb
	.byte	0x1
	.4byte	.LASF641
	.byte	0x1
	.byte	0x79
	.byte	0xe
	.byte	0x1
	.4byte	0x78
	.4byte	.LFB4
	.4byte	.LFE4
	.byte	0x2
	.byte	0x7d
	.sleb128 0
	.byte	0x1
	.4byte	0x139
	.uleb128 0xc
	.4byte	.LASF643
	.byte	0x1
	.byte	0x79
	.byte	0x28
	.4byte	0x139
	.4byte	.LLST7
	.4byte	.LVUS7
	.uleb128 0xd
	.4byte	.LASF644
	.byte	0x1
	.byte	0x7b
	.byte	0x9
	.4byte	0x71
	.4byte	.LLST8
	.4byte	.LVUS8
	.byte	0
	.uleb128 0xe
	.byte	0x4
	.4byte	0xe6
	.uleb128 0xb
	.byte	0x1
	.4byte	.LASF642
	.byte	0x1
	.byte	0x6b
	.byte	0x6
	.byte	0x1
	.4byte	0x186
	.4byte	.LFB3
	.4byte	.LFE3
	.byte	0x2
	.byte	0x7d
	.sleb128 0
	.byte	0x1
	.4byte	0x186
	.uleb128 0xc
	.4byte	.LASF643
	.byte	0x1
	.byte	0x6b
	.byte	0x23
	.4byte	0x139
	.4byte	.LLST5
	.4byte	.LVUS5
	.uleb128 0xd
	.4byte	.LASF637
	.byte	0x1
	.byte	0x6e
	.byte	0x12
	.4byte	0x78
	.4byte	.LLST6
	.4byte	.LVUS6
	.byte	0
	.uleb128 0x2
	.byte	0x1
	.byte	0x2
	.4byte	.LASF645
	.uleb128 0xb
	.byte	0x1
	.4byte	.LASF646
	.byte	0x1
	.byte	0x50
	.byte	0x9
	.byte	0x1
	.4byte	0x84
	.4byte	.LFB2
	.4byte	.LFE2
	.byte	0x2
	.byte	0x7d
	.sleb128 0
	.byte	0x1
	.4byte	0x1e8
	.uleb128 0xc
	.4byte	.LASF643
	.byte	0x1
	.byte	0x50
	.byte	0x26
	.4byte	0x139
	.4byte	.LLST2
	.4byte	.LVUS2
	.uleb128 0xd
	.4byte	.LASF647
	.byte	0x1
	.byte	0x54
	.byte	0x12
	.4byte	0x78
	.4byte	.LLST3
	.4byte	.LVUS3
	.uleb128 0xd
	.4byte	.LASF648
	.byte	0x1
	.byte	0x55
	.byte	0xd
	.4byte	0x84
	.4byte	.LLST4
	.4byte	.LVUS4
	.byte	0
	.uleb128 0xf
	.byte	0x1
	.4byte	.LASF654
	.byte	0x1
	.byte	0x31
	.byte	0x6
	.byte	0x1
	.4byte	.LFB1
	.4byte	.LFE1
	.4byte	.LLST0
	.byte	0x1
	.4byte	0x241
	.uleb128 0x10
	.4byte	.LASF643
	.byte	0x1
	.byte	0x31
	.byte	0x21
	.4byte	0x139
	.byte	0x1
	.byte	0x50
	.uleb128 0x11
	.ascii	"ch\000"
	.byte	0x1
	.byte	0x31
	.byte	0x2f
	.4byte	0x84
	.byte	0x1
	.byte	0x51
	.uleb128 0x10
	.4byte	.LASF649
	.byte	0x1
	.byte	0x31
	.byte	0x38
	.4byte	0x186
	.byte	0x1
	.byte	0x52
	.uleb128 0xd
	.4byte	.LASF650
	.byte	0x1
	.byte	0x34
	.byte	0x12
	.4byte	0x78
	.4byte	.LLST1
	.4byte	.LVUS1
	.byte	0
	.uleb128 0x12
	.byte	0x1
	.4byte	.LASF655
	.byte	0x1
	.byte	0x25
	.byte	0x6
	.byte	0x1
	.4byte	.LFB0
	.4byte	.LFE0
	.byte	0x2
	.byte	0x7d
	.sleb128 0
	.byte	0x1
	.uleb128 0x10
	.4byte	.LASF643
	.byte	0x1
	.byte	0x25
	.byte	0x1f
	.4byte	0x139
	.byte	0x1
	.byte	0x50
	.byte	0
	.byte	0
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x55
	.uleb128 0x6
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x52
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x6
	.uleb128 0x2119
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x35
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.uleb128 0x2117
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.uleb128 0x2137
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.uleb128 0x2137
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x2117
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.uleb128 0x2117
	.uleb128 0xc
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_loc,"",%progbits
.Ldebug_loc0:
.LVUS7:
	.uleb128 0
	.uleb128 .LVU65
	.uleb128 .LVU65
	.uleb128 0
.LLST7:
	.4byte	.LVL17
	.4byte	.LVL19
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL19
	.4byte	.LFE4
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x50
	.byte	0x9f
	.4byte	0
	.4byte	0
.LVUS8:
	.uleb128 .LVU63
	.uleb128 .LVU65
	.uleb128 .LVU65
	.uleb128 .LVU67
	.uleb128 .LVU67
	.uleb128 0
.LLST8:
	.4byte	.LVL18
	.4byte	.LVL19
	.2byte	0x1
	.byte	0x53
	.4byte	.LVL19
	.4byte	.LVL20
	.2byte	0x6
	.byte	0x73
	.sleb128 0
	.byte	0x70
	.sleb128 0
	.byte	0x1c
	.byte	0x9f
	.4byte	.LVL20
	.4byte	.LFE4
	.2byte	0x1
	.byte	0x50
	.4byte	0
	.4byte	0
.LVUS5:
	.uleb128 0
	.uleb128 .LVU58
	.uleb128 .LVU58
	.uleb128 0
.LLST5:
	.4byte	.LVL14
	.4byte	.LVL16
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL16
	.4byte	.LFE3
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x50
	.byte	0x9f
	.4byte	0
	.4byte	0
.LVUS6:
	.uleb128 .LVU56
	.uleb128 0
.LLST6:
	.4byte	.LVL15
	.4byte	.LFE3
	.2byte	0x1
	.byte	0x53
	.4byte	0
	.4byte	0
.LVUS2:
	.uleb128 0
	.uleb128 .LVU39
	.uleb128 .LVU39
	.uleb128 0
.LLST2:
	.4byte	.LVL8
	.4byte	.LVL10
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL10
	.4byte	.LFE2
	.2byte	0x1
	.byte	0x52
	.4byte	0
	.4byte	0
.LVUS3:
	.uleb128 .LVU37
	.uleb128 0
.LLST3:
	.4byte	.LVL9
	.4byte	.LFE2
	.2byte	0x1
	.byte	0x53
	.4byte	0
	.4byte	0
.LVUS4:
	.uleb128 .LVU39
	.uleb128 .LVU52
	.uleb128 .LVU52
	.uleb128 0
.LLST4:
	.4byte	.LVL10
	.4byte	.LVL13
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL13
	.4byte	.LFE2
	.2byte	0x7
	.byte	0x72
	.sleb128 0
	.byte	0x73
	.sleb128 0
	.byte	0x22
	.byte	0x23
	.uleb128 0xc
	.4byte	0
	.4byte	0
.LLST0:
	.4byte	.LFB1
	.4byte	.LCFI0
	.2byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	.LCFI0
	.4byte	.LCFI1
	.2byte	0x2
	.byte	0x7d
	.sleb128 4
	.4byte	.LCFI1
	.4byte	.LFE1
	.2byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	0
	.4byte	0
.LVUS1:
	.uleb128 .LVU11
	.uleb128 .LVU25
	.uleb128 .LVU26
	.uleb128 .LVU32
.LLST1:
	.4byte	.LVL2
	.4byte	.LVL5
	.2byte	0x1
	.byte	0x54
	.4byte	.LVL6
	.4byte	.LVL7
	.2byte	0x1
	.byte	0x54
	.4byte	0
	.4byte	0
	.section	.debug_aranges,"",%progbits
	.4byte	0x3c
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0
	.2byte	0
	.2byte	0
	.4byte	.LFB0
	.4byte	.LFE0-.LFB0
	.4byte	.LFB1
	.4byte	.LFE1-.LFB1
	.4byte	.LFB2
	.4byte	.LFE2-.LFB2
	.4byte	.LFB3
	.4byte	.LFE3-.LFB3
	.4byte	.LFB4
	.4byte	.LFE4-.LFB4
	.4byte	0
	.4byte	0
	.section	.debug_ranges,"",%progbits
.Ldebug_ranges0:
	.4byte	.LFB0
	.4byte	.LFE0
	.4byte	.LFB1
	.4byte	.LFE1
	.4byte	.LFB2
	.4byte	.LFE2
	.4byte	.LFB3
	.4byte	.LFE3
	.4byte	.LFB4
	.4byte	.LFE4
	.4byte	0
	.4byte	0
	.section	.debug_macro,"",%progbits
.Ldebug_macro0:
	.2byte	0x4
	.byte	0x2
	.4byte	.Ldebug_line0
	.byte	0x7
	.4byte	.Ldebug_macro2
	.byte	0x3
	.uleb128 0
	.uleb128 0x1
	.byte	0x3
	.uleb128 0x1e
	.uleb128 0x4
	.byte	0x5
	.uleb128 0x1f
	.4byte	.LASF459
	.file 5 "c:\\program files (x86)\\gnu arm embedded toolchain\\9 2020-q2-update\\lib\\gcc\\arm-none-eabi\\9.3.1\\include\\stdbool.h"
	.byte	0x3
	.uleb128 0x22
	.uleb128 0x5
	.byte	0x7
	.4byte	.Ldebug_macro3
	.byte	0x4
	.byte	0x7
	.4byte	.Ldebug_macro4
	.file 6 "c:\\program files (x86)\\gnu arm embedded toolchain\\9 2020-q2-update\\lib\\gcc\\arm-none-eabi\\9.3.1\\include\\stdint.h"
	.byte	0x3
	.uleb128 0x2b
	.uleb128 0x6
	.file 7 "c:\\program files (x86)\\gnu arm embedded toolchain\\9 2020-q2-update\\arm-none-eabi\\include\\stdint.h"
	.byte	0x3
	.uleb128 0x9
	.uleb128 0x7
	.byte	0x5
	.uleb128 0xa
	.4byte	.LASF468
	.byte	0x3
	.uleb128 0xc
	.uleb128 0x2
	.byte	0x5
	.uleb128 0x6
	.4byte	.LASF469
	.file 8 "c:\\program files (x86)\\gnu arm embedded toolchain\\9 2020-q2-update\\arm-none-eabi\\include\\sys\\features.h"
	.byte	0x3
	.uleb128 0x8
	.uleb128 0x8
	.byte	0x5
	.uleb128 0x16
	.4byte	.LASF470
	.file 9 "c:\\program files (x86)\\gnu arm embedded toolchain\\9 2020-q2-update\\arm-none-eabi\\include\\_newlib_version.h"
	.byte	0x3
	.uleb128 0x1c
	.uleb128 0x9
	.byte	0x7
	.4byte	.Ldebug_macro5
	.byte	0x4
	.byte	0x7
	.4byte	.Ldebug_macro6
	.byte	0x4
	.byte	0x7
	.4byte	.Ldebug_macro7
	.byte	0x4
	.file 10 "c:\\program files (x86)\\gnu arm embedded toolchain\\9 2020-q2-update\\arm-none-eabi\\include\\sys\\_intsup.h"
	.byte	0x3
	.uleb128 0xd
	.uleb128 0xa
	.byte	0x7
	.4byte	.Ldebug_macro8
	.byte	0x4
	.byte	0x3
	.uleb128 0xe
	.uleb128 0x3
	.byte	0x7
	.4byte	.Ldebug_macro9
	.byte	0x4
	.byte	0x7
	.4byte	.Ldebug_macro10
	.byte	0x4
	.byte	0x5
	.uleb128 0xd
	.4byte	.LASF625
	.byte	0x4
	.byte	0x4
	.byte	0x4
	.byte	0
	.section	.debug_macro,"G",%progbits,wm4.0.f71aa3078e5cb2afd95bc44219de276a,comdat
.Ldebug_macro2:
	.2byte	0x4
	.byte	0
	.byte	0x5
	.uleb128 0
	.4byte	.LASF0
	.byte	0x5
	.uleb128 0
	.4byte	.LASF1
	.byte	0x5
	.uleb128 0
	.4byte	.LASF2
	.byte	0x5
	.uleb128 0
	.4byte	.LASF3
	.byte	0x5
	.uleb128 0
	.4byte	.LASF4
	.byte	0x5
	.uleb128 0
	.4byte	.LASF5
	.byte	0x5
	.uleb128 0
	.4byte	.LASF6
	.byte	0x5
	.uleb128 0
	.4byte	.LASF7
	.byte	0x5
	.uleb128 0
	.4byte	.LASF8
	.byte	0x5
	.uleb128 0
	.4byte	.LASF9
	.byte	0x5
	.uleb128 0
	.4byte	.LASF10
	.byte	0x5
	.uleb128 0
	.4byte	.LASF11
	.byte	0x5
	.uleb128 0
	.4byte	.LASF12
	.byte	0x5
	.uleb128 0
	.4byte	.LASF13
	.byte	0x5
	.uleb128 0
	.4byte	.LASF14
	.byte	0x5
	.uleb128 0
	.4byte	.LASF15
	.byte	0x5
	.uleb128 0
	.4byte	.LASF16
	.byte	0x5
	.uleb128 0
	.4byte	.LASF17
	.byte	0x5
	.uleb128 0
	.4byte	.LASF18
	.byte	0x5
	.uleb128 0
	.4byte	.LASF19
	.byte	0x5
	.uleb128 0
	.4byte	.LASF20
	.byte	0x5
	.uleb128 0
	.4byte	.LASF21
	.byte	0x5
	.uleb128 0
	.4byte	.LASF22
	.byte	0x5
	.uleb128 0
	.4byte	.LASF23
	.byte	0x5
	.uleb128 0
	.4byte	.LASF24
	.byte	0x5
	.uleb128 0
	.4byte	.LASF25
	.byte	0x5
	.uleb128 0
	.4byte	.LASF26
	.byte	0x5
	.uleb128 0
	.4byte	.LASF27
	.byte	0x5
	.uleb128 0
	.4byte	.LASF28
	.byte	0x5
	.uleb128 0
	.4byte	.LASF29
	.byte	0x5
	.uleb128 0
	.4byte	.LASF30
	.byte	0x5
	.uleb128 0
	.4byte	.LASF31
	.byte	0x5
	.uleb128 0
	.4byte	.LASF32
	.byte	0x5
	.uleb128 0
	.4byte	.LASF33
	.byte	0x5
	.uleb128 0
	.4byte	.LASF34
	.byte	0x5
	.uleb128 0
	.4byte	.LASF35
	.byte	0x5
	.uleb128 0
	.4byte	.LASF36
	.byte	0x5
	.uleb128 0
	.4byte	.LASF37
	.byte	0x5
	.uleb128 0
	.4byte	.LASF38
	.byte	0x5
	.uleb128 0
	.4byte	.LASF39
	.byte	0x5
	.uleb128 0
	.4byte	.LASF40
	.byte	0x5
	.uleb128 0
	.4byte	.LASF41
	.byte	0x5
	.uleb128 0
	.4byte	.LASF42
	.byte	0x5
	.uleb128 0
	.4byte	.LASF43
	.byte	0x5
	.uleb128 0
	.4byte	.LASF44
	.byte	0x5
	.uleb128 0
	.4byte	.LASF45
	.byte	0x5
	.uleb128 0
	.4byte	.LASF46
	.byte	0x5
	.uleb128 0
	.4byte	.LASF47
	.byte	0x5
	.uleb128 0
	.4byte	.LASF48
	.byte	0x5
	.uleb128 0
	.4byte	.LASF49
	.byte	0x5
	.uleb128 0
	.4byte	.LASF50
	.byte	0x5
	.uleb128 0
	.4byte	.LASF51
	.byte	0x5
	.uleb128 0
	.4byte	.LASF52
	.byte	0x5
	.uleb128 0
	.4byte	.LASF53
	.byte	0x5
	.uleb128 0
	.4byte	.LASF54
	.byte	0x5
	.uleb128 0
	.4byte	.LASF55
	.byte	0x5
	.uleb128 0
	.4byte	.LASF56
	.byte	0x5
	.uleb128 0
	.4byte	.LASF57
	.byte	0x5
	.uleb128 0
	.4byte	.LASF58
	.byte	0x5
	.uleb128 0
	.4byte	.LASF59
	.byte	0x5
	.uleb128 0
	.4byte	.LASF60
	.byte	0x5
	.uleb128 0
	.4byte	.LASF61
	.byte	0x5
	.uleb128 0
	.4byte	.LASF62
	.byte	0x5
	.uleb128 0
	.4byte	.LASF63
	.byte	0x5
	.uleb128 0
	.4byte	.LASF64
	.byte	0x5
	.uleb128 0
	.4byte	.LASF65
	.byte	0x5
	.uleb128 0
	.4byte	.LASF66
	.byte	0x5
	.uleb128 0
	.4byte	.LASF67
	.byte	0x5
	.uleb128 0
	.4byte	.LASF68
	.byte	0x5
	.uleb128 0
	.4byte	.LASF69
	.byte	0x5
	.uleb128 0
	.4byte	.LASF70
	.byte	0x5
	.uleb128 0
	.4byte	.LASF71
	.byte	0x5
	.uleb128 0
	.4byte	.LASF72
	.byte	0x5
	.uleb128 0
	.4byte	.LASF73
	.byte	0x5
	.uleb128 0
	.4byte	.LASF74
	.byte	0x5
	.uleb128 0
	.4byte	.LASF75
	.byte	0x5
	.uleb128 0
	.4byte	.LASF76
	.byte	0x5
	.uleb128 0
	.4byte	.LASF77
	.byte	0x5
	.uleb128 0
	.4byte	.LASF78
	.byte	0x5
	.uleb128 0
	.4byte	.LASF79
	.byte	0x5
	.uleb128 0
	.4byte	.LASF80
	.byte	0x5
	.uleb128 0
	.4byte	.LASF81
	.byte	0x5
	.uleb128 0
	.4byte	.LASF82
	.byte	0x5
	.uleb128 0
	.4byte	.LASF83
	.byte	0x5
	.uleb128 0
	.4byte	.LASF84
	.byte	0x5
	.uleb128 0
	.4byte	.LASF85
	.byte	0x5
	.uleb128 0
	.4byte	.LASF86
	.byte	0x5
	.uleb128 0
	.4byte	.LASF87
	.byte	0x5
	.uleb128 0
	.4byte	.LASF88
	.byte	0x5
	.uleb128 0
	.4byte	.LASF89
	.byte	0x5
	.uleb128 0
	.4byte	.LASF90
	.byte	0x5
	.uleb128 0
	.4byte	.LASF91
	.byte	0x5
	.uleb128 0
	.4byte	.LASF92
	.byte	0x5
	.uleb128 0
	.4byte	.LASF93
	.byte	0x5
	.uleb128 0
	.4byte	.LASF94
	.byte	0x5
	.uleb128 0
	.4byte	.LASF95
	.byte	0x5
	.uleb128 0
	.4byte	.LASF96
	.byte	0x5
	.uleb128 0
	.4byte	.LASF97
	.byte	0x5
	.uleb128 0
	.4byte	.LASF98
	.byte	0x5
	.uleb128 0
	.4byte	.LASF99
	.byte	0x5
	.uleb128 0
	.4byte	.LASF100
	.byte	0x5
	.uleb128 0
	.4byte	.LASF101
	.byte	0x5
	.uleb128 0
	.4byte	.LASF102
	.byte	0x5
	.uleb128 0
	.4byte	.LASF103
	.byte	0x5
	.uleb128 0
	.4byte	.LASF104
	.byte	0x5
	.uleb128 0
	.4byte	.LASF105
	.byte	0x5
	.uleb128 0
	.4byte	.LASF106
	.byte	0x5
	.uleb128 0
	.4byte	.LASF107
	.byte	0x5
	.uleb128 0
	.4byte	.LASF108
	.byte	0x5
	.uleb128 0
	.4byte	.LASF109
	.byte	0x5
	.uleb128 0
	.4byte	.LASF110
	.byte	0x5
	.uleb128 0
	.4byte	.LASF111
	.byte	0x5
	.uleb128 0
	.4byte	.LASF112
	.byte	0x5
	.uleb128 0
	.4byte	.LASF113
	.byte	0x5
	.uleb128 0
	.4byte	.LASF114
	.byte	0x5
	.uleb128 0
	.4byte	.LASF115
	.byte	0x5
	.uleb128 0
	.4byte	.LASF116
	.byte	0x5
	.uleb128 0
	.4byte	.LASF117
	.byte	0x5
	.uleb128 0
	.4byte	.LASF118
	.byte	0x5
	.uleb128 0
	.4byte	.LASF119
	.byte	0x5
	.uleb128 0
	.4byte	.LASF120
	.byte	0x5
	.uleb128 0
	.4byte	.LASF121
	.byte	0x5
	.uleb128 0
	.4byte	.LASF122
	.byte	0x5
	.uleb128 0
	.4byte	.LASF123
	.byte	0x5
	.uleb128 0
	.4byte	.LASF124
	.byte	0x5
	.uleb128 0
	.4byte	.LASF125
	.byte	0x5
	.uleb128 0
	.4byte	.LASF126
	.byte	0x5
	.uleb128 0
	.4byte	.LASF127
	.byte	0x5
	.uleb128 0
	.4byte	.LASF128
	.byte	0x5
	.uleb128 0
	.4byte	.LASF129
	.byte	0x5
	.uleb128 0
	.4byte	.LASF130
	.byte	0x5
	.uleb128 0
	.4byte	.LASF131
	.byte	0x5
	.uleb128 0
	.4byte	.LASF132
	.byte	0x5
	.uleb128 0
	.4byte	.LASF133
	.byte	0x5
	.uleb128 0
	.4byte	.LASF134
	.byte	0x5
	.uleb128 0
	.4byte	.LASF135
	.byte	0x5
	.uleb128 0
	.4byte	.LASF136
	.byte	0x5
	.uleb128 0
	.4byte	.LASF137
	.byte	0x5
	.uleb128 0
	.4byte	.LASF138
	.byte	0x5
	.uleb128 0
	.4byte	.LASF139
	.byte	0x5
	.uleb128 0
	.4byte	.LASF140
	.byte	0x5
	.uleb128 0
	.4byte	.LASF141
	.byte	0x5
	.uleb128 0
	.4byte	.LASF142
	.byte	0x5
	.uleb128 0
	.4byte	.LASF143
	.byte	0x5
	.uleb128 0
	.4byte	.LASF144
	.byte	0x5
	.uleb128 0
	.4byte	.LASF145
	.byte	0x5
	.uleb128 0
	.4byte	.LASF146
	.byte	0x5
	.uleb128 0
	.4byte	.LASF147
	.byte	0x5
	.uleb128 0
	.4byte	.LASF148
	.byte	0x5
	.uleb128 0
	.4byte	.LASF149
	.byte	0x5
	.uleb128 0
	.4byte	.LASF150
	.byte	0x5
	.uleb128 0
	.4byte	.LASF151
	.byte	0x5
	.uleb128 0
	.4byte	.LASF152
	.byte	0x5
	.uleb128 0
	.4byte	.LASF153
	.byte	0x5
	.uleb128 0
	.4byte	.LASF154
	.byte	0x5
	.uleb128 0
	.4byte	.LASF155
	.byte	0x5
	.uleb128 0
	.4byte	.LASF156
	.byte	0x5
	.uleb128 0
	.4byte	.LASF157
	.byte	0x5
	.uleb128 0
	.4byte	.LASF158
	.byte	0x5
	.uleb128 0
	.4byte	.LASF159
	.byte	0x5
	.uleb128 0
	.4byte	.LASF160
	.byte	0x5
	.uleb128 0
	.4byte	.LASF161
	.byte	0x5
	.uleb128 0
	.4byte	.LASF162
	.byte	0x5
	.uleb128 0
	.4byte	.LASF163
	.byte	0x5
	.uleb128 0
	.4byte	.LASF164
	.byte	0x5
	.uleb128 0
	.4byte	.LASF165
	.byte	0x5
	.uleb128 0
	.4byte	.LASF166
	.byte	0x5
	.uleb128 0
	.4byte	.LASF167
	.byte	0x5
	.uleb128 0
	.4byte	.LASF168
	.byte	0x5
	.uleb128 0
	.4byte	.LASF169
	.byte	0x5
	.uleb128 0
	.4byte	.LASF170
	.byte	0x5
	.uleb128 0
	.4byte	.LASF171
	.byte	0x5
	.uleb128 0
	.4byte	.LASF172
	.byte	0x5
	.uleb128 0
	.4byte	.LASF173
	.byte	0x5
	.uleb128 0
	.4byte	.LASF174
	.byte	0x5
	.uleb128 0
	.4byte	.LASF175
	.byte	0x5
	.uleb128 0
	.4byte	.LASF176
	.byte	0x5
	.uleb128 0
	.4byte	.LASF177
	.byte	0x5
	.uleb128 0
	.4byte	.LASF178
	.byte	0x5
	.uleb128 0
	.4byte	.LASF179
	.byte	0x5
	.uleb128 0
	.4byte	.LASF180
	.byte	0x5
	.uleb128 0
	.4byte	.LASF181
	.byte	0x5
	.uleb128 0
	.4byte	.LASF182
	.byte	0x5
	.uleb128 0
	.4byte	.LASF183
	.byte	0x5
	.uleb128 0
	.4byte	.LASF184
	.byte	0x5
	.uleb128 0
	.4byte	.LASF185
	.byte	0x5
	.uleb128 0
	.4byte	.LASF186
	.byte	0x5
	.uleb128 0
	.4byte	.LASF187
	.byte	0x5
	.uleb128 0
	.4byte	.LASF188
	.byte	0x5
	.uleb128 0
	.4byte	.LASF189
	.byte	0x5
	.uleb128 0
	.4byte	.LASF190
	.byte	0x5
	.uleb128 0
	.4byte	.LASF191
	.byte	0x5
	.uleb128 0
	.4byte	.LASF192
	.byte	0x5
	.uleb128 0
	.4byte	.LASF193
	.byte	0x5
	.uleb128 0
	.4byte	.LASF194
	.byte	0x5
	.uleb128 0
	.4byte	.LASF195
	.byte	0x5
	.uleb128 0
	.4byte	.LASF196
	.byte	0x5
	.uleb128 0
	.4byte	.LASF197
	.byte	0x5
	.uleb128 0
	.4byte	.LASF198
	.byte	0x5
	.uleb128 0
	.4byte	.LASF199
	.byte	0x5
	.uleb128 0
	.4byte	.LASF200
	.byte	0x5
	.uleb128 0
	.4byte	.LASF201
	.byte	0x5
	.uleb128 0
	.4byte	.LASF202
	.byte	0x5
	.uleb128 0
	.4byte	.LASF203
	.byte	0x5
	.uleb128 0
	.4byte	.LASF204
	.byte	0x5
	.uleb128 0
	.4byte	.LASF205
	.byte	0x5
	.uleb128 0
	.4byte	.LASF206
	.byte	0x5
	.uleb128 0
	.4byte	.LASF207
	.byte	0x5
	.uleb128 0
	.4byte	.LASF208
	.byte	0x5
	.uleb128 0
	.4byte	.LASF209
	.byte	0x5
	.uleb128 0
	.4byte	.LASF210
	.byte	0x5
	.uleb128 0
	.4byte	.LASF211
	.byte	0x5
	.uleb128 0
	.4byte	.LASF212
	.byte	0x5
	.uleb128 0
	.4byte	.LASF213
	.byte	0x5
	.uleb128 0
	.4byte	.LASF214
	.byte	0x5
	.uleb128 0
	.4byte	.LASF215
	.byte	0x5
	.uleb128 0
	.4byte	.LASF216
	.byte	0x5
	.uleb128 0
	.4byte	.LASF217
	.byte	0x5
	.uleb128 0
	.4byte	.LASF218
	.byte	0x5
	.uleb128 0
	.4byte	.LASF219
	.byte	0x5
	.uleb128 0
	.4byte	.LASF220
	.byte	0x5
	.uleb128 0
	.4byte	.LASF221
	.byte	0x5
	.uleb128 0
	.4byte	.LASF222
	.byte	0x5
	.uleb128 0
	.4byte	.LASF223
	.byte	0x5
	.uleb128 0
	.4byte	.LASF224
	.byte	0x5
	.uleb128 0
	.4byte	.LASF225
	.byte	0x5
	.uleb128 0
	.4byte	.LASF226
	.byte	0x5
	.uleb128 0
	.4byte	.LASF227
	.byte	0x5
	.uleb128 0
	.4byte	.LASF228
	.byte	0x5
	.uleb128 0
	.4byte	.LASF229
	.byte	0x5
	.uleb128 0
	.4byte	.LASF230
	.byte	0x5
	.uleb128 0
	.4byte	.LASF231
	.byte	0x5
	.uleb128 0
	.4byte	.LASF232
	.byte	0x5
	.uleb128 0
	.4byte	.LASF233
	.byte	0x5
	.uleb128 0
	.4byte	.LASF234
	.byte	0x5
	.uleb128 0
	.4byte	.LASF235
	.byte	0x5
	.uleb128 0
	.4byte	.LASF236
	.byte	0x5
	.uleb128 0
	.4byte	.LASF237
	.byte	0x5
	.uleb128 0
	.4byte	.LASF238
	.byte	0x5
	.uleb128 0
	.4byte	.LASF239
	.byte	0x5
	.uleb128 0
	.4byte	.LASF240
	.byte	0x5
	.uleb128 0
	.4byte	.LASF241
	.byte	0x5
	.uleb128 0
	.4byte	.LASF242
	.byte	0x5
	.uleb128 0
	.4byte	.LASF243
	.byte	0x5
	.uleb128 0
	.4byte	.LASF244
	.byte	0x5
	.uleb128 0
	.4byte	.LASF245
	.byte	0x5
	.uleb128 0
	.4byte	.LASF246
	.byte	0x5
	.uleb128 0
	.4byte	.LASF247
	.byte	0x5
	.uleb128 0
	.4byte	.LASF248
	.byte	0x5
	.uleb128 0
	.4byte	.LASF249
	.byte	0x5
	.uleb128 0
	.4byte	.LASF250
	.byte	0x5
	.uleb128 0
	.4byte	.LASF251
	.byte	0x5
	.uleb128 0
	.4byte	.LASF252
	.byte	0x5
	.uleb128 0
	.4byte	.LASF253
	.byte	0x5
	.uleb128 0
	.4byte	.LASF254
	.byte	0x5
	.uleb128 0
	.4byte	.LASF255
	.byte	0x5
	.uleb128 0
	.4byte	.LASF256
	.byte	0x5
	.uleb128 0
	.4byte	.LASF257
	.byte	0x5
	.uleb128 0
	.4byte	.LASF258
	.byte	0x5
	.uleb128 0
	.4byte	.LASF259
	.byte	0x5
	.uleb128 0
	.4byte	.LASF260
	.byte	0x5
	.uleb128 0
	.4byte	.LASF261
	.byte	0x5
	.uleb128 0
	.4byte	.LASF262
	.byte	0x5
	.uleb128 0
	.4byte	.LASF263
	.byte	0x5
	.uleb128 0
	.4byte	.LASF264
	.byte	0x5
	.uleb128 0
	.4byte	.LASF265
	.byte	0x5
	.uleb128 0
	.4byte	.LASF266
	.byte	0x5
	.uleb128 0
	.4byte	.LASF267
	.byte	0x5
	.uleb128 0
	.4byte	.LASF268
	.byte	0x5
	.uleb128 0
	.4byte	.LASF269
	.byte	0x5
	.uleb128 0
	.4byte	.LASF270
	.byte	0x5
	.uleb128 0
	.4byte	.LASF271
	.byte	0x5
	.uleb128 0
	.4byte	.LASF272
	.byte	0x5
	.uleb128 0
	.4byte	.LASF273
	.byte	0x5
	.uleb128 0
	.4byte	.LASF274
	.byte	0x5
	.uleb128 0
	.4byte	.LASF275
	.byte	0x5
	.uleb128 0
	.4byte	.LASF276
	.byte	0x5
	.uleb128 0
	.4byte	.LASF277
	.byte	0x5
	.uleb128 0
	.4byte	.LASF278
	.byte	0x5
	.uleb128 0
	.4byte	.LASF279
	.byte	0x5
	.uleb128 0
	.4byte	.LASF280
	.byte	0x5
	.uleb128 0
	.4byte	.LASF281
	.byte	0x5
	.uleb128 0
	.4byte	.LASF282
	.byte	0x5
	.uleb128 0
	.4byte	.LASF283
	.byte	0x5
	.uleb128 0
	.4byte	.LASF284
	.byte	0x5
	.uleb128 0
	.4byte	.LASF285
	.byte	0x5
	.uleb128 0
	.4byte	.LASF286
	.byte	0x5
	.uleb128 0
	.4byte	.LASF287
	.byte	0x5
	.uleb128 0
	.4byte	.LASF288
	.byte	0x5
	.uleb128 0
	.4byte	.LASF289
	.byte	0x5
	.uleb128 0
	.4byte	.LASF290
	.byte	0x5
	.uleb128 0
	.4byte	.LASF291
	.byte	0x5
	.uleb128 0
	.4byte	.LASF292
	.byte	0x5
	.uleb128 0
	.4byte	.LASF293
	.byte	0x5
	.uleb128 0
	.4byte	.LASF294
	.byte	0x5
	.uleb128 0
	.4byte	.LASF295
	.byte	0x5
	.uleb128 0
	.4byte	.LASF296
	.byte	0x5
	.uleb128 0
	.4byte	.LASF297
	.byte	0x5
	.uleb128 0
	.4byte	.LASF298
	.byte	0x5
	.uleb128 0
	.4byte	.LASF299
	.byte	0x5
	.uleb128 0
	.4byte	.LASF300
	.byte	0x5
	.uleb128 0
	.4byte	.LASF301
	.byte	0x5
	.uleb128 0
	.4byte	.LASF302
	.byte	0x5
	.uleb128 0
	.4byte	.LASF303
	.byte	0x5
	.uleb128 0
	.4byte	.LASF304
	.byte	0x5
	.uleb128 0
	.4byte	.LASF305
	.byte	0x5
	.uleb128 0
	.4byte	.LASF306
	.byte	0x5
	.uleb128 0
	.4byte	.LASF307
	.byte	0x5
	.uleb128 0
	.4byte	.LASF308
	.byte	0x5
	.uleb128 0
	.4byte	.LASF309
	.byte	0x5
	.uleb128 0
	.4byte	.LASF310
	.byte	0x5
	.uleb128 0
	.4byte	.LASF311
	.byte	0x5
	.uleb128 0
	.4byte	.LASF312
	.byte	0x5
	.uleb128 0
	.4byte	.LASF313
	.byte	0x5
	.uleb128 0
	.4byte	.LASF314
	.byte	0x5
	.uleb128 0
	.4byte	.LASF315
	.byte	0x5
	.uleb128 0
	.4byte	.LASF316
	.byte	0x5
	.uleb128 0
	.4byte	.LASF317
	.byte	0x5
	.uleb128 0
	.4byte	.LASF318
	.byte	0x5
	.uleb128 0
	.4byte	.LASF319
	.byte	0x5
	.uleb128 0
	.4byte	.LASF320
	.byte	0x5
	.uleb128 0
	.4byte	.LASF321
	.byte	0x5
	.uleb128 0
	.4byte	.LASF322
	.byte	0x5
	.uleb128 0
	.4byte	.LASF323
	.byte	0x5
	.uleb128 0
	.4byte	.LASF324
	.byte	0x5
	.uleb128 0
	.4byte	.LASF325
	.byte	0x5
	.uleb128 0
	.4byte	.LASF326
	.byte	0x5
	.uleb128 0
	.4byte	.LASF327
	.byte	0x5
	.uleb128 0
	.4byte	.LASF328
	.byte	0x5
	.uleb128 0
	.4byte	.LASF329
	.byte	0x5
	.uleb128 0
	.4byte	.LASF330
	.byte	0x5
	.uleb128 0
	.4byte	.LASF331
	.byte	0x5
	.uleb128 0
	.4byte	.LASF332
	.byte	0x5
	.uleb128 0
	.4byte	.LASF333
	.byte	0x5
	.uleb128 0
	.4byte	.LASF334
	.byte	0x5
	.uleb128 0
	.4byte	.LASF335
	.byte	0x5
	.uleb128 0
	.4byte	.LASF336
	.byte	0x5
	.uleb128 0
	.4byte	.LASF337
	.byte	0x5
	.uleb128 0
	.4byte	.LASF338
	.byte	0x5
	.uleb128 0
	.4byte	.LASF339
	.byte	0x5
	.uleb128 0
	.4byte	.LASF340
	.byte	0x5
	.uleb128 0
	.4byte	.LASF341
	.byte	0x5
	.uleb128 0
	.4byte	.LASF342
	.byte	0x5
	.uleb128 0
	.4byte	.LASF343
	.byte	0x5
	.uleb128 0
	.4byte	.LASF344
	.byte	0x5
	.uleb128 0
	.4byte	.LASF345
	.byte	0x5
	.uleb128 0
	.4byte	.LASF346
	.byte	0x5
	.uleb128 0
	.4byte	.LASF347
	.byte	0x5
	.uleb128 0
	.4byte	.LASF348
	.byte	0x5
	.uleb128 0
	.4byte	.LASF349
	.byte	0x5
	.uleb128 0
	.4byte	.LASF350
	.byte	0x5
	.uleb128 0
	.4byte	.LASF351
	.byte	0x5
	.uleb128 0
	.4byte	.LASF352
	.byte	0x5
	.uleb128 0
	.4byte	.LASF353
	.byte	0x5
	.uleb128 0
	.4byte	.LASF354
	.byte	0x5
	.uleb128 0
	.4byte	.LASF355
	.byte	0x5
	.uleb128 0
	.4byte	.LASF356
	.byte	0x5
	.uleb128 0
	.4byte	.LASF357
	.byte	0x5
	.uleb128 0
	.4byte	.LASF358
	.byte	0x5
	.uleb128 0
	.4byte	.LASF359
	.byte	0x5
	.uleb128 0
	.4byte	.LASF360
	.byte	0x5
	.uleb128 0
	.4byte	.LASF361
	.byte	0x5
	.uleb128 0
	.4byte	.LASF362
	.byte	0x5
	.uleb128 0
	.4byte	.LASF363
	.byte	0x5
	.uleb128 0
	.4byte	.LASF364
	.byte	0x5
	.uleb128 0
	.4byte	.LASF365
	.byte	0x5
	.uleb128 0
	.4byte	.LASF366
	.byte	0x5
	.uleb128 0
	.4byte	.LASF367
	.byte	0x5
	.uleb128 0
	.4byte	.LASF368
	.byte	0x5
	.uleb128 0
	.4byte	.LASF369
	.byte	0x5
	.uleb128 0
	.4byte	.LASF370
	.byte	0x5
	.uleb128 0
	.4byte	.LASF371
	.byte	0x5
	.uleb128 0
	.4byte	.LASF372
	.byte	0x5
	.uleb128 0
	.4byte	.LASF373
	.byte	0x5
	.uleb128 0
	.4byte	.LASF374
	.byte	0x5
	.uleb128 0
	.4byte	.LASF375
	.byte	0x5
	.uleb128 0
	.4byte	.LASF376
	.byte	0x5
	.uleb128 0
	.4byte	.LASF377
	.byte	0x5
	.uleb128 0
	.4byte	.LASF378
	.byte	0x5
	.uleb128 0
	.4byte	.LASF379
	.byte	0x5
	.uleb128 0
	.4byte	.LASF380
	.byte	0x5
	.uleb128 0
	.4byte	.LASF381
	.byte	0x5
	.uleb128 0
	.4byte	.LASF382
	.byte	0x5
	.uleb128 0
	.4byte	.LASF383
	.byte	0x5
	.uleb128 0
	.4byte	.LASF384
	.byte	0x5
	.uleb128 0
	.4byte	.LASF385
	.byte	0x5
	.uleb128 0
	.4byte	.LASF386
	.byte	0x5
	.uleb128 0
	.4byte	.LASF387
	.byte	0x5
	.uleb128 0
	.4byte	.LASF388
	.byte	0x5
	.uleb128 0
	.4byte	.LASF389
	.byte	0x5
	.uleb128 0
	.4byte	.LASF390
	.byte	0x5
	.uleb128 0
	.4byte	.LASF391
	.byte	0x5
	.uleb128 0
	.4byte	.LASF392
	.byte	0x5
	.uleb128 0
	.4byte	.LASF393
	.byte	0x6
	.uleb128 0
	.4byte	.LASF394
	.byte	0x5
	.uleb128 0
	.4byte	.LASF395
	.byte	0x5
	.uleb128 0
	.4byte	.LASF396
	.byte	0x6
	.uleb128 0
	.4byte	.LASF397
	.byte	0x5
	.uleb128 0
	.4byte	.LASF398
	.byte	0x6
	.uleb128 0
	.4byte	.LASF399
	.byte	0x6
	.uleb128 0
	.4byte	.LASF400
	.byte	0x6
	.uleb128 0
	.4byte	.LASF401
	.byte	0x6
	.uleb128 0
	.4byte	.LASF402
	.byte	0x5
	.uleb128 0
	.4byte	.LASF403
	.byte	0x6
	.uleb128 0
	.4byte	.LASF404
	.byte	0x6
	.uleb128 0
	.4byte	.LASF405
	.byte	0x5
	.uleb128 0
	.4byte	.LASF406
	.byte	0x5
	.uleb128 0
	.4byte	.LASF407
	.byte	0x6
	.uleb128 0
	.4byte	.LASF408
	.byte	0x6
	.uleb128 0
	.4byte	.LASF409
	.byte	0x5
	.uleb128 0
	.4byte	.LASF410
	.byte	0x5
	.uleb128 0
	.4byte	.LASF411
	.byte	0x6
	.uleb128 0
	.4byte	.LASF412
	.byte	0x5
	.uleb128 0
	.4byte	.LASF413
	.byte	0x5
	.uleb128 0
	.4byte	.LASF414
	.byte	0x6
	.uleb128 0
	.4byte	.LASF415
	.byte	0x5
	.uleb128 0
	.4byte	.LASF416
	.byte	0x5
	.uleb128 0
	.4byte	.LASF417
	.byte	0x5
	.uleb128 0
	.4byte	.LASF418
	.byte	0x5
	.uleb128 0
	.4byte	.LASF419
	.byte	0x5
	.uleb128 0
	.4byte	.LASF420
	.byte	0x6
	.uleb128 0
	.4byte	.LASF421
	.byte	0x5
	.uleb128 0
	.4byte	.LASF422
	.byte	0x5
	.uleb128 0
	.4byte	.LASF423
	.byte	0x5
	.uleb128 0
	.4byte	.LASF424
	.byte	0x5
	.uleb128 0
	.4byte	.LASF425
	.byte	0x6
	.uleb128 0
	.4byte	.LASF426
	.byte	0x6
	.uleb128 0
	.4byte	.LASF427
	.byte	0x6
	.uleb128 0
	.4byte	.LASF428
	.byte	0x6
	.uleb128 0
	.4byte	.LASF429
	.byte	0x6
	.uleb128 0
	.4byte	.LASF430
	.byte	0x6
	.uleb128 0
	.4byte	.LASF431
	.byte	0x6
	.uleb128 0
	.4byte	.LASF432
	.byte	0x6
	.uleb128 0
	.4byte	.LASF433
	.byte	0x6
	.uleb128 0
	.4byte	.LASF434
	.byte	0x6
	.uleb128 0
	.4byte	.LASF435
	.byte	0x6
	.uleb128 0
	.4byte	.LASF436
	.byte	0x5
	.uleb128 0
	.4byte	.LASF437
	.byte	0x5
	.uleb128 0
	.4byte	.LASF438
	.byte	0x5
	.uleb128 0
	.4byte	.LASF439
	.byte	0x5
	.uleb128 0
	.4byte	.LASF440
	.byte	0x5
	.uleb128 0
	.4byte	.LASF441
	.byte	0x5
	.uleb128 0
	.4byte	.LASF442
	.byte	0x5
	.uleb128 0
	.4byte	.LASF443
	.byte	0x6
	.uleb128 0
	.4byte	.LASF444
	.byte	0x5
	.uleb128 0
	.4byte	.LASF445
	.byte	0x5
	.uleb128 0
	.4byte	.LASF446
	.byte	0x5
	.uleb128 0
	.4byte	.LASF447
	.byte	0x5
	.uleb128 0
	.4byte	.LASF448
	.byte	0x5
	.uleb128 0
	.4byte	.LASF449
	.byte	0x5
	.uleb128 0
	.4byte	.LASF450
	.byte	0x5
	.uleb128 0
	.4byte	.LASF451
	.byte	0x5
	.uleb128 0
	.4byte	.LASF452
	.byte	0x5
	.uleb128 0
	.4byte	.LASF453
	.byte	0x5
	.uleb128 0
	.4byte	.LASF454
	.byte	0x5
	.uleb128 0
	.4byte	.LASF455
	.byte	0x5
	.uleb128 0
	.4byte	.LASF456
	.byte	0x5
	.uleb128 0
	.4byte	.LASF457
	.byte	0x5
	.uleb128 0
	.4byte	.LASF458
	.byte	0
	.section	.debug_macro,"G",%progbits,wm4.stdbool.h.29.07dce69c3b78884144b7f7bd19483461,comdat
.Ldebug_macro3:
	.2byte	0x4
	.byte	0
	.byte	0x5
	.uleb128 0x1d
	.4byte	.LASF460
	.byte	0x5
	.uleb128 0x21
	.4byte	.LASF461
	.byte	0x5
	.uleb128 0x22
	.4byte	.LASF462
	.byte	0x5
	.uleb128 0x23
	.4byte	.LASF463
	.byte	0x5
	.uleb128 0x34
	.4byte	.LASF464
	.byte	0
	.section	.debug_macro,"G",%progbits,wm4.circbuffer.h.37.7625611e66bfcc1536a90ead4ef124ff,comdat
.Ldebug_macro4:
	.2byte	0x4
	.byte	0
	.byte	0x5
	.uleb128 0x25
	.4byte	.LASF465
	.byte	0x5
	.uleb128 0x26
	.4byte	.LASF466
	.byte	0x5
	.uleb128 0x29
	.4byte	.LASF467
	.byte	0
	.section	.debug_macro,"G",%progbits,wm4._newlib_version.h.4.1ef4e12f167f8b69d7c30054be56050d,comdat
.Ldebug_macro5:
	.2byte	0x4
	.byte	0
	.byte	0x5
	.uleb128 0x4
	.4byte	.LASF471
	.byte	0x5
	.uleb128 0x6
	.4byte	.LASF472
	.byte	0x5
	.uleb128 0x7
	.4byte	.LASF473
	.byte	0x5
	.uleb128 0x8
	.4byte	.LASF474
	.byte	0x5
	.uleb128 0x9
	.4byte	.LASF475
	.byte	0
	.section	.debug_macro,"G",%progbits,wm4.features.h.33.5bd1eea2d80518b50f7af0b5ce6750e0,comdat
.Ldebug_macro6:
	.2byte	0x4
	.byte	0
	.byte	0x5
	.uleb128 0x21
	.4byte	.LASF476
	.byte	0x5
	.uleb128 0x28
	.4byte	.LASF477
	.byte	0x6
	.uleb128 0x83
	.4byte	.LASF478
	.byte	0x5
	.uleb128 0x84
	.4byte	.LASF479
	.byte	0x6
	.uleb128 0x88
	.4byte	.LASF480
	.byte	0x5
	.uleb128 0x89
	.4byte	.LASF481
	.byte	0x6
	.uleb128 0x8a
	.4byte	.LASF482
	.byte	0x5
	.uleb128 0x8b
	.4byte	.LASF483
	.byte	0x6
	.uleb128 0x9e
	.4byte	.LASF484
	.byte	0x5
	.uleb128 0x9f
	.4byte	.LASF485
	.byte	0x5
	.uleb128 0xf7
	.4byte	.LASF486
	.byte	0x5
	.uleb128 0xfd
	.4byte	.LASF487
	.byte	0x5
	.uleb128 0x105
	.4byte	.LASF488
	.byte	0x5
	.uleb128 0x10d
	.4byte	.LASF489
	.byte	0x5
	.uleb128 0x115
	.4byte	.LASF490
	.byte	0x5
	.uleb128 0x119
	.4byte	.LASF491
	.byte	0x5
	.uleb128 0x11f
	.4byte	.LASF492
	.byte	0x5
	.uleb128 0x12f
	.4byte	.LASF493
	.byte	0x5
	.uleb128 0x13f
	.4byte	.LASF494
	.byte	0x5
	.uleb128 0x14a
	.4byte	.LASF495
	.byte	0
	.section	.debug_macro,"G",%progbits,wm4._default_types.h.15.247e5cd201eca3442cbf5404108c4935,comdat
.Ldebug_macro7:
	.2byte	0x4
	.byte	0
	.byte	0x5
	.uleb128 0xf
	.4byte	.LASF496
	.byte	0x5
	.uleb128 0x1a
	.4byte	.LASF497
	.byte	0x5
	.uleb128 0x21
	.4byte	.LASF498
	.byte	0x5
	.uleb128 0x2f
	.4byte	.LASF499
	.byte	0x5
	.uleb128 0x3d
	.4byte	.LASF500
	.byte	0x5
	.uleb128 0x53
	.4byte	.LASF501
	.byte	0x5
	.uleb128 0x6d
	.4byte	.LASF502
	.byte	0x5
	.uleb128 0x8c
	.4byte	.LASF503
	.byte	0x5
	.uleb128 0xa6
	.4byte	.LASF504
	.byte	0x5
	.uleb128 0xbc
	.4byte	.LASF505
	.byte	0x5
	.uleb128 0xce
	.4byte	.LASF506
	.byte	0x6
	.uleb128 0xf4
	.4byte	.LASF507
	.byte	0
	.section	.debug_macro,"G",%progbits,wm4._intsup.h.10.48bafbb683905c4daa4565a85aeeb264,comdat
.Ldebug_macro8:
	.2byte	0x4
	.byte	0
	.byte	0x5
	.uleb128 0xa
	.4byte	.LASF508
	.byte	0x5
	.uleb128 0x10
	.4byte	.LASF509
	.byte	0x6
	.uleb128 0x2b
	.4byte	.LASF510
	.byte	0x6
	.uleb128 0x2c
	.4byte	.LASF511
	.byte	0x6
	.uleb128 0x2d
	.4byte	.LASF512
	.byte	0x6
	.uleb128 0x2e
	.4byte	.LASF513
	.byte	0x2
	.uleb128 0x2f
	.ascii	"int\000"
	.byte	0x6
	.uleb128 0x30
	.4byte	.LASF514
	.byte	0x6
	.uleb128 0x31
	.4byte	.LASF515
	.byte	0x6
	.uleb128 0x32
	.4byte	.LASF516
	.byte	0x5
	.uleb128 0x33
	.4byte	.LASF517
	.byte	0x5
	.uleb128 0x34
	.4byte	.LASF518
	.byte	0x5
	.uleb128 0x35
	.4byte	.LASF519
	.byte	0x5
	.uleb128 0x36
	.4byte	.LASF520
	.byte	0x5
	.uleb128 0x37
	.4byte	.LASF521
	.byte	0x5
	.uleb128 0x38
	.4byte	.LASF522
	.byte	0x5
	.uleb128 0x39
	.4byte	.LASF523
	.byte	0x5
	.uleb128 0x3a
	.4byte	.LASF524
	.byte	0x5
	.uleb128 0x43
	.4byte	.LASF525
	.byte	0x5
	.uleb128 0x4a
	.4byte	.LASF526
	.byte	0x5
	.uleb128 0x52
	.4byte	.LASF527
	.byte	0x5
	.uleb128 0x5d
	.4byte	.LASF528
	.byte	0x5
	.uleb128 0x68
	.4byte	.LASF529
	.byte	0x5
	.uleb128 0x71
	.4byte	.LASF530
	.byte	0x5
	.uleb128 0x78
	.4byte	.LASF531
	.byte	0x5
	.uleb128 0x81
	.4byte	.LASF532
	.byte	0x5
	.uleb128 0x88
	.4byte	.LASF533
	.byte	0x5
	.uleb128 0x93
	.4byte	.LASF534
	.byte	0x5
	.uleb128 0x97
	.4byte	.LASF535
	.byte	0x5
	.uleb128 0xa2
	.4byte	.LASF536
	.byte	0x5
	.uleb128 0xad
	.4byte	.LASF537
	.byte	0x5
	.uleb128 0xb6
	.4byte	.LASF538
	.byte	0x6
	.uleb128 0xb8
	.4byte	.LASF510
	.byte	0x6
	.uleb128 0xb9
	.4byte	.LASF511
	.byte	0x6
	.uleb128 0xba
	.4byte	.LASF512
	.byte	0x6
	.uleb128 0xbb
	.4byte	.LASF513
	.byte	0x2
	.uleb128 0xbc
	.ascii	"int\000"
	.byte	0x6
	.uleb128 0xbd
	.4byte	.LASF516
	.byte	0x6
	.uleb128 0xc2
	.4byte	.LASF514
	.byte	0x6
	.uleb128 0xc3
	.4byte	.LASF515
	.byte	0
	.section	.debug_macro,"G",%progbits,wm4._stdint.h.10.c24fa3af3bc1706662bb5593a907e841,comdat
.Ldebug_macro9:
	.2byte	0x4
	.byte	0
	.byte	0x5
	.uleb128 0xa
	.4byte	.LASF539
	.byte	0x5
	.uleb128 0x15
	.4byte	.LASF540
	.byte	0x5
	.uleb128 0x19
	.4byte	.LASF541
	.byte	0x5
	.uleb128 0x1b
	.4byte	.LASF542
	.byte	0x5
	.uleb128 0x21
	.4byte	.LASF543
	.byte	0x5
	.uleb128 0x25
	.4byte	.LASF544
	.byte	0x5
	.uleb128 0x27
	.4byte	.LASF545
	.byte	0x5
	.uleb128 0x2d
	.4byte	.LASF546
	.byte	0x5
	.uleb128 0x31
	.4byte	.LASF547
	.byte	0x5
	.uleb128 0x33
	.4byte	.LASF548
	.byte	0x5
	.uleb128 0x39
	.4byte	.LASF549
	.byte	0x5
	.uleb128 0x3d
	.4byte	.LASF550
	.byte	0x5
	.uleb128 0x3f
	.4byte	.LASF551
	.byte	0x5
	.uleb128 0x44
	.4byte	.LASF552
	.byte	0x5
	.uleb128 0x49
	.4byte	.LASF553
	.byte	0x5
	.uleb128 0x4e
	.4byte	.LASF554
	.byte	0x5
	.uleb128 0x53
	.4byte	.LASF555
	.byte	0
	.section	.debug_macro,"G",%progbits,wm4.stdint.h.23.d53047a68f4a85177f80b422d52785ed,comdat
.Ldebug_macro10:
	.2byte	0x4
	.byte	0
	.byte	0x5
	.uleb128 0x17
	.4byte	.LASF556
	.byte	0x5
	.uleb128 0x1d
	.4byte	.LASF557
	.byte	0x5
	.uleb128 0x23
	.4byte	.LASF558
	.byte	0x5
	.uleb128 0x29
	.4byte	.LASF559
	.byte	0x5
	.uleb128 0x35
	.4byte	.LASF560
	.byte	0x5
	.uleb128 0x3f
	.4byte	.LASF561
	.byte	0x5
	.uleb128 0x49
	.4byte	.LASF562
	.byte	0x5
	.uleb128 0x53
	.4byte	.LASF563
	.byte	0x5
	.uleb128 0x80
	.4byte	.LASF564
	.byte	0x5
	.uleb128 0x81
	.4byte	.LASF565
	.byte	0x5
	.uleb128 0x82
	.4byte	.LASF566
	.byte	0x5
	.uleb128 0x98
	.4byte	.LASF567
	.byte	0x5
	.uleb128 0x99
	.4byte	.LASF568
	.byte	0x5
	.uleb128 0x9a
	.4byte	.LASF569
	.byte	0x5
	.uleb128 0xa2
	.4byte	.LASF570
	.byte	0x5
	.uleb128 0xa3
	.4byte	.LASF571
	.byte	0x5
	.uleb128 0xa4
	.4byte	.LASF572
	.byte	0x5
	.uleb128 0xae
	.4byte	.LASF573
	.byte	0x5
	.uleb128 0xaf
	.4byte	.LASF574
	.byte	0x5
	.uleb128 0xb0
	.4byte	.LASF575
	.byte	0x5
	.uleb128 0xb8
	.4byte	.LASF576
	.byte	0x5
	.uleb128 0xb9
	.4byte	.LASF577
	.byte	0x5
	.uleb128 0xba
	.4byte	.LASF578
	.byte	0x5
	.uleb128 0xc4
	.4byte	.LASF579
	.byte	0x5
	.uleb128 0xc5
	.4byte	.LASF580
	.byte	0x5
	.uleb128 0xc6
	.4byte	.LASF581
	.byte	0x5
	.uleb128 0xd4
	.4byte	.LASF582
	.byte	0x5
	.uleb128 0xd5
	.4byte	.LASF583
	.byte	0x5
	.uleb128 0xd6
	.4byte	.LASF584
	.byte	0x5
	.uleb128 0xe6
	.4byte	.LASF585
	.byte	0x5
	.uleb128 0xe7
	.4byte	.LASF586
	.byte	0x5
	.uleb128 0xe8
	.4byte	.LASF587
	.byte	0x5
	.uleb128 0xf6
	.4byte	.LASF588
	.byte	0x5
	.uleb128 0xf7
	.4byte	.LASF589
	.byte	0x5
	.uleb128 0xf8
	.4byte	.LASF590
	.byte	0x5
	.uleb128 0x106
	.4byte	.LASF591
	.byte	0x5
	.uleb128 0x107
	.4byte	.LASF592
	.byte	0x5
	.uleb128 0x108
	.4byte	.LASF593
	.byte	0x5
	.uleb128 0x116
	.4byte	.LASF594
	.byte	0x5
	.uleb128 0x117
	.4byte	.LASF595
	.byte	0x5
	.uleb128 0x118
	.4byte	.LASF596
	.byte	0x5
	.uleb128 0x126
	.4byte	.LASF597
	.byte	0x5
	.uleb128 0x127
	.4byte	.LASF598
	.byte	0x5
	.uleb128 0x128
	.4byte	.LASF599
	.byte	0x5
	.uleb128 0x136
	.4byte	.LASF600
	.byte	0x5
	.uleb128 0x137
	.4byte	.LASF601
	.byte	0x5
	.uleb128 0x138
	.4byte	.LASF602
	.byte	0x5
	.uleb128 0x146
	.4byte	.LASF603
	.byte	0x5
	.uleb128 0x147
	.4byte	.LASF604
	.byte	0x5
	.uleb128 0x14f
	.4byte	.LASF605
	.byte	0x5
	.uleb128 0x157
	.4byte	.LASF606
	.byte	0x5
	.uleb128 0x15d
	.4byte	.LASF607
	.byte	0x5
	.uleb128 0x15e
	.4byte	.LASF608
	.byte	0x5
	.uleb128 0x162
	.4byte	.LASF609
	.byte	0x5
	.uleb128 0x166
	.4byte	.LASF610
	.byte	0x5
	.uleb128 0x16b
	.4byte	.LASF611
	.byte	0x5
	.uleb128 0x176
	.4byte	.LASF612
	.byte	0x5
	.uleb128 0x180
	.4byte	.LASF613
	.byte	0x5
	.uleb128 0x185
	.4byte	.LASF614
	.byte	0x5
	.uleb128 0x18c
	.4byte	.LASF615
	.byte	0x5
	.uleb128 0x18d
	.4byte	.LASF616
	.byte	0x5
	.uleb128 0x198
	.4byte	.LASF617
	.byte	0x5
	.uleb128 0x199
	.4byte	.LASF618
	.byte	0x5
	.uleb128 0x1a4
	.4byte	.LASF619
	.byte	0x5
	.uleb128 0x1a5
	.4byte	.LASF620
	.byte	0x5
	.uleb128 0x1b1
	.4byte	.LASF621
	.byte	0x5
	.uleb128 0x1b2
	.4byte	.LASF622
	.byte	0x5
	.uleb128 0x1c1
	.4byte	.LASF623
	.byte	0x5
	.uleb128 0x1c2
	.4byte	.LASF624
	.byte	0
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.section	.debug_str,"MS",%progbits,1
.LASF652:
	.ascii	"naeusb/circbuffer.c\000"
.LASF431:
	.ascii	"__ARM_FEATURE_FP16_VECTOR_ARITHMETIC\000"
.LASF96:
	.ascii	"__SIG_ATOMIC_MAX__ 0x7fffffff\000"
.LASF218:
	.ascii	"__FLT64_HAS_QUIET_NAN__ 1\000"
.LASF246:
	.ascii	"__DEC64_SUBNORMAL_MIN__ 0.000000000000001E-383DD\000"
.LASF270:
	.ascii	"__UFRACT_IBIT__ 0\000"
.LASF203:
	.ascii	"__FLT32_HAS_INFINITY__ 1\000"
.LASF453:
	.ascii	"UDD_ENABLE 1\000"
.LASF62:
	.ascii	"__UINT_FAST8_TYPE__ unsigned int\000"
.LASF209:
	.ascii	"__FLT64_MAX_EXP__ 1024\000"
.LASF571:
	.ascii	"INT_LEAST8_MAX (__INT_LEAST8_MAX__)\000"
.LASF498:
	.ascii	"__have_long32 1\000"
.LASF223:
	.ascii	"__FLT32X_MAX_EXP__ 1024\000"
.LASF130:
	.ascii	"__INT_FAST16_WIDTH__ 32\000"
.LASF386:
	.ascii	"__GCC_ATOMIC_TEST_AND_SET_TRUEVAL 1\000"
.LASF604:
	.ascii	"INTMAX_MIN (-INTMAX_MAX - 1)\000"
.LASF245:
	.ascii	"__DEC64_EPSILON__ 1E-15DD\000"
.LASF323:
	.ascii	"__ULACCUM_EPSILON__ 0x1P-32ULK\000"
.LASF409:
	.ascii	"__ARM_FEATURE_SIMD32\000"
.LASF191:
	.ascii	"__FLT32_MANT_DIG__ 24\000"
.LASF349:
	.ascii	"__USQ_IBIT__ 0\000"
.LASF588:
	.ascii	"INT_LEAST64_MIN (-__INT_LEAST64_MAX__ - 1)\000"
.LASF120:
	.ascii	"__UINT8_C(c) c\000"
.LASF434:
	.ascii	"__ARM_NEON__\000"
.LASF392:
	.ascii	"__SIZEOF_WINT_T__ 4\000"
.LASF335:
	.ascii	"__QQ_IBIT__ 0\000"
.LASF351:
	.ascii	"__UDQ_IBIT__ 0\000"
.LASF515:
	.ascii	"__int20__\000"
.LASF622:
	.ascii	"UINT64_C(x) __UINT64_C(x)\000"
.LASF456:
	.ascii	"HAL_TYPE HAL_sam3u1c\000"
.LASF584:
	.ascii	"UINT_LEAST32_MAX (__UINT_LEAST32_MAX__)\000"
.LASF225:
	.ascii	"__FLT32X_DECIMAL_DIG__ 17\000"
.LASF494:
	.ascii	"__XSI_VISIBLE 0\000"
.LASF164:
	.ascii	"__DBL_MIN_EXP__ (-1021)\000"
.LASF86:
	.ascii	"__LONG_LONG_WIDTH__ 64\000"
.LASF69:
	.ascii	"__has_include_next(STR) __has_include_next__(STR)\000"
.LASF276:
	.ascii	"__LFRACT_MIN__ (-0.5LR-0.5LR)\000"
.LASF411:
	.ascii	"__ARM_SIZEOF_WCHAR_T 4\000"
.LASF427:
	.ascii	"__ARM_FP16_FORMAT_IEEE\000"
.LASF492:
	.ascii	"__POSIX_VISIBLE 200809\000"
.LASF402:
	.ascii	"__ARM_FEATURE_COMPLEX\000"
.LASF285:
	.ascii	"__LLFRACT_IBIT__ 0\000"
.LASF476:
	.ascii	"__GNUC_PREREQ(maj,min) ((__GNUC__ << 16) + __GNUC_M"
	.ascii	"INOR__ >= ((maj) << 16) + (min))\000"
.LASF169:
	.ascii	"__DBL_MAX__ ((double)1.7976931348623157e+308L)\000"
.LASF641:
	.ascii	"circ_buf_count\000"
.LASF624:
	.ascii	"UINTMAX_C(x) __UINTMAX_C(x)\000"
.LASF261:
	.ascii	"__USFRACT_MIN__ 0.0UHR\000"
.LASF578:
	.ascii	"UINT_LEAST16_MAX (__UINT_LEAST16_MAX__)\000"
.LASF596:
	.ascii	"UINT_FAST16_MAX (__UINT_FAST16_MAX__)\000"
.LASF6:
	.ascii	"__GNUC_MINOR__ 3\000"
.LASF523:
	.ascii	"int +2\000"
.LASF443:
	.ascii	"__ARM_ASM_SYNTAX_UNIFIED__ 1\000"
.LASF634:
	.ascii	"unsigned int\000"
.LASF570:
	.ascii	"INT_LEAST8_MIN (-__INT_LEAST8_MAX__ - 1)\000"
.LASF397:
	.ascii	"__ARM_FEATURE_CRYPTO\000"
.LASF469:
	.ascii	"_MACHINE__DEFAULT_TYPES_H \000"
.LASF122:
	.ascii	"__UINT16_C(c) c\000"
.LASF480:
	.ascii	"_POSIX_SOURCE\000"
.LASF24:
	.ascii	"__SIZEOF_SIZE_T__ 4\000"
.LASF39:
	.ascii	"__CHAR16_TYPE__ short unsigned int\000"
.LASF567:
	.ascii	"INT8_MIN (-__INT8_MAX__ - 1)\000"
.LASF23:
	.ascii	"__SIZEOF_LONG_DOUBLE__ 8\000"
.LASF127:
	.ascii	"__INT_FAST8_MAX__ 0x7fffffff\000"
.LASF28:
	.ascii	"__ORDER_BIG_ENDIAN__ 4321\000"
.LASF292:
	.ascii	"__ULLFRACT_MAX__ 0XFFFFFFFFFFFFFFFFP-64ULLR\000"
.LASF450:
	.ascii	"__SAM3U2E__ 1\000"
.LASF5:
	.ascii	"__GNUC__ 9\000"
.LASF520:
	.ascii	"short +1\000"
.LASF472:
	.ascii	"_NEWLIB_VERSION \"3.3.0\"\000"
.LASF368:
	.ascii	"__UTA_FBIT__ 64\000"
.LASF162:
	.ascii	"__DBL_MANT_DIG__ 53\000"
.LASF57:
	.ascii	"__UINT_LEAST64_TYPE__ long long unsigned int\000"
.LASF73:
	.ascii	"__INT_MAX__ 0x7fffffff\000"
.LASF12:
	.ascii	"__ATOMIC_RELEASE 3\000"
.LASF34:
	.ascii	"__PTRDIFF_TYPE__ int\000"
.LASF373:
	.ascii	"__CHAR_UNSIGNED__ 1\000"
.LASF43:
	.ascii	"__INT16_TYPE__ short int\000"
.LASF399:
	.ascii	"__ARM_FEATURE_QRDMX\000"
.LASF238:
	.ascii	"__DEC32_EPSILON__ 1E-6DF\000"
.LASF177:
	.ascii	"__LDBL_DIG__ 15\000"
.LASF18:
	.ascii	"__SIZEOF_LONG__ 4\000"
.LASF198:
	.ascii	"__FLT32_MAX__ 3.4028234663852886e+38F32\000"
.LASF325:
	.ascii	"__LLACCUM_IBIT__ 32\000"
.LASF474:
	.ascii	"__NEWLIB_MINOR__ 3\000"
.LASF10:
	.ascii	"__ATOMIC_SEQ_CST 5\000"
.LASF459:
	.ascii	"CIRCBUFFER_H \000"
.LASF20:
	.ascii	"__SIZEOF_SHORT__ 2\000"
.LASF471:
	.ascii	"_NEWLIB_VERSION_H__ 1\000"
.LASF107:
	.ascii	"__INT_LEAST8_MAX__ 0x7f\000"
.LASF50:
	.ascii	"__INT_LEAST8_TYPE__ signed char\000"
.LASF205:
	.ascii	"__FLT64_MANT_DIG__ 53\000"
.LASF94:
	.ascii	"__UINTMAX_C(c) c ## ULL\000"
.LASF619:
	.ascii	"INT32_C(x) __INT32_C(x)\000"
.LASF473:
	.ascii	"__NEWLIB__ 3\000"
.LASF30:
	.ascii	"__BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__\000"
.LASF239:
	.ascii	"__DEC32_SUBNORMAL_MIN__ 0.000001E-95DF\000"
.LASF654:
	.ascii	"add_to_circ_buf\000"
.LASF194:
	.ascii	"__FLT32_MIN_10_EXP__ (-37)\000"
.LASF514:
	.ascii	"__int20\000"
.LASF275:
	.ascii	"__LFRACT_IBIT__ 0\000"
.LASF487:
	.ascii	"__BSD_VISIBLE 1\000"
.LASF519:
	.ascii	"char +0\000"
.LASF416:
	.ascii	"__ARM_ARCH 7\000"
.LASF3:
	.ascii	"__STDC_UTF_32__ 1\000"
.LASF590:
	.ascii	"UINT_LEAST64_MAX (__UINT_LEAST64_MAX__)\000"
.LASF110:
	.ascii	"__INT_LEAST16_MAX__ 0x7fff\000"
.LASF565:
	.ascii	"INTPTR_MAX (__INTPTR_MAX__)\000"
.LASF71:
	.ascii	"__SCHAR_MAX__ 0x7f\000"
.LASF646:
	.ascii	"get_from_circ_buf\000"
.LASF253:
	.ascii	"__DEC128_SUBNORMAL_MIN__ 0.000000000000000000000000"
	.ascii	"000000001E-6143DL\000"
.LASF290:
	.ascii	"__ULLFRACT_IBIT__ 0\000"
.LASF206:
	.ascii	"__FLT64_DIG__ 15\000"
.LASF533:
	.ascii	"__FAST32 \000"
.LASF9:
	.ascii	"__ATOMIC_RELAXED 0\000"
.LASF52:
	.ascii	"__INT_LEAST32_TYPE__ long int\000"
.LASF432:
	.ascii	"__ARM_FEATURE_FP16_FML\000"
.LASF81:
	.ascii	"__SIZE_MAX__ 0xffffffffU\000"
.LASF547:
	.ascii	"_UINT32_T_DECLARED \000"
.LASF316:
	.ascii	"__LACCUM_MIN__ (-0X1P31LK-0X1P31LK)\000"
.LASF332:
	.ascii	"__ULLACCUM_MAX__ 0XFFFFFFFFFFFFFFFFP-32ULLK\000"
.LASF321:
	.ascii	"__ULACCUM_MIN__ 0.0ULK\000"
.LASF160:
	.ascii	"__FLT_HAS_INFINITY__ 1\000"
.LASF484:
	.ascii	"_ATFILE_SOURCE\000"
.LASF457:
	.ascii	"PLATFORM SAM3U1C\000"
.LASF200:
	.ascii	"__FLT32_EPSILON__ 1.1920928955078125e-7F32\000"
.LASF541:
	.ascii	"_UINT8_T_DECLARED \000"
.LASF61:
	.ascii	"__INT_FAST64_TYPE__ long long int\000"
.LASF108:
	.ascii	"__INT8_C(c) c\000"
.LASF419:
	.ascii	"__thumb2__ 1\000"
.LASF403:
	.ascii	"__ARM_32BIT_STATE 1\000"
.LASF112:
	.ascii	"__INT_LEAST16_WIDTH__ 16\000"
.LASF544:
	.ascii	"_UINT16_T_DECLARED \000"
.LASF538:
	.ascii	"__LEAST64 \"ll\"\000"
.LASF511:
	.ascii	"unsigned\000"
.LASF331:
	.ascii	"__ULLACCUM_MIN__ 0.0ULLK\000"
.LASF651:
	.ascii	"GNU C99 9.3.1 20200408 (release) -mcpu=cortex-m3 -m"
	.ascii	"long-calls -mfloat-abi=soft -mthumb -march=armv7-m "
	.ascii	"-g3 -gdwarf-2 -Og -std=gnu99 -fdata-sections -ffunc"
	.ascii	"tion-sections -fno-strict-aliasing -funsigned-char "
	.ascii	"-funsigned-bitfields -fshort-enums --param max-inli"
	.ascii	"ne-insns-single=500\000"
.LASF625:
	.ascii	"_GCC_WRAP_STDINT_H \000"
.LASF401:
	.ascii	"__ARM_FEATURE_DOTPROD\000"
.LASF157:
	.ascii	"__FLT_EPSILON__ 1.1920928955078125e-7F\000"
.LASF435:
	.ascii	"__ARM_NEON\000"
.LASF461:
	.ascii	"bool _Bool\000"
.LASF404:
	.ascii	"__ARM_FEATURE_CMSE\000"
.LASF67:
	.ascii	"__UINTPTR_TYPE__ unsigned int\000"
.LASF202:
	.ascii	"__FLT32_HAS_DENORM__ 1\000"
.LASF215:
	.ascii	"__FLT64_DENORM_MIN__ 4.9406564584124654e-324F64\000"
.LASF374:
	.ascii	"__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1 1\000"
.LASF357:
	.ascii	"__SA_IBIT__ 16\000"
.LASF287:
	.ascii	"__LLFRACT_MAX__ 0X7FFFFFFFFFFFFFFFP-63LLR\000"
.LASF608:
	.ascii	"SIG_ATOMIC_MAX (__STDINT_EXP(INT_MAX))\000"
.LASF133:
	.ascii	"__INT_FAST64_MAX__ 0x7fffffffffffffffLL\000"
.LASF495:
	.ascii	"__SSP_FORTIFY_LEVEL 0\000"
.LASF430:
	.ascii	"__ARM_FEATURE_FP16_SCALAR_ARITHMETIC\000"
.LASF413:
	.ascii	"__ARM_ARCH_PROFILE 77\000"
.LASF149:
	.ascii	"__FLT_DIG__ 6\000"
.LASF171:
	.ascii	"__DBL_EPSILON__ ((double)2.2204460492503131e-16L)\000"
.LASF377:
	.ascii	"__GCC_ATOMIC_BOOL_LOCK_FREE 2\000"
.LASF375:
	.ascii	"__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2 1\000"
.LASF516:
	.ascii	"long\000"
.LASF358:
	.ascii	"__DA_FBIT__ 31\000"
.LASF410:
	.ascii	"__ARM_SIZEOF_MINIMAL_ENUM 1\000"
.LASF448:
	.ascii	"__USES_INITFINI__ 1\000"
.LASF236:
	.ascii	"__DEC32_MIN__ 1E-95DF\000"
.LASF644:
	.ascii	"count\000"
.LASF142:
	.ascii	"__GCC_IEC_559 0\000"
.LASF633:
	.ascii	"long long unsigned int\000"
.LASF517:
	.ascii	"signed +0\000"
.LASF303:
	.ascii	"__USACCUM_EPSILON__ 0x1P-8UHK\000"
.LASF499:
	.ascii	"___int8_t_defined 1\000"
.LASF80:
	.ascii	"__PTRDIFF_MAX__ 0x7fffffff\000"
.LASF204:
	.ascii	"__FLT32_HAS_QUIET_NAN__ 1\000"
.LASF581:
	.ascii	"UINT32_MAX (__UINT32_MAX__)\000"
.LASF322:
	.ascii	"__ULACCUM_MAX__ 0XFFFFFFFFFFFFFFFFP-32ULK\000"
.LASF272:
	.ascii	"__UFRACT_MAX__ 0XFFFFP-16UR\000"
.LASF257:
	.ascii	"__SFRACT_MAX__ 0X7FP-7HR\000"
.LASF440:
	.ascii	"__ARM_EABI__ 1\000"
.LASF470:
	.ascii	"_SYS_FEATURES_H \000"
.LASF119:
	.ascii	"__UINT_LEAST8_MAX__ 0xff\000"
.LASF102:
	.ascii	"__INT64_MAX__ 0x7fffffffffffffffLL\000"
.LASF99:
	.ascii	"__INT8_MAX__ 0x7f\000"
.LASF283:
	.ascii	"__ULFRACT_EPSILON__ 0x1P-32ULR\000"
.LASF252:
	.ascii	"__DEC128_EPSILON__ 1E-33DL\000"
.LASF280:
	.ascii	"__ULFRACT_IBIT__ 0\000"
.LASF324:
	.ascii	"__LLACCUM_FBIT__ 31\000"
.LASF408:
	.ascii	"__ARM_FEATURE_NUMERIC_MAXMIN\000"
.LASF125:
	.ascii	"__UINT_LEAST64_MAX__ 0xffffffffffffffffULL\000"
.LASF147:
	.ascii	"__FLT_RADIX__ 2\000"
.LASF291:
	.ascii	"__ULLFRACT_MIN__ 0.0ULLR\000"
.LASF175:
	.ascii	"__DBL_HAS_QUIET_NAN__ 1\000"
.LASF311:
	.ascii	"__UACCUM_MIN__ 0.0UK\000"
.LASF278:
	.ascii	"__LFRACT_EPSILON__ 0x1P-31LR\000"
.LASF58:
	.ascii	"__INT_FAST8_TYPE__ int\000"
.LASF577:
	.ascii	"INT_LEAST16_MAX (__INT_LEAST16_MAX__)\000"
.LASF217:
	.ascii	"__FLT64_HAS_INFINITY__ 1\000"
.LASF229:
	.ascii	"__FLT32X_DENORM_MIN__ 4.9406564584124654e-324F32x\000"
.LASF464:
	.ascii	"__bool_true_false_are_defined 1\000"
.LASF294:
	.ascii	"__SACCUM_FBIT__ 7\000"
.LASF35:
	.ascii	"__WCHAR_TYPE__ unsigned int\000"
.LASF170:
	.ascii	"__DBL_MIN__ ((double)2.2250738585072014e-308L)\000"
.LASF213:
	.ascii	"__FLT64_MIN__ 2.2250738585072014e-308F64\000"
.LASF244:
	.ascii	"__DEC64_MAX__ 9.999999999999999E384DD\000"
.LASF266:
	.ascii	"__FRACT_MIN__ (-0.5R-0.5R)\000"
.LASF383:
	.ascii	"__GCC_ATOMIC_INT_LOCK_FREE 2\000"
.LASF91:
	.ascii	"__INTMAX_MAX__ 0x7fffffffffffffffLL\000"
.LASF550:
	.ascii	"_UINT64_T_DECLARED \000"
.LASF313:
	.ascii	"__UACCUM_EPSILON__ 0x1P-16UK\000"
.LASF605:
	.ascii	"UINTMAX_MAX (__UINTMAX_MAX__)\000"
.LASF305:
	.ascii	"__ACCUM_IBIT__ 16\000"
.LASF187:
	.ascii	"__LDBL_DENORM_MIN__ 4.9406564584124654e-324L\000"
.LASF210:
	.ascii	"__FLT64_MAX_10_EXP__ 308\000"
.LASF295:
	.ascii	"__SACCUM_IBIT__ 8\000"
.LASF393:
	.ascii	"__SIZEOF_PTRDIFF_T__ 4\000"
.LASF390:
	.ascii	"__PRAGMA_REDEFINE_EXTNAME 1\000"
.LASF467:
	.ascii	"SERIAL_ERR 0xFF\000"
.LASF256:
	.ascii	"__SFRACT_MIN__ (-0.5HR-0.5HR)\000"
.LASF365:
	.ascii	"__USA_IBIT__ 16\000"
.LASF575:
	.ascii	"UINT16_MAX (__UINT16_MAX__)\000"
.LASF502:
	.ascii	"___int64_t_defined 1\000"
.LASF247:
	.ascii	"__DEC128_MANT_DIG__ 34\000"
.LASF441:
	.ascii	"__ARM_ARCH_EXT_IDIV__ 1\000"
.LASF617:
	.ascii	"INT16_C(x) __INT16_C(x)\000"
.LASF398:
	.ascii	"__ARM_FEATURE_UNALIGNED 1\000"
.LASF483:
	.ascii	"_POSIX_C_SOURCE 200809L\000"
.LASF21:
	.ascii	"__SIZEOF_FLOAT__ 4\000"
.LASF124:
	.ascii	"__UINT32_C(c) c ## UL\000"
.LASF235:
	.ascii	"__DEC32_MAX_EXP__ 97\000"
.LASF597:
	.ascii	"INT_FAST32_MIN (-__INT_FAST32_MAX__ - 1)\000"
.LASF460:
	.ascii	"_STDBOOL_H \000"
.LASF643:
	.ascii	"cbuf\000"
.LASF420:
	.ascii	"__THUMBEL__ 1\000"
.LASF551:
	.ascii	"__int64_t_defined 1\000"
.LASF639:
	.ascii	"dropped\000"
.LASF359:
	.ascii	"__DA_IBIT__ 32\000"
.LASF26:
	.ascii	"__BIGGEST_ALIGNMENT__ 8\000"
.LASF315:
	.ascii	"__LACCUM_IBIT__ 32\000"
.LASF539:
	.ascii	"_SYS__STDINT_H \000"
.LASF262:
	.ascii	"__USFRACT_MAX__ 0XFFP-8UHR\000"
.LASF135:
	.ascii	"__UINT_FAST8_MAX__ 0xffffffffU\000"
.LASF27:
	.ascii	"__ORDER_LITTLE_ENDIAN__ 1234\000"
.LASF388:
	.ascii	"__HAVE_SPECULATION_SAFE_VALUE 1\000"
.LASF326:
	.ascii	"__LLACCUM_MIN__ (-0X1P31LLK-0X1P31LLK)\000"
.LASF15:
	.ascii	"__OPTIMIZE__ 1\000"
.LASF356:
	.ascii	"__SA_FBIT__ 15\000"
.LASF208:
	.ascii	"__FLT64_MIN_10_EXP__ (-307)\000"
.LASF479:
	.ascii	"_DEFAULT_SOURCE 1\000"
.LASF298:
	.ascii	"__SACCUM_EPSILON__ 0x1P-7HK\000"
.LASF501:
	.ascii	"___int32_t_defined 1\000"
.LASF59:
	.ascii	"__INT_FAST16_TYPE__ int\000"
.LASF488:
	.ascii	"__GNU_VISIBLE 0\000"
.LASF197:
	.ascii	"__FLT32_DECIMAL_DIG__ 9\000"
.LASF47:
	.ascii	"__UINT16_TYPE__ short unsigned int\000"
.LASF87:
	.ascii	"__WCHAR_WIDTH__ 32\000"
.LASF372:
	.ascii	"__GNUC_STDC_INLINE__ 1\000"
.LASF347:
	.ascii	"__UHQ_IBIT__ 0\000"
.LASF269:
	.ascii	"__UFRACT_FBIT__ 16\000"
.LASF407:
	.ascii	"__ARM_FEATURE_CLZ 1\000"
.LASF75:
	.ascii	"__LONG_LONG_MAX__ 0x7fffffffffffffffLL\000"
.LASF534:
	.ascii	"__FAST64 \"ll\"\000"
.LASF45:
	.ascii	"__INT64_TYPE__ long long int\000"
.LASF153:
	.ascii	"__FLT_MAX_10_EXP__ 38\000"
.LASF532:
	.ascii	"__FAST16 \000"
.LASF327:
	.ascii	"__LLACCUM_MAX__ 0X7FFFFFFFFFFFFFFFP-31LLK\000"
.LASF328:
	.ascii	"__LLACCUM_EPSILON__ 0x1P-31LLK\000"
.LASF219:
	.ascii	"__FLT32X_MANT_DIG__ 53\000"
.LASF615:
	.ascii	"INT8_C(x) __INT8_C(x)\000"
.LASF248:
	.ascii	"__DEC128_MIN_EXP__ (-6142)\000"
.LASF444:
	.ascii	"__ARM_FEATURE_COPROC\000"
.LASF281:
	.ascii	"__ULFRACT_MIN__ 0.0ULR\000"
.LASF148:
	.ascii	"__FLT_MANT_DIG__ 24\000"
.LASF415:
	.ascii	"__ARM_ARCH\000"
.LASF211:
	.ascii	"__FLT64_DECIMAL_DIG__ 17\000"
.LASF370:
	.ascii	"__REGISTER_PREFIX__ \000"
.LASF49:
	.ascii	"__UINT64_TYPE__ long long unsigned int\000"
.LASF193:
	.ascii	"__FLT32_MIN_EXP__ (-125)\000"
.LASF310:
	.ascii	"__UACCUM_IBIT__ 16\000"
.LASF136:
	.ascii	"__UINT_FAST16_MAX__ 0xffffffffU\000"
.LASF63:
	.ascii	"__UINT_FAST16_TYPE__ unsigned int\000"
.LASF165:
	.ascii	"__DBL_MIN_10_EXP__ (-307)\000"
.LASF300:
	.ascii	"__USACCUM_IBIT__ 8\000"
.LASF475:
	.ascii	"__NEWLIB_PATCHLEVEL__ 0\000"
.LASF70:
	.ascii	"__GXX_ABI_VERSION 1013\000"
.LASF367:
	.ascii	"__UDA_IBIT__ 32\000"
.LASF192:
	.ascii	"__FLT32_DIG__ 6\000"
.LASF454:
	.ascii	"scanf iscanf\000"
.LASF216:
	.ascii	"__FLT64_HAS_DENORM__ 1\000"
.LASF506:
	.ascii	"___int_least64_t_defined 1\000"
.LASF144:
	.ascii	"__FLT_EVAL_METHOD__ 0\000"
.LASF458:
	.ascii	"F_CPU 7372800UL\000"
.LASF230:
	.ascii	"__FLT32X_HAS_DENORM__ 1\000"
.LASF637:
	.ascii	"head\000"
.LASF525:
	.ascii	"_INTPTR_EQ_INT \000"
.LASF116:
	.ascii	"__INT_LEAST64_MAX__ 0x7fffffffffffffffLL\000"
.LASF579:
	.ascii	"INT32_MIN (-__INT32_MAX__ - 1)\000"
.LASF304:
	.ascii	"__ACCUM_FBIT__ 15\000"
.LASF508:
	.ascii	"_SYS__INTSUP_H \000"
.LASF241:
	.ascii	"__DEC64_MIN_EXP__ (-382)\000"
.LASF320:
	.ascii	"__ULACCUM_IBIT__ 32\000"
.LASF452:
	.ascii	"printf iprintf\000"
.LASF612:
	.ascii	"WCHAR_MAX (__WCHAR_MAX__)\000"
.LASF284:
	.ascii	"__LLFRACT_FBIT__ 63\000"
.LASF109:
	.ascii	"__INT_LEAST8_WIDTH__ 8\000"
.LASF364:
	.ascii	"__USA_FBIT__ 16\000"
.LASF103:
	.ascii	"__UINT8_MAX__ 0xff\000"
.LASF104:
	.ascii	"__UINT16_MAX__ 0xffff\000"
.LASF449:
	.ascii	"DEBUG 1\000"
.LASF582:
	.ascii	"INT_LEAST32_MIN (-__INT_LEAST32_MAX__ - 1)\000"
.LASF587:
	.ascii	"UINT64_MAX (__UINT64_MAX__)\000"
.LASF433:
	.ascii	"__ARM_FEATURE_FMA\000"
.LASF405:
	.ascii	"__ARM_FEATURE_LDREX\000"
.LASF636:
	.ascii	"uint8_t\000"
.LASF504:
	.ascii	"___int_least16_t_defined 1\000"
.LASF95:
	.ascii	"__INTMAX_WIDTH__ 64\000"
.LASF237:
	.ascii	"__DEC32_MAX__ 9.999999E96DF\000"
.LASF274:
	.ascii	"__LFRACT_FBIT__ 31\000"
.LASF572:
	.ascii	"UINT_LEAST8_MAX (__UINT_LEAST8_MAX__)\000"
.LASF51:
	.ascii	"__INT_LEAST16_TYPE__ short int\000"
.LASF195:
	.ascii	"__FLT32_MAX_EXP__ 128\000"
.LASF537:
	.ascii	"__LEAST32 \"l\"\000"
.LASF163:
	.ascii	"__DBL_DIG__ 15\000"
.LASF379:
	.ascii	"__GCC_ATOMIC_CHAR16_T_LOCK_FREE 2\000"
.LASF423:
	.ascii	"__ARMEL__ 1\000"
.LASF421:
	.ascii	"__ARM_ARCH_ISA_THUMB\000"
.LASF314:
	.ascii	"__LACCUM_FBIT__ 31\000"
.LASF353:
	.ascii	"__UTQ_IBIT__ 0\000"
.LASF273:
	.ascii	"__UFRACT_EPSILON__ 0x1P-16UR\000"
.LASF156:
	.ascii	"__FLT_MIN__ 1.1754943508222875e-38F\000"
.LASF384:
	.ascii	"__GCC_ATOMIC_LONG_LOCK_FREE 2\000"
.LASF220:
	.ascii	"__FLT32X_DIG__ 15\000"
.LASF648:
	.ascii	"retval\000"
.LASF574:
	.ascii	"INT16_MAX (__INT16_MAX__)\000"
.LASF350:
	.ascii	"__UDQ_FBIT__ 64\000"
.LASF129:
	.ascii	"__INT_FAST16_MAX__ 0x7fffffff\000"
.LASF563:
	.ascii	"__int_fast64_t_defined 1\000"
.LASF422:
	.ascii	"__ARM_ARCH_ISA_THUMB 2\000"
.LASF618:
	.ascii	"UINT16_C(x) __UINT16_C(x)\000"
.LASF352:
	.ascii	"__UTQ_FBIT__ 128\000"
.LASF250:
	.ascii	"__DEC128_MIN__ 1E-6143DL\000"
.LASF632:
	.ascii	"long long int\000"
.LASF308:
	.ascii	"__ACCUM_EPSILON__ 0x1P-15K\000"
.LASF426:
	.ascii	"__ARM_FP\000"
.LASF559:
	.ascii	"__int_least64_t_defined 1\000"
.LASF140:
	.ascii	"__INTPTR_WIDTH__ 32\000"
.LASF54:
	.ascii	"__UINT_LEAST8_TYPE__ unsigned char\000"
.LASF184:
	.ascii	"__LDBL_MAX__ 1.7976931348623157e+308L\000"
.LASF16:
	.ascii	"__FINITE_MATH_ONLY__ 0\000"
.LASF297:
	.ascii	"__SACCUM_MAX__ 0X7FFFP-7HK\000"
.LASF414:
	.ascii	"__arm__ 1\000"
.LASF176:
	.ascii	"__LDBL_MANT_DIG__ 53\000"
.LASF343:
	.ascii	"__TQ_IBIT__ 0\000"
.LASF362:
	.ascii	"__UHA_FBIT__ 8\000"
.LASF493:
	.ascii	"__SVID_VISIBLE 1\000"
.LASF616:
	.ascii	"UINT8_C(x) __UINT8_C(x)\000"
.LASF595:
	.ascii	"INT_FAST16_MAX (__INT_FAST16_MAX__)\000"
.LASF447:
	.ascii	"__ELF__ 1\000"
.LASF412:
	.ascii	"__ARM_ARCH_PROFILE\000"
.LASF111:
	.ascii	"__INT16_C(c) c\000"
.LASF389:
	.ascii	"__GCC_HAVE_DWARF2_CFI_ASM 1\000"
.LASF106:
	.ascii	"__UINT64_MAX__ 0xffffffffffffffffULL\000"
.LASF317:
	.ascii	"__LACCUM_MAX__ 0X7FFFFFFFFFFFFFFFP-31LK\000"
.LASF131:
	.ascii	"__INT_FAST32_MAX__ 0x7fffffff\000"
.LASF263:
	.ascii	"__USFRACT_EPSILON__ 0x1P-8UHR\000"
.LASF11:
	.ascii	"__ATOMIC_ACQUIRE 2\000"
.LASF557:
	.ascii	"__int_least16_t_defined 1\000"
.LASF536:
	.ascii	"__LEAST16 \"h\"\000"
.LASF553:
	.ascii	"_UINTMAX_T_DECLARED \000"
.LASF445:
	.ascii	"__ARM_FEATURE_COPROC 15\000"
.LASF0:
	.ascii	"__STDC__ 1\000"
.LASF309:
	.ascii	"__UACCUM_FBIT__ 16\000"
.LASF521:
	.ascii	"__int20 +2\000"
.LASF166:
	.ascii	"__DBL_MAX_EXP__ 1024\000"
.LASF478:
	.ascii	"_DEFAULT_SOURCE\000"
.LASF14:
	.ascii	"__ATOMIC_CONSUME 1\000"
.LASF558:
	.ascii	"__int_least32_t_defined 1\000"
.LASF132:
	.ascii	"__INT_FAST32_WIDTH__ 32\000"
.LASF594:
	.ascii	"INT_FAST16_MIN (-__INT_FAST16_MAX__ - 1)\000"
.LASF36:
	.ascii	"__WINT_TYPE__ unsigned int\000"
.LASF74:
	.ascii	"__LONG_MAX__ 0x7fffffffL\000"
.LASF100:
	.ascii	"__INT16_MAX__ 0x7fff\000"
.LASF279:
	.ascii	"__ULFRACT_FBIT__ 32\000"
.LASF82:
	.ascii	"__SCHAR_WIDTH__ 8\000"
.LASF306:
	.ascii	"__ACCUM_MIN__ (-0X1P15K-0X1P15K)\000"
.LASF22:
	.ascii	"__SIZEOF_DOUBLE__ 8\000"
.LASF7:
	.ascii	"__GNUC_PATCHLEVEL__ 1\000"
.LASF446:
	.ascii	"__GXX_TYPEINFO_EQUALITY_INLINE 0\000"
.LASF613:
	.ascii	"WINT_MAX (__WINT_MAX__)\000"
.LASF267:
	.ascii	"__FRACT_MAX__ 0X7FFFP-15R\000"
.LASF79:
	.ascii	"__WINT_MIN__ 0U\000"
.LASF319:
	.ascii	"__ULACCUM_FBIT__ 32\000"
.LASF566:
	.ascii	"UINTPTR_MAX (__UINTPTR_MAX__)\000"
.LASF271:
	.ascii	"__UFRACT_MIN__ 0.0UR\000"
.LASF552:
	.ascii	"_INTMAX_T_DECLARED \000"
.LASF527:
	.ascii	"__INT8 \"hh\"\000"
.LASF436:
	.ascii	"__ARM_NEON_FP\000"
.LASF543:
	.ascii	"_INT16_T_DECLARED \000"
.LASF2:
	.ascii	"__STDC_UTF_16__ 1\000"
.LASF330:
	.ascii	"__ULLACCUM_IBIT__ 32\000"
.LASF620:
	.ascii	"UINT32_C(x) __UINT32_C(x)\000"
.LASF265:
	.ascii	"__FRACT_IBIT__ 0\000"
.LASF185:
	.ascii	"__LDBL_MIN__ 2.2250738585072014e-308L\000"
.LASF360:
	.ascii	"__TA_FBIT__ 63\000"
.LASF189:
	.ascii	"__LDBL_HAS_INFINITY__ 1\000"
.LASF143:
	.ascii	"__GCC_IEC_559_COMPLEX 0\000"
.LASF105:
	.ascii	"__UINT32_MAX__ 0xffffffffUL\000"
.LASF560:
	.ascii	"__int_fast8_t_defined 1\000"
.LASF178:
	.ascii	"__LDBL_MIN_EXP__ (-1021)\000"
.LASF598:
	.ascii	"INT_FAST32_MAX (__INT_FAST32_MAX__)\000"
.LASF289:
	.ascii	"__ULLFRACT_FBIT__ 64\000"
.LASF391:
	.ascii	"__SIZEOF_WCHAR_T__ 4\000"
.LASF645:
	.ascii	"_Bool\000"
.LASF31:
	.ascii	"__FLOAT_WORD_ORDER__ __ORDER_LITTLE_ENDIAN__\000"
.LASF307:
	.ascii	"__ACCUM_MAX__ 0X7FFFFFFFP-15K\000"
.LASF90:
	.ascii	"__SIZE_WIDTH__ 32\000"
.LASF233:
	.ascii	"__DEC32_MANT_DIG__ 7\000"
.LASF535:
	.ascii	"__LEAST8 \"hh\"\000"
.LASF97:
	.ascii	"__SIG_ATOMIC_MIN__ (-__SIG_ATOMIC_MAX__ - 1)\000"
.LASF288:
	.ascii	"__LLFRACT_EPSILON__ 0x1P-63LLR\000"
.LASF85:
	.ascii	"__LONG_WIDTH__ 32\000"
.LASF254:
	.ascii	"__SFRACT_FBIT__ 7\000"
.LASF40:
	.ascii	"__CHAR32_TYPE__ long unsigned int\000"
.LASF255:
	.ascii	"__SFRACT_IBIT__ 0\000"
.LASF439:
	.ascii	"__ARM_PCS 1\000"
.LASF337:
	.ascii	"__HQ_IBIT__ 0\000"
.LASF561:
	.ascii	"__int_fast16_t_defined 1\000"
.LASF485:
	.ascii	"_ATFILE_SOURCE 1\000"
.LASF371:
	.ascii	"__USER_LABEL_PREFIX__ \000"
.LASF158:
	.ascii	"__FLT_DENORM_MIN__ 1.4012984643248171e-45F\000"
.LASF56:
	.ascii	"__UINT_LEAST32_TYPE__ long unsigned int\000"
.LASF340:
	.ascii	"__DQ_FBIT__ 63\000"
.LASF302:
	.ascii	"__USACCUM_MAX__ 0XFFFFP-8UHK\000"
.LASF522:
	.ascii	"__int20__ +2\000"
.LASF406:
	.ascii	"__ARM_FEATURE_LDREX 7\000"
.LASF573:
	.ascii	"INT16_MIN (-__INT16_MAX__ - 1)\000"
.LASF98:
	.ascii	"__SIG_ATOMIC_WIDTH__ 32\000"
.LASF564:
	.ascii	"INTPTR_MIN (-__INTPTR_MAX__ - 1)\000"
.LASF477:
	.ascii	"__GNUC_PREREQ__(ma,mi) __GNUC_PREREQ(ma, mi)\000"
.LASF531:
	.ascii	"__FAST8 \000"
.LASF72:
	.ascii	"__SHRT_MAX__ 0x7fff\000"
.LASF381:
	.ascii	"__GCC_ATOMIC_WCHAR_T_LOCK_FREE 2\000"
.LASF429:
	.ascii	"__ARM_FP16_ARGS\000"
.LASF29:
	.ascii	"__ORDER_PDP_ENDIAN__ 3412\000"
.LASF489:
	.ascii	"__ISO_C_VISIBLE 1999\000"
.LASF339:
	.ascii	"__SQ_IBIT__ 0\000"
.LASF512:
	.ascii	"char\000"
.LASF154:
	.ascii	"__FLT_DECIMAL_DIG__ 9\000"
.LASF542:
	.ascii	"__int8_t_defined 1\000"
.LASF101:
	.ascii	"__INT32_MAX__ 0x7fffffffL\000"
.LASF466:
	.ascii	"CIRCBUFHALF 100\000"
.LASF88:
	.ascii	"__WINT_WIDTH__ 32\000"
.LASF599:
	.ascii	"UINT_FAST32_MAX (__UINT_FAST32_MAX__)\000"
.LASF513:
	.ascii	"short\000"
.LASF344:
	.ascii	"__UQQ_FBIT__ 8\000"
.LASF486:
	.ascii	"__ATFILE_VISIBLE 1\000"
.LASF545:
	.ascii	"__int16_t_defined 1\000"
.LASF628:
	.ascii	"short int\000"
.LASF312:
	.ascii	"__UACCUM_MAX__ 0XFFFFFFFFP-16UK\000"
.LASF242:
	.ascii	"__DEC64_MAX_EXP__ 385\000"
.LASF118:
	.ascii	"__INT_LEAST64_WIDTH__ 64\000"
.LASF44:
	.ascii	"__INT32_TYPE__ long int\000"
.LASF222:
	.ascii	"__FLT32X_MIN_10_EXP__ (-307)\000"
.LASF611:
	.ascii	"WCHAR_MIN (__WCHAR_MIN__)\000"
.LASF630:
	.ascii	"long int\000"
.LASF442:
	.ascii	"__ARM_FEATURE_IDIV 1\000"
.LASF548:
	.ascii	"__int32_t_defined 1\000"
.LASF180:
	.ascii	"__LDBL_MAX_EXP__ 1024\000"
.LASF338:
	.ascii	"__SQ_FBIT__ 31\000"
.LASF496:
	.ascii	"__EXP(x) __ ##x ##__\000"
.LASF299:
	.ascii	"__USACCUM_FBIT__ 8\000"
.LASF249:
	.ascii	"__DEC128_MAX_EXP__ 6145\000"
.LASF77:
	.ascii	"__WCHAR_MIN__ 0U\000"
.LASF212:
	.ascii	"__FLT64_MAX__ 1.7976931348623157e+308F64\000"
.LASF529:
	.ascii	"__INT32 \"l\"\000"
.LASF123:
	.ascii	"__UINT_LEAST32_MAX__ 0xffffffffUL\000"
.LASF418:
	.ascii	"__thumb__ 1\000"
.LASF510:
	.ascii	"signed\000"
.LASF190:
	.ascii	"__LDBL_HAS_QUIET_NAN__ 1\000"
.LASF42:
	.ascii	"__INT8_TYPE__ signed char\000"
.LASF78:
	.ascii	"__WINT_MAX__ 0xffffffffU\000"
.LASF614:
	.ascii	"WINT_MIN (__WINT_MIN__)\000"
.LASF580:
	.ascii	"INT32_MAX (__INT32_MAX__)\000"
.LASF468:
	.ascii	"_STDINT_H \000"
.LASF183:
	.ascii	"__LDBL_DECIMAL_DIG__ 17\000"
.LASF490:
	.ascii	"__LARGEFILE_VISIBLE 0\000"
.LASF601:
	.ascii	"INT_FAST64_MAX (__INT_FAST64_MAX__)\000"
.LASF482:
	.ascii	"_POSIX_C_SOURCE\000"
.LASF546:
	.ascii	"_INT32_T_DECLARED \000"
.LASF342:
	.ascii	"__TQ_FBIT__ 127\000"
.LASF346:
	.ascii	"__UHQ_FBIT__ 16\000"
.LASF138:
	.ascii	"__UINT_FAST64_MAX__ 0xffffffffffffffffULL\000"
.LASF137:
	.ascii	"__UINT_FAST32_MAX__ 0xffffffffU\000"
.LASF528:
	.ascii	"__INT16 \"h\"\000"
.LASF259:
	.ascii	"__USFRACT_FBIT__ 8\000"
.LASF117:
	.ascii	"__INT64_C(c) c ## LL\000"
.LASF336:
	.ascii	"__HQ_FBIT__ 15\000"
.LASF649:
	.ascii	"block\000"
.LASF76:
	.ascii	"__WCHAR_MAX__ 0xffffffffU\000"
.LASF19:
	.ascii	"__SIZEOF_LONG_LONG__ 8\000"
.LASF46:
	.ascii	"__UINT8_TYPE__ unsigned char\000"
.LASF83:
	.ascii	"__SHRT_WIDTH__ 16\000"
.LASF586:
	.ascii	"INT64_MAX (__INT64_MAX__)\000"
.LASF592:
	.ascii	"INT_FAST8_MAX (__INT_FAST8_MAX__)\000"
.LASF640:
	.ascii	"tcirc_buf\000"
.LASF500:
	.ascii	"___int16_t_defined 1\000"
.LASF635:
	.ascii	"__uint8_t\000"
.LASF606:
	.ascii	"SIZE_MAX (__SIZE_MAX__)\000"
.LASF53:
	.ascii	"__INT_LEAST64_TYPE__ long long int\000"
.LASF455:
	.ascii	"PLATFORMCW1190 1\000"
.LASF48:
	.ascii	"__UINT32_TYPE__ long unsigned int\000"
.LASF179:
	.ascii	"__LDBL_MIN_10_EXP__ (-307)\000"
.LASF607:
	.ascii	"SIG_ATOMIC_MIN (-__STDINT_EXP(INT_MAX) - 1)\000"
.LASF32:
	.ascii	"__SIZEOF_POINTER__ 4\000"
.LASF425:
	.ascii	"__VFP_FP__ 1\000"
.LASF282:
	.ascii	"__ULFRACT_MAX__ 0XFFFFFFFFP-32ULR\000"
.LASF354:
	.ascii	"__HA_FBIT__ 7\000"
.LASF89:
	.ascii	"__PTRDIFF_WIDTH__ 32\000"
.LASF201:
	.ascii	"__FLT32_DENORM_MIN__ 1.4012984643248171e-45F32\000"
.LASF583:
	.ascii	"INT_LEAST32_MAX (__INT_LEAST32_MAX__)\000"
.LASF296:
	.ascii	"__SACCUM_MIN__ (-0X1P7HK-0X1P7HK)\000"
.LASF84:
	.ascii	"__INT_WIDTH__ 32\000"
.LASF642:
	.ascii	"circ_buf_has_char\000"
.LASF387:
	.ascii	"__GCC_ATOMIC_POINTER_LOCK_FREE 2\000"
.LASF126:
	.ascii	"__UINT64_C(c) c ## ULL\000"
.LASF8:
	.ascii	"__VERSION__ \"9.3.1 20200408 (release)\"\000"
.LASF318:
	.ascii	"__LACCUM_EPSILON__ 0x1P-31LK\000"
.LASF258:
	.ascii	"__SFRACT_EPSILON__ 0x1P-7HR\000"
.LASF400:
	.ascii	"__ARM_FEATURE_CRC32\000"
.LASF345:
	.ascii	"__UQQ_IBIT__ 0\000"
.LASF226:
	.ascii	"__FLT32X_MAX__ 1.7976931348623157e+308F32x\000"
.LASF348:
	.ascii	"__USQ_FBIT__ 32\000"
.LASF355:
	.ascii	"__HA_IBIT__ 8\000"
.LASF631:
	.ascii	"long unsigned int\000"
.LASF609:
	.ascii	"PTRDIFF_MAX (__PTRDIFF_MAX__)\000"
.LASF380:
	.ascii	"__GCC_ATOMIC_CHAR32_T_LOCK_FREE 2\000"
.LASF1:
	.ascii	"__STDC_VERSION__ 199901L\000"
.LASF438:
	.ascii	"__ARM_ARCH_7M__ 1\000"
.LASF589:
	.ascii	"INT_LEAST64_MAX (__INT_LEAST64_MAX__)\000"
.LASF25:
	.ascii	"__CHAR_BIT__ 8\000"
.LASF234:
	.ascii	"__DEC32_MIN_EXP__ (-94)\000"
.LASF4:
	.ascii	"__STDC_HOSTED__ 1\000"
.LASF555:
	.ascii	"_UINTPTR_T_DECLARED \000"
.LASF593:
	.ascii	"UINT_FAST8_MAX (__UINT_FAST8_MAX__)\000"
.LASF139:
	.ascii	"__INTPTR_MAX__ 0x7fffffff\000"
.LASF68:
	.ascii	"__has_include(STR) __has_include__(STR)\000"
.LASF424:
	.ascii	"__SOFTFP__ 1\000"
.LASF509:
	.ascii	"__STDINT_EXP(x) __ ##x ##__\000"
.LASF214:
	.ascii	"__FLT64_EPSILON__ 2.2204460492503131e-16F64\000"
.LASF451:
	.ascii	"ARM_MATH_CM3 true\000"
.LASF540:
	.ascii	"_INT8_T_DECLARED \000"
.LASF385:
	.ascii	"__GCC_ATOMIC_LLONG_LOCK_FREE 1\000"
.LASF376:
	.ascii	"__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4 1\000"
.LASF152:
	.ascii	"__FLT_MAX_EXP__ 128\000"
.LASF13:
	.ascii	"__ATOMIC_ACQ_REL 4\000"
.LASF65:
	.ascii	"__UINT_FAST64_TYPE__ long long unsigned int\000"
.LASF647:
	.ascii	"newtail\000"
.LASF627:
	.ascii	"unsigned char\000"
.LASF655:
	.ascii	"init_circ_buf\000"
.LASF38:
	.ascii	"__UINTMAX_TYPE__ long long unsigned int\000"
.LASF491:
	.ascii	"__MISC_VISIBLE 1\000"
.LASF186:
	.ascii	"__LDBL_EPSILON__ 2.2204460492503131e-16L\000"
.LASF66:
	.ascii	"__INTPTR_TYPE__ int\000"
.LASF530:
	.ascii	"__INT64 \"ll\"\000"
.LASF146:
	.ascii	"__DEC_EVAL_METHOD__ 2\000"
.LASF301:
	.ascii	"__USACCUM_MIN__ 0.0UHK\000"
.LASF602:
	.ascii	"UINT_FAST64_MAX (__UINT_FAST64_MAX__)\000"
.LASF260:
	.ascii	"__USFRACT_IBIT__ 0\000"
.LASF115:
	.ascii	"__INT_LEAST32_WIDTH__ 32\000"
.LASF394:
	.ascii	"__ARM_FEATURE_DSP\000"
.LASF64:
	.ascii	"__UINT_FAST32_TYPE__ unsigned int\000"
.LASF638:
	.ascii	"tail\000"
.LASF341:
	.ascii	"__DQ_IBIT__ 0\000"
.LASF155:
	.ascii	"__FLT_MAX__ 3.4028234663852886e+38F\000"
.LASF199:
	.ascii	"__FLT32_MIN__ 1.1754943508222875e-38F32\000"
.LASF33:
	.ascii	"__SIZE_TYPE__ unsigned int\000"
.LASF481:
	.ascii	"_POSIX_SOURCE 1\000"
.LASF329:
	.ascii	"__ULLACCUM_FBIT__ 32\000"
.LASF518:
	.ascii	"unsigned +0\000"
.LASF207:
	.ascii	"__FLT64_MIN_EXP__ (-1021)\000"
.LASF507:
	.ascii	"__EXP\000"
.LASF621:
	.ascii	"INT64_C(x) __INT64_C(x)\000"
.LASF264:
	.ascii	"__FRACT_FBIT__ 15\000"
.LASF382:
	.ascii	"__GCC_ATOMIC_SHORT_LOCK_FREE 2\000"
.LASF503:
	.ascii	"___int_least8_t_defined 1\000"
.LASF465:
	.ascii	"CIRCBUFSIZE 200\000"
.LASF369:
	.ascii	"__UTA_IBIT__ 64\000"
.LASF268:
	.ascii	"__FRACT_EPSILON__ 0x1P-15R\000"
.LASF151:
	.ascii	"__FLT_MIN_10_EXP__ (-37)\000"
.LASF221:
	.ascii	"__FLT32X_MIN_EXP__ (-1021)\000"
.LASF228:
	.ascii	"__FLT32X_EPSILON__ 2.2204460492503131e-16F32x\000"
.LASF145:
	.ascii	"__FLT_EVAL_METHOD_TS_18661_3__ 0\000"
.LASF174:
	.ascii	"__DBL_HAS_INFINITY__ 1\000"
.LASF395:
	.ascii	"__ARM_FEATURE_QBIT 1\000"
.LASF60:
	.ascii	"__INT_FAST32_TYPE__ int\000"
.LASF568:
	.ascii	"INT8_MAX (__INT8_MAX__)\000"
.LASF161:
	.ascii	"__FLT_HAS_QUIET_NAN__ 1\000"
.LASF17:
	.ascii	"__SIZEOF_INT__ 4\000"
.LASF37:
	.ascii	"__INTMAX_TYPE__ long long int\000"
.LASF526:
	.ascii	"_INT32_EQ_LONG \000"
.LASF92:
	.ascii	"__INTMAX_C(c) c ## LL\000"
.LASF417:
	.ascii	"__APCS_32__ 1\000"
.LASF585:
	.ascii	"INT64_MIN (-__INT64_MAX__ - 1)\000"
.LASF623:
	.ascii	"INTMAX_C(x) __INTMAX_C(x)\000"
.LASF172:
	.ascii	"__DBL_DENORM_MIN__ ((double)4.9406564584124654e-324"
	.ascii	"L)\000"
.LASF626:
	.ascii	"signed char\000"
.LASF603:
	.ascii	"INTMAX_MAX (__INTMAX_MAX__)\000"
.LASF232:
	.ascii	"__FLT32X_HAS_QUIET_NAN__ 1\000"
.LASF167:
	.ascii	"__DBL_MAX_10_EXP__ 308\000"
.LASF227:
	.ascii	"__FLT32X_MIN__ 2.2250738585072014e-308F32x\000"
.LASF629:
	.ascii	"short unsigned int\000"
.LASF378:
	.ascii	"__GCC_ATOMIC_CHAR_LOCK_FREE 2\000"
.LASF361:
	.ascii	"__TA_IBIT__ 64\000"
.LASF334:
	.ascii	"__QQ_FBIT__ 7\000"
.LASF562:
	.ascii	"__int_fast32_t_defined 1\000"
.LASF231:
	.ascii	"__FLT32X_HAS_INFINITY__ 1\000"
.LASF591:
	.ascii	"INT_FAST8_MIN (-__INT_FAST8_MAX__ - 1)\000"
.LASF554:
	.ascii	"_INTPTR_T_DECLARED \000"
.LASF55:
	.ascii	"__UINT_LEAST16_TYPE__ short unsigned int\000"
.LASF363:
	.ascii	"__UHA_IBIT__ 8\000"
.LASF128:
	.ascii	"__INT_FAST8_WIDTH__ 32\000"
.LASF600:
	.ascii	"INT_FAST64_MIN (-__INT_FAST64_MAX__ - 1)\000"
.LASF610:
	.ascii	"PTRDIFF_MIN (-PTRDIFF_MAX - 1)\000"
.LASF150:
	.ascii	"__FLT_MIN_EXP__ (-125)\000"
.LASF505:
	.ascii	"___int_least32_t_defined 1\000"
.LASF251:
	.ascii	"__DEC128_MAX__ 9.999999999999999999999999999999999E"
	.ascii	"6144DL\000"
.LASF576:
	.ascii	"INT_LEAST16_MIN (-__INT_LEAST16_MAX__ - 1)\000"
.LASF182:
	.ascii	"__DECIMAL_DIG__ 17\000"
.LASF437:
	.ascii	"__THUMB_INTERWORK__ 1\000"
.LASF114:
	.ascii	"__INT32_C(c) c ## L\000"
.LASF549:
	.ascii	"_INT64_T_DECLARED \000"
.LASF462:
	.ascii	"true 1\000"
.LASF428:
	.ascii	"__ARM_FP16_FORMAT_ALTERNATIVE\000"
.LASF181:
	.ascii	"__LDBL_MAX_10_EXP__ 308\000"
.LASF224:
	.ascii	"__FLT32X_MAX_10_EXP__ 308\000"
.LASF141:
	.ascii	"__UINTPTR_MAX__ 0xffffffffU\000"
.LASF173:
	.ascii	"__DBL_HAS_DENORM__ 1\000"
.LASF497:
	.ascii	"__have_longlong64 1\000"
.LASF569:
	.ascii	"UINT8_MAX (__UINT8_MAX__)\000"
.LASF653:
	.ascii	"C:\\Users\\adewa\\Code\\chipwhisperer\\hardware\\vi"
	.ascii	"ctims\\cw305_artixtarget\\fw\\sam3u\\CW305_SAM3U_FW"
	.ascii	"\\src\000"
.LASF196:
	.ascii	"__FLT32_MAX_10_EXP__ 38\000"
.LASF650:
	.ascii	"newhead\000"
.LASF396:
	.ascii	"__ARM_FEATURE_SAT 1\000"
.LASF277:
	.ascii	"__LFRACT_MAX__ 0X7FFFFFFFP-31LR\000"
.LASF366:
	.ascii	"__UDA_FBIT__ 32\000"
.LASF463:
	.ascii	"false 0\000"
.LASF113:
	.ascii	"__INT_LEAST32_MAX__ 0x7fffffffL\000"
.LASF243:
	.ascii	"__DEC64_MIN__ 1E-383DD\000"
.LASF524:
	.ascii	"long +4\000"
.LASF556:
	.ascii	"__int_least8_t_defined 1\000"
.LASF93:
	.ascii	"__UINTMAX_MAX__ 0xffffffffffffffffULL\000"
.LASF41:
	.ascii	"__SIG_ATOMIC_TYPE__ int\000"
.LASF286:
	.ascii	"__LLFRACT_MIN__ (-0.5LLR-0.5LLR)\000"
.LASF293:
	.ascii	"__ULLFRACT_EPSILON__ 0x1P-64ULLR\000"
.LASF188:
	.ascii	"__LDBL_HAS_DENORM__ 1\000"
.LASF121:
	.ascii	"__UINT_LEAST16_MAX__ 0xffff\000"
.LASF333:
	.ascii	"__ULLACCUM_EPSILON__ 0x1P-32ULLK\000"
.LASF240:
	.ascii	"__DEC64_MANT_DIG__ 16\000"
.LASF159:
	.ascii	"__FLT_HAS_DENORM__ 1\000"
.LASF134:
	.ascii	"__INT_FAST64_WIDTH__ 64\000"
.LASF168:
	.ascii	"__DBL_DECIMAL_DIG__ 17\000"
	.ident	"GCC: (GNU Arm Embedded Toolchain 9-2020-q2-update) 9.3.1 20200408 (release)"
