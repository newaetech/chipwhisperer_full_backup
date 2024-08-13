#ifndef CLOCK_H
#define CLOCK_H
void MODIFY_HIRC_24(void);
void MODIFY_HIRC_16(void);
void MODIFY_HIRC_166(void);
void use_external_clock(void);
void disable_output_clock(void);
void use_internal_clock(void);
void enable_output_clock(void);
#endif // CLOCK_H