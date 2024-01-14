#ifndef _COMPLIANCE_MODEL_H
#define _COMPLIANCE_MODEL_H
#define RVMODEL_DATA_SECTION \
        .pushsection .tohost,"aw",@progbits;                            \
        .align 8; .global tohost; tohost: .dword 0;                     \
        .align 8; .global fromhost; fromhost: .dword 0;                 \
        .popsection;                                                    \
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

//RVTEST_IO_INIT
#define RVMODEL_IO_INIT
//RVTEST_IO_WRITE_STR
#define RVMODEL_IO_WRITE_STR(_R, _STR) \
 .pushsection .data.string,"aw",@progbits; \
1: .string _STR; \
 .popsection; \
  lla _R, model_scratch; \
  sd t0, 0x00(_R); \
  sd t1, 0x08(_R); \
  sd t2, 0x10(_R); \
  la t0, 0x101000; \
  lla t1, 1b; \
2:\
  lb t2, 0(t1); \
  sb t2, 0(t0); \
  addi t1, t1, 1;   \
  bnez t2, 2b;  \
  ld t0, 0x00(_R); \
  ld t1, 0x08(_R); \
  ld t2, 0x10(_R);
//RVTEST_IO_CHECK
#define RVMODEL_IO_CHECK()
//RVTEST_IO_ASSERT_GPR_EQ
#define RVMODEL_IO_ASSERT_GPR_EQ(_S, _R, _I) \
  lla _S, model_scratch; \
  sd _R, 0x00(_S); \
  sd t0, 0x08(_S); \
  sd t1, 0x10(_S); \
  sd t2, 0x18(_S); \
  sd t3, 0x20(_S); \
  li t2, _I;       \
  bne _R, t2, 1f;  \
  ld _R, 0x00(_S); \
  ld t0, 0x08(_S); \
  ld t1, 0x10(_S); \
  ld t2, 0x18(_S); \
  ld t3, 0x20(_S); \
  j 2f;            \
1:                 \
  la t0, 0x102000; \
  li t1, 1;        \
  sd t1, 0x00(t0); \
  fence;           \
2:

//RVTEST_IO_ASSERT_SFPR_EQ
#define RVMODEL_IO_ASSERT_SFPR_EQ(_S, _R, _I) \
  lla _S, model_scratch; \
  fsw _R, 0x00(_S); \
  sd t0, 0x08(_S); \
  sd t1, 0x10(_S); \
  sd t2, 0x18(_S); \
  sd t3, 0x20(_S); \
  li t2, _I;       \
  lw t3, 0x00(_S); \
  bne t3, t2, 1f;  \
  flw _R, 0x00(_S); \
  ld t0, 0x08(_S); \
  ld t1, 0x10(_S); \
  ld t2, 0x18(_S); \
  ld t3, 0x20(_S); \
  j 2f;            \
1:                 \
  la t0, 0x102000; \
  li t1, 1;        \
  sd t1, 0x00(t0); \
  fence;           \
2:

//RVTEST_IO_ASSERT_DFPR_EQ
#define RVMODEL_IO_ASSERT_DFPR_EQ(_S, _R, _I) \
  lla _S, model_scratch; \
  fsd _R, 0x00(_S); \
  sd t0, 0x08(_S); \
  sd t1, 0x10(_S); \
  sd t2, 0x18(_S); \
  sd t3, 0x20(_S); \
  li t2, _I;       \
  ld t3, 0x00(_S); \
  bne t3, t2, 1f;  \
  fld _R, 0x00(_S); \
  ld t0, 0x08(_S); \
  ld t1, 0x10(_S); \
  ld t2, 0x18(_S); \
  ld t3, 0x20(_S); \
  j 2f;            \
1:                 \
  la t0, 0x102000; \
  li t1, 1;        \
  sd t1, 0x00(t0); \
  fence;           \
2:

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

