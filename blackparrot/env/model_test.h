#ifndef _COMPLIANCE_MODEL_H
#define _COMPLIANCE_MODEL_H
#define RVMODEL_DATA_SECTION \
        .align 8; .global begin_regstate; begin_regstate:               \
        .word 128;                                                      \
        .align 8; .global end_regstate; end_regstate:                   \
        .word 4;                                                        \
        .align 8; model_scratch:                                        \
        .word 128;

//RV_COMPLIANCE_HALT
#define RVMODEL_HALT                                              \
  /* signature address */                                         \
  li t0, 0x104000;                                                \
  li t1, 0x102000;                                                \
  la t2, begin_signature;                                         \
  la t3, end_signature;                                           \
loop:                                                             \
  lw t5, 0(t2);                                                   \
  sw t5, 0(t0);                                                   \
  addi t2, t2, 0x4;                                               \
  blt t2, t3, loop;                                               \
  sw x0, 0(t1);                                                   \
  fence;

#define RVMODEL_BOOT

//RV_COMPLIANCE_DATA_BEGIN
#define RVMODEL_DATA_BEGIN                                              \
  RVMODEL_DATA_SECTION                                                        \
  .align 4;\
  .global begin_signature; begin_signature: \

//RV_COMPLIANCE_DATA_END
#define RVMODEL_DATA_END                                                      \
  .align 4;\
  .global end_signature; end_signature:  

// Stack construction routine to wind up debug routines
// Clobbers _S
// Saves a0<-[_R], a1<-[_I], t0-t6
#define RVMODEL_STACK_CONSTRUCT(_S, _R, _I) \
  lla _S, model_scratch; \
  sd _R, 0x00(_S);       \
  sd a0, 0x08(_S);       \
  sd a1, 0x10(_S);       \
  sd t0, 0x18(_S);       \
  sd t1, 0x20(_S);       \
  sd t2, 0x28(_S);       \
  sd t3, 0x30(_S);       \
  sd t4, 0x38(_S);       \
  sd t5, 0x40(_S);       \
  sd t6, 0x48(_S);       \
  mv a0, _R;             \
  li a1, _I;             \

// Stack destruction routine to unwind debug routines
// Restores _R, t0-t6
#define RVMODEL_STACK_DESTRUCT(_S, _R, _I) \
  lla _S, model_scratch; \
  ld t6, 0x48(_S);       \
  ld t5, 0x40(_S);       \
  ld t4, 0x38(_S);       \
  ld t3, 0x30(_S);       \
  ld t2, 0x28(_S);       \
  ld t1, 0x20(_S);       \
  ld t0, 0x18(_S);       \
  ld a1, 0x10(_S);       \
  ld a0, 0x08(_S);       \
  ld _R, 0x00(_S);       \

// Stack construction routine to wind up debug routines
// Clobbers _S
// Saves a0<-[_R], a1<-[_I], t0-t6
#define RVMODEL_FP_STACK_CONSTRUCT(_S, _R, _I) \
  lla _S, model_scratch; \
  fsd _R, 0x00(_S);      \
  sd a0, 0x08(_S);       \
  sd a1, 0x10(_S);       \
  sd t0, 0x18(_S);       \
  sd t1, 0x20(_S);       \
  sd t2, 0x28(_S);       \
  sd t3, 0x30(_S);       \
  sd t4, 0x38(_S);       \
  sd t5, 0x40(_S);       \
  sd t6, 0x48(_S);       \
  fmv.x.d a0, _R;        \
  li a1, _I;             \

// Stack destruction routine to unwind debug routines
// Restores _R, t0-t6
#define RVMODEL_FP_STACK_DESTRUCT(_S, _R, _I) \
  lla _S, model_scratch; \
  ld t6, 0x48(_S);       \
  ld t5, 0x40(_S);       \
  ld t4, 0x38(_S);       \
  ld t3, 0x30(_S);       \
  ld t2, 0x28(_S);       \
  ld t1, 0x20(_S);       \
  ld t0, 0x18(_S);       \
  ld a1, 0x10(_S);       \
  ld a0, 0x08(_S);       \
  fld _R, 0x00(_S);      \

// Prints a0=a1 or a0!=a1
// Clobbers t0, t1, t2, t3
#define RVMODEL_PRINT_COMP \
  la t0, 0x101000; \
  la t1, 0x105000; \
  li t2, '!';      \
  li t3, '=';      \
  li t4, '\n';     \
  sd a0, 0(t1);    \
  beq a0, a1, 1f;  \
  sb t2, 0(t0);    \
1:\
  sb t3, 0(t0);    \
  sd a1, 0(t1);    \
  sb t4, 0(t0)

//RVTEST_IO_INIT
#define RVMODEL_IO_INIT
//RVTEST_IO_WRITE_STR
#define RVMODEL_IO_WRITE_STR(_S, _STR) \
RVMODEL_STACK_CONSTRUCT(_S, x0, 0); \
 .pushsection .data.string,"aw",@progbits; \
1: .string _STR; \
 .popsection; \
  la t0, 0x101000; \
  lla t1, 1b; \
2:\
  lb t2, 0(t1);    \
  sb t2, 0(t0);    \
  addi t1, t1, 1;  \
  bnez t2, 2b;     \
RVMODEL_STACK_DESTRUCT(_S, x0, 0);

//RVTEST_IO_CHECK
#define RVMODEL_IO_CHECK()
//RVTEST_IO_ASSERT_GPR_EQ
#define RVMODEL_IO_ASSERT_GPR_EQ(_S, _R, _I) \
RVMODEL_STACK_CONSTRUCT(_S, _R, _I); \
RVMODEL_PRINT_COMP; \
RVMODEL_STACK_DESTRUCT(_S, _R, _I);

//RVTEST_IO_ASSERT_SFPR_EQ
#define RVMODEL_IO_ASSERT_SFPR_EQ(_S, _R, _I) \
RVMODEL_FP_STACK_CONSTRUCT(_S, _R, _I); \
RVMODEL_PRINT_COMP; \
RVMODEL_FP_STACK_DESTRUCT(_S, _R, _I);

//RVTEST_IO_ASSERT_DFPR_EQ
#define RVMODEL_IO_ASSERT_DFPR_EQ(_S, _R, _I) \
RVMODEL_FP_STACK_CONSTRUCT(_S, _R, _I); \
RVMODEL_PRINT_COMP; \
RVMODEL_FP_STACK_DESTRUCT(_S, _R, _I);

#define RVMODEL_SET_MSW_INT       \
 li t1, 1;                        \
 li t2, 0x300000;                 \
 sw t1, 0(t2);

#define RVMODEL_CLEAR_MSW_INT     \
 li t2, 0x300000;                 \
 sw x0, 0(t2);

#define RVMODEL_CLEAR_MTIMER_INT  \
 li t2, 0x304000;                 \
 sw x0, 0(t2);

#define RVMODEL_CLEAR_MEXT_INT    \
 li t2, 0x30b000;                 \
 sw x0, 0(t2);

#endif // _COMPLIANCE_MODEL_H

