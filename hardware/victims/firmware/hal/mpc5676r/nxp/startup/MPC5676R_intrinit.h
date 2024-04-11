#if !defined(MPC5676R_INTRINIT_H_)
#define MPC5676R_INTRINIT_H_


void SetIVPR (register unsigned int x);
void InitIVORS (void);
void InitINTC(void);
void enableIrq(void);
void xcptn_xmpl(void);

#endif //MPC5676R_HWINIT