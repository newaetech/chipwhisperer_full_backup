#ifndef __COMPILER_ABSTRACTION_H__
#define __COMPILER_ABSTRACTION_H__

/****** Macros as an abstraction layer between ARMCC and GCC ********/

/**** C level abstraction **************/
#if defined(__CC_ARM)       /* ARMCC */

#define _INLINE inline
#define _PACKED __attribute__((packed))
#define _ALIGNED(a) __attribute__((aligned(a)))

#elif defined (__GNUC__)    /* GCC */

#define _INLINE inline
#define _PACKED __attribute__((packed))
#define _ALIGNED(a) __attribute__((aligned(a)))

#else
#error "Sorry: unsupported compiler (please use GCC or ARMCC)!"
#endif

/**** Assembly language abstraction ****/
#ifdef ASM
#if defined(__CC_ARM)       /* Keil */
    #define CODESECT AREA     MYCODE, CODE, READONLY, ALIGN=2
    #define RODATASECT AREA   MYDATA, DATA, READONLY
    #define DATASECT AREA     MYDATA, DATA, READWRITE
    #define MYLABEL

    #define MYBYTE DCB
    #define MYLONG DCD
    #define MYWORD DCW

    MACRO
    ldr_addr $reg, $val
    movw $reg, #:lower16:$val
    movt $reg, #:upper16:$val
    MEND

    /* Load a field address from a structure, with offset non zero */
    MACRO
    ldr_struct_field_addr $reg, $stack_offset, $field_offset
    ldr $reg, [sp, #$stack_offset]
    add $reg, #$field_offset
    MEND
 
    #define ldr_struct_field_addr(reg, stack_offset, field_offset)\
    ldr_struct_field_addr reg, stack_offset, field_offset

   /* Set a global variable g with a constant value cst */
   #define DECLARE_GLOB_VAR(g) \
   GBLA g
 
   #define SET_GLOB_VAR(g, cst) \
   g SETA (cst)

   #define DECLARE_UNLOOP_VAR(g) \
   DECLARE_GLOB_VAR(g)

#ifdef USE_FULL_UNROLLING
   /******** Loop handling *******/
   #define SET_REPT_START(counter, start) \
   count SETA    start

   #define SET_REPT_END(counter, end, labelname) \
   WHILE   count <= end

   #define SET_REPT_INC(counter) \
   count   SETA    count+1

   #define SET_REPT_CLOSE(labelname) \
   WEND
#else
  
   MACRO
   set_rept_start $counter, $start
   push {r0, r1}
   ldr r0, [sp, #8]
   mov r1, # $start
   str r1, [r0, # $counter]
   pop {r0, r1}
   MEND

   /******** Loop handling *******/
   #define SET_REPT_START(counter, start)\
   set_rept_start counter, start
 
   MACRO
   set_rept_end $counter, $end, $labelname
$labelname.startlabel
   push {r0, r1}
   ldr r0, [sp, #8]
   ldr r1, [r0, # $counter]
   add r1, r1, #1
   cmp r1, # $end+2
   str r1, [r0, # $counter]
   pop {r0, r1}
   beq $labelname.endlabel
   MEND

   #define SET_REPT_END(counter, end, labelname)\
   set_rept_end counter, end, labelname

   #define SET_REPT_INC(counter)

   MACRO
   set_rept_close $labelname
   b $labelname.startlabel
$labelname.endlabel
   MEND

   #define SET_REPT_CLOSE(labelname)\
   set_rept_close labelname

#endif
   
#elif defined (__GNUC__)    // GCC
    #define THUMB .thumb
    #define CODESECT .section .text
    #define DATASECT .section .data
    #define RODATASECT .section .rodata
    #define EXPORT .global
    #define PROC :
    #define MYLABEL :
    #define ENDP
    #define END
    #define MYBYTE .byte
    #define MYWORD .word
    #define MYLONG .long
	.syntax unified
	.cpu cortex-m4
	.fpu softvfp
	.thumb
    .syntax unified
	#define LOCAL .local
	#define ALIGN(a) .align(a)

    .macro ldr_addr reg, val
    movw \reg, #:lower16:\val
    movt \reg, #:upper16:\val
    .endm

    /* Load a field address from a structure pointed in stack */
    .macro ldr_struct_field_addr reg, stack_offset, field_offset
    ldr \reg, [sp, #\stack_offset]
    add \reg, #\field_offset
    .endm
    #define ldr_struct_field_addr(reg, stack_offset, field_offset)\
    ldr_struct_field_addr reg, stack_offset, field_offset

    /* Load a field address from a structure pointed in register */
    .macro ldr_struct_field_addr_reg reg, reg_ptr, field_offset
    mov \reg, \reg_ptr
    add \reg, #\field_offset
    .endm
    #define ldr_struct_field_addr_reg(reg, reg_ptr, field_offset)\
    ldr_struct_field_addr_reg reg, reg_ptr, field_offset

    /* Set a global variable g with a constant value cst */
    #define DECLARE_GLOB_VAR(g)
    #define SET_GLOB_VAR(g, cst) \
	.set g,  (cst)

 #ifdef USE_FULL_UNROLLING
   /******** Loop handling *******/
   #define SET_REPT_START(counter, start)

   #define SET_REPT_END(counter, end, labelname) \
    .rept end 

   #define SET_REPT_INC(counter)

   #define SET_REPT_CLOSE(labelname) \
   .endr

   #define DECLARE_UNLOOP_VAR(g) \
    DECLARE_GLOB_VAR(g)
#else
   /******** Loop handling *******/
   #define SET_REPT_START(counter, start)\
   push {r0, r1};\
   ldr r0, [sp, #8];\
   mov r1, #0+start;\
   str r1, [r0, #0+counter];\
   pop {r0, r1};\
       
   #define SET_REPT_END(counter, end, labelname)\
   .L##labelname##_startlabel:\
   push {r0, r1};\
   ldr r0, [sp, #8];\
   ldr r1, [r0, #0+counter];\
   add r1, r1, #1;\
   cmp r1, #2+end;\
   str r1, [r0, #0+counter];\
   pop {r0, r1};\
   beq .L##labelname##_endlabel;\

   #define SET_REPT_INC(count)
  
   #define SET_REPT_CLOSE(labelname) \
   b .L##labelname##_startlabel;\
   .L##labelname##_endlabel:\

#endif

#else
#error "Sorry: unsupported compiler (please use GCC or ARMCC)!"
#endif
#endif

#endif /* __COMPILER_ABSTRACTION_H__ */
