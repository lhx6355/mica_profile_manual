//
// Created by shenshan on 16-9-25.
//

#ifndef DECODE_INST_MISCREGINFO_H
#define DECODE_INST_MISCREGINFO_H

#include <string>
#include <bitset>

using namespace std;

enum MiscRegIndex {
	MISCREG_CPSR = 0,               //   0
	MISCREG_SPSR,                   //   1
	MISCREG_SPSR_FIQ,               //   2
	MISCREG_SPSR_IRQ,               //   3
	MISCREG_SPSR_SVC,               //   4
	MISCREG_SPSR_MON,               //   5
	MISCREG_SPSR_ABT,               //   6
	MISCREG_SPSR_HYP,               //   7
	MISCREG_SPSR_UND,               //   8
	MISCREG_ELR_HYP,                //   9
	MISCREG_FPSID,                  //  10
	MISCREG_FPSCR,                  //  11
	MISCREG_MVFR1,                  //  12
	MISCREG_MVFR0,                  //  13
	MISCREG_FPEXC,                  //  14

	// Helper registers
	MISCREG_CPSR_MODE,              //  15
	MISCREG_CPSR_Q,                 //  16
	MISCREG_FPSCR_EXC,              //  17
	MISCREG_FPSCR_QC,               //  18
	MISCREG_LOCKADDR,               //  19
	MISCREG_LOCKFLAG,               //  20
	MISCREG_PRRR_MAIR0,             //  21
	MISCREG_PRRR_MAIR0_NS,          //  22
	MISCREG_PRRR_MAIR0_S,           //  23
	MISCREG_NMRR_MAIR1,             //  24
	MISCREG_NMRR_MAIR1_NS,          //  25
	MISCREG_NMRR_MAIR1_S,           //  26
	MISCREG_PMXEVTYPER_PMCCFILTR,   //  27
	MISCREG_SCTLR_RST,              //  28
	MISCREG_SEV_MAILBOX,            //  29

	// AArch32 CP14 registers (debug/trace/ThumbEE/Jazelle control)
	MISCREG_DBGDIDR,                //  30
	MISCREG_DBGDSCRint,             //  31
	MISCREG_DBGDCCINT,              //  32
	MISCREG_DBGDTRTXint,            //  33
	MISCREG_DBGDTRRXint,            //  34
	MISCREG_DBGWFAR,                //  35
	MISCREG_DBGVCR,                 //  36
	MISCREG_DBGDTRRXext,            //  37
	MISCREG_DBGDSCRext,             //  38
	MISCREG_DBGDTRTXext,            //  39
	MISCREG_DBGOSECCR,              //  40
	MISCREG_DBGBVR0,                //  41
	MISCREG_DBGBVR1,                //  42
	MISCREG_DBGBVR2,                //  43
	MISCREG_DBGBVR3,                //  44
	MISCREG_DBGBVR4,                //  45
	MISCREG_DBGBVR5,                //  46
	MISCREG_DBGBCR0,                //  47
	MISCREG_DBGBCR1,                //  48
	MISCREG_DBGBCR2,                //  49
	MISCREG_DBGBCR3,                //  50
	MISCREG_DBGBCR4,                //  51
	MISCREG_DBGBCR5,                //  52
	MISCREG_DBGWVR0,                //  53
	MISCREG_DBGWVR1,                //  54
	MISCREG_DBGWVR2,                //  55
	MISCREG_DBGWVR3,                //  56
	MISCREG_DBGWCR0,                //  57
	MISCREG_DBGWCR1,                //  58
	MISCREG_DBGWCR2,                //  59
	MISCREG_DBGWCR3,                //  60
	MISCREG_DBGDRAR,                //  61
	MISCREG_DBGBXVR4,               //  62
	MISCREG_DBGBXVR5,               //  63
	MISCREG_DBGOSLAR,               //  64
	MISCREG_DBGOSLSR,               //  65
	MISCREG_DBGOSDLR,               //  66
	MISCREG_DBGPRCR,                //  67
	MISCREG_DBGDSAR,                //  68
	MISCREG_DBGCLAIMSET,            //  69
	MISCREG_DBGCLAIMCLR,            //  70
	MISCREG_DBGAUTHSTATUS,          //  71
	MISCREG_DBGDEVID2,              //  72
	MISCREG_DBGDEVID1,              //  73
	MISCREG_DBGDEVID0,              //  74
	MISCREG_TEECR,                  //  75
	MISCREG_JIDR,                   //  76
	MISCREG_TEEHBR,                 //  77
	MISCREG_JOSCR,                  //  78
	MISCREG_JMCR,                   //  79

	// AArch32 CP15 registers (system control)
	MISCREG_MIDR,                   //  80
	MISCREG_CTR,                    //  81
	MISCREG_TCMTR,                  //  82
	MISCREG_TLBTR,                  //  83
	MISCREG_MPIDR,                  //  84
	MISCREG_REVIDR,                 //  85
	MISCREG_ID_PFR0,                //  86
	MISCREG_ID_PFR1,                //  87
	MISCREG_ID_DFR0,                //  88
	MISCREG_ID_AFR0,                //  89
	MISCREG_ID_MMFR0,               //  90
	MISCREG_ID_MMFR1,               //  91
	MISCREG_ID_MMFR2,               //  92
	MISCREG_ID_MMFR3,               //  93
	MISCREG_ID_ISAR0,               //  94
	MISCREG_ID_ISAR1,               //  95
	MISCREG_ID_ISAR2,               //  96
	MISCREG_ID_ISAR3,               //  97
	MISCREG_ID_ISAR4,               //  98
	MISCREG_ID_ISAR5,               //  99
	MISCREG_CCSIDR,                 // 100
	MISCREG_CLIDR,                  // 101
	MISCREG_AIDR,                   // 102
	MISCREG_CSSELR,                 // 103
	MISCREG_CSSELR_NS,              // 104
	MISCREG_CSSELR_S,               // 105
	MISCREG_VPIDR,                  // 106
	MISCREG_VMPIDR,                 // 107
	MISCREG_SCTLR,                  // 108
	MISCREG_SCTLR_NS,               // 109
	MISCREG_SCTLR_S,                // 110
	MISCREG_ACTLR,                  // 111
	MISCREG_ACTLR_NS,               // 112
	MISCREG_ACTLR_S,                // 113
	MISCREG_CPACR,                  // 114
	MISCREG_SCR,                    // 115
	MISCREG_SDER,                   // 116
	MISCREG_NSACR,                  // 117
	MISCREG_HSCTLR,                 // 118
	MISCREG_HACTLR,                 // 119
	MISCREG_HCR,                    // 120
	MISCREG_HDCR,                   // 121
	MISCREG_HCPTR,                  // 122
	MISCREG_HSTR,                   // 123
	MISCREG_HACR,                   // 124
	MISCREG_TTBR0,                  // 125
	MISCREG_TTBR0_NS,               // 126
	MISCREG_TTBR0_S,                // 127
	MISCREG_TTBR1,                  // 128
	MISCREG_TTBR1_NS,               // 129
	MISCREG_TTBR1_S,                // 130
	MISCREG_TTBCR,                  // 131
	MISCREG_TTBCR_NS,               // 132
	MISCREG_TTBCR_S,                // 133
	MISCREG_HTCR,                   // 134
	MISCREG_VTCR,                   // 135
	MISCREG_DACR,                   // 136
	MISCREG_DACR_NS,                // 137
	MISCREG_DACR_S,                 // 138
	MISCREG_DFSR,                   // 139
	MISCREG_DFSR_NS,                // 140
	MISCREG_DFSR_S,                 // 141
	MISCREG_IFSR,                   // 142
	MISCREG_IFSR_NS,                // 143
	MISCREG_IFSR_S,                 // 144
	MISCREG_ADFSR,                  // 145
	MISCREG_ADFSR_NS,               // 146
	MISCREG_ADFSR_S,                // 147
	MISCREG_AIFSR,                  // 148
	MISCREG_AIFSR_NS,               // 149
	MISCREG_AIFSR_S,                // 150
	MISCREG_HADFSR,                 // 151
	MISCREG_HAIFSR,                 // 152
	MISCREG_HSR,                    // 153
	MISCREG_DFAR,                   // 154
	MISCREG_DFAR_NS,                // 155
	MISCREG_DFAR_S,                 // 156
	MISCREG_IFAR,                   // 157
	MISCREG_IFAR_NS,                // 158
	MISCREG_IFAR_S,                 // 159
	MISCREG_HDFAR,                  // 160
	MISCREG_HIFAR,                  // 161
	MISCREG_HPFAR,                  // 162
	MISCREG_ICIALLUIS,              // 163
	MISCREG_BPIALLIS,               // 164
	MISCREG_PAR,                    // 165
	MISCREG_PAR_NS,                 // 166
	MISCREG_PAR_S,                  // 167
	MISCREG_ICIALLU,                // 168
	MISCREG_ICIMVAU,                // 169
	MISCREG_CP15ISB,                // 170
	MISCREG_BPIALL,                 // 171
	MISCREG_BPIMVA,                 // 172
	MISCREG_DCIMVAC,                // 173
	MISCREG_DCISW,                  // 174
	MISCREG_ATS1CPR,                // 175
	MISCREG_ATS1CPW,                // 176
	MISCREG_ATS1CUR,                // 177
	MISCREG_ATS1CUW,                // 178
	MISCREG_ATS12NSOPR,             // 179
	MISCREG_ATS12NSOPW,             // 180
	MISCREG_ATS12NSOUR,             // 181
	MISCREG_ATS12NSOUW,             // 182
	MISCREG_DCCMVAC,                // 183
	MISCREG_DCCSW,                  // 184
	MISCREG_CP15DSB,                // 185
	MISCREG_CP15DMB,                // 186
	MISCREG_DCCMVAU,                // 187
	MISCREG_DCCIMVAC,               // 188
	MISCREG_DCCISW,                 // 189
	MISCREG_ATS1HR,                 // 190
	MISCREG_ATS1HW,                 // 191
	MISCREG_TLBIALLIS,              // 192
	MISCREG_TLBIMVAIS,              // 193
	MISCREG_TLBIASIDIS,             // 194
	MISCREG_TLBIMVAAIS,             // 195
	MISCREG_TLBIMVALIS,             // 196
	MISCREG_TLBIMVAALIS,            // 197
	MISCREG_ITLBIALL,               // 198
	MISCREG_ITLBIMVA,               // 199
	MISCREG_ITLBIASID,              // 200
	MISCREG_DTLBIALL,               // 201
	MISCREG_DTLBIMVA,               // 202
	MISCREG_DTLBIASID,              // 203
	MISCREG_TLBIALL,                // 204
	MISCREG_TLBIMVA,                // 205
	MISCREG_TLBIASID,               // 206
	MISCREG_TLBIMVAA,               // 207
	MISCREG_TLBIMVAL,               // 208
	MISCREG_TLBIMVAAL,              // 209
	MISCREG_TLBIIPAS2IS,            // 210
	MISCREG_TLBIIPAS2LIS,           // 211
	MISCREG_TLBIALLHIS,             // 212
	MISCREG_TLBIMVAHIS,             // 213
	MISCREG_TLBIALLNSNHIS,          // 214
	MISCREG_TLBIMVALHIS,            // 215
	MISCREG_TLBIIPAS2,              // 216
	MISCREG_TLBIIPAS2L,             // 217
	MISCREG_TLBIALLH,               // 218
	MISCREG_TLBIMVAH,               // 219
	MISCREG_TLBIALLNSNH,            // 220
	MISCREG_TLBIMVALH,              // 221
	MISCREG_PMCR,                   // 222
	MISCREG_PMCNTENSET,             // 223
	MISCREG_PMCNTENCLR,             // 224
	MISCREG_PMOVSR,                 // 225
	MISCREG_PMSWINC,                // 226
	MISCREG_PMSELR,                 // 227
	MISCREG_PMCEID0,                // 228
	MISCREG_PMCEID1,                // 229
	MISCREG_PMCCNTR,                // 230
	MISCREG_PMXEVTYPER,             // 231
	MISCREG_PMCCFILTR,              // 232
	MISCREG_PMXEVCNTR,              // 233
	MISCREG_PMUSERENR,              // 234
	MISCREG_PMINTENSET,             // 235
	MISCREG_PMINTENCLR,             // 236
	MISCREG_PMOVSSET,               // 237
	MISCREG_L2CTLR,                 // 238
	MISCREG_L2ECTLR,                // 239
	MISCREG_PRRR,                   // 240
	MISCREG_PRRR_NS,                // 241
	MISCREG_PRRR_S,                 // 242
	MISCREG_MAIR0,                  // 243
	MISCREG_MAIR0_NS,               // 244
	MISCREG_MAIR0_S,                // 245
	MISCREG_NMRR,                   // 246
	MISCREG_NMRR_NS,                // 247
	MISCREG_NMRR_S,                 // 248
	MISCREG_MAIR1,                  // 249
	MISCREG_MAIR1_NS,               // 250
	MISCREG_MAIR1_S,                // 251
	MISCREG_AMAIR0,                 // 252
	MISCREG_AMAIR0_NS,              // 253
	MISCREG_AMAIR0_S,               // 254
	MISCREG_AMAIR1,                 // 255
	MISCREG_AMAIR1_NS,              // 256
	MISCREG_AMAIR1_S,               // 257
	MISCREG_HMAIR0,                 // 258
	MISCREG_HMAIR1,                 // 259
	MISCREG_HAMAIR0,                // 260
	MISCREG_HAMAIR1,                // 261
	MISCREG_VBAR,                   // 262
	MISCREG_VBAR_NS,                // 263
	MISCREG_VBAR_S,                 // 264
	MISCREG_MVBAR,                  // 265
	MISCREG_RMR,                    // 266
	MISCREG_ISR,                    // 267
	MISCREG_HVBAR,                  // 268
	MISCREG_FCSEIDR,                // 269
	MISCREG_CONTEXTIDR,             // 270
	MISCREG_CONTEXTIDR_NS,          // 271
	MISCREG_CONTEXTIDR_S,           // 272
	MISCREG_TPIDRURW,               // 273
	MISCREG_TPIDRURW_NS,            // 274
	MISCREG_TPIDRURW_S,             // 275
	MISCREG_TPIDRURO,               // 276
	MISCREG_TPIDRURO_NS,            // 277
	MISCREG_TPIDRURO_S,             // 278
	MISCREG_TPIDRPRW,               // 279
	MISCREG_TPIDRPRW_NS,            // 280
	MISCREG_TPIDRPRW_S,             // 281
	MISCREG_HTPIDR,                 // 282
	MISCREG_CNTFRQ,                 // 283
	MISCREG_CNTKCTL,                // 284
	MISCREG_CNTP_TVAL,              // 285
	MISCREG_CNTP_TVAL_NS,           // 286
	MISCREG_CNTP_TVAL_S,            // 287
	MISCREG_CNTP_CTL,               // 288
	MISCREG_CNTP_CTL_NS,            // 289
	MISCREG_CNTP_CTL_S,             // 290
	MISCREG_CNTV_TVAL,              // 291
	MISCREG_CNTV_CTL,               // 292
	MISCREG_CNTHCTL,                // 293
	MISCREG_CNTHP_TVAL,             // 294
	MISCREG_CNTHP_CTL,              // 295
	MISCREG_IL1DATA0,               // 296
	MISCREG_IL1DATA1,               // 297
	MISCREG_IL1DATA2,               // 298
	MISCREG_IL1DATA3,               // 299
	MISCREG_DL1DATA0,               // 300
	MISCREG_DL1DATA1,               // 301
	MISCREG_DL1DATA2,               // 302
	MISCREG_DL1DATA3,               // 303
	MISCREG_DL1DATA4,               // 304
	MISCREG_RAMINDEX,               // 305
	MISCREG_L2ACTLR,                // 306
	MISCREG_CBAR,                   // 307
	MISCREG_HTTBR,                  // 308
	MISCREG_VTTBR,                  // 309
	MISCREG_CNTPCT,                 // 310
	MISCREG_CNTVCT,                 // 311
	MISCREG_CNTP_CVAL,              // 312
	MISCREG_CNTP_CVAL_NS,           // 313
	MISCREG_CNTP_CVAL_S,            // 314
	MISCREG_CNTV_CVAL,              // 315
	MISCREG_CNTVOFF,                // 316
	MISCREG_CNTHP_CVAL,             // 317
	MISCREG_CPUMERRSR,              // 318
	MISCREG_L2MERRSR,               // 319

	// AArch64 registers (Op0=2)
	MISCREG_MDCCINT_EL1,            // 320
	MISCREG_OSDTRRX_EL1,            // 321
	MISCREG_MDSCR_EL1,              // 322
	MISCREG_OSDTRTX_EL1,            // 323
	MISCREG_OSECCR_EL1,             // 324
	MISCREG_DBGBVR0_EL1,            // 325
	MISCREG_DBGBVR1_EL1,            // 326
	MISCREG_DBGBVR2_EL1,            // 327
	MISCREG_DBGBVR3_EL1,            // 328
	MISCREG_DBGBVR4_EL1,            // 329
	MISCREG_DBGBVR5_EL1,            // 330
	MISCREG_DBGBCR0_EL1,            // 331
	MISCREG_DBGBCR1_EL1,            // 332
	MISCREG_DBGBCR2_EL1,            // 333
	MISCREG_DBGBCR3_EL1,            // 334
	MISCREG_DBGBCR4_EL1,            // 335
	MISCREG_DBGBCR5_EL1,            // 336
	MISCREG_DBGWVR0_EL1,            // 337
	MISCREG_DBGWVR1_EL1,            // 338
	MISCREG_DBGWVR2_EL1,            // 339
	MISCREG_DBGWVR3_EL1,            // 340
	MISCREG_DBGWCR0_EL1,            // 341
	MISCREG_DBGWCR1_EL1,            // 342
	MISCREG_DBGWCR2_EL1,            // 343
	MISCREG_DBGWCR3_EL1,            // 344
	MISCREG_MDCCSR_EL0,             // 345
	MISCREG_MDDTR_EL0,              // 346
	MISCREG_MDDTRTX_EL0,            // 347
	MISCREG_MDDTRRX_EL0,            // 348
	MISCREG_DBGVCR32_EL2,           // 349
	MISCREG_MDRAR_EL1,              // 350
	MISCREG_OSLAR_EL1,              // 351
	MISCREG_OSLSR_EL1,              // 352
	MISCREG_OSDLR_EL1,              // 353
	MISCREG_DBGPRCR_EL1,            // 354
	MISCREG_DBGCLAIMSET_EL1,        // 355
	MISCREG_DBGCLAIMCLR_EL1,        // 356
	MISCREG_DBGAUTHSTATUS_EL1,      // 357
	MISCREG_TEECR32_EL1,            // 358
	MISCREG_TEEHBR32_EL1,           // 359

	// AArch64 registers (Op0=1,3)
	MISCREG_MIDR_EL1,               // 360
	MISCREG_MPIDR_EL1,              // 361
	MISCREG_REVIDR_EL1,             // 362
	MISCREG_ID_PFR0_EL1,            // 363
	MISCREG_ID_PFR1_EL1,            // 364
	MISCREG_ID_DFR0_EL1,            // 365
	MISCREG_ID_AFR0_EL1,            // 366
	MISCREG_ID_MMFR0_EL1,           // 367
	MISCREG_ID_MMFR1_EL1,           // 368
	MISCREG_ID_MMFR2_EL1,           // 369
	MISCREG_ID_MMFR3_EL1,           // 370
	MISCREG_ID_ISAR0_EL1,           // 371
	MISCREG_ID_ISAR1_EL1,           // 372
	MISCREG_ID_ISAR2_EL1,           // 373
	MISCREG_ID_ISAR3_EL1,           // 374
	MISCREG_ID_ISAR4_EL1,           // 375
	MISCREG_ID_ISAR5_EL1,           // 376
	MISCREG_MVFR0_EL1,              // 377
	MISCREG_MVFR1_EL1,              // 378
	MISCREG_MVFR2_EL1,              // 379
	MISCREG_ID_AA64PFR0_EL1,        // 380
	MISCREG_ID_AA64PFR1_EL1,        // 381
	MISCREG_ID_AA64DFR0_EL1,        // 382
	MISCREG_ID_AA64DFR1_EL1,        // 383
	MISCREG_ID_AA64AFR0_EL1,        // 384
	MISCREG_ID_AA64AFR1_EL1,        // 385
	MISCREG_ID_AA64ISAR0_EL1,       // 386
	MISCREG_ID_AA64ISAR1_EL1,       // 387
	MISCREG_ID_AA64MMFR0_EL1,       // 388
	MISCREG_ID_AA64MMFR1_EL1,       // 389
	MISCREG_CCSIDR_EL1,             // 390
	MISCREG_CLIDR_EL1,              // 391
	MISCREG_AIDR_EL1,               // 392
	MISCREG_CSSELR_EL1,             // 393
	MISCREG_CTR_EL0,                // 394
	MISCREG_DCZID_EL0,              // 395
	MISCREG_VPIDR_EL2,              // 396
	MISCREG_VMPIDR_EL2,             // 397
	MISCREG_SCTLR_EL1,              // 398
	MISCREG_ACTLR_EL1,              // 399
	MISCREG_CPACR_EL1,              // 400
	MISCREG_SCTLR_EL2,              // 401
	MISCREG_ACTLR_EL2,              // 402
	MISCREG_HCR_EL2,                // 403
	MISCREG_MDCR_EL2,               // 404
	MISCREG_CPTR_EL2,               // 405
	MISCREG_HSTR_EL2,               // 406
	MISCREG_HACR_EL2,               // 407
	MISCREG_SCTLR_EL3,              // 408
	MISCREG_ACTLR_EL3,              // 409
	MISCREG_SCR_EL3,                // 410
	MISCREG_SDER32_EL3,             // 411
	MISCREG_CPTR_EL3,               // 412
	MISCREG_MDCR_EL3,               // 413
	MISCREG_TTBR0_EL1,              // 414
	MISCREG_TTBR1_EL1,              // 415
	MISCREG_TCR_EL1,                // 416
	MISCREG_TTBR0_EL2,              // 417
	MISCREG_TCR_EL2,                // 418
	MISCREG_VTTBR_EL2,              // 419
	MISCREG_VTCR_EL2,               // 420
	MISCREG_TTBR0_EL3,              // 421
	MISCREG_TCR_EL3,                // 422
	MISCREG_DACR32_EL2,             // 423
	MISCREG_SPSR_EL1,               // 424
	MISCREG_ELR_EL1,                // 425
	MISCREG_SP_EL0,                 // 426
	MISCREG_SPSEL,                  // 427
	MISCREG_CURRENTEL,              // 428
	MISCREG_NZCV,                   // 429
	MISCREG_DAIF,                   // 430
	MISCREG_FPCR,                   // 431
	MISCREG_FPSR,                   // 432
	MISCREG_DSPSR_EL0,              // 433
	MISCREG_DLR_EL0,                // 434
	MISCREG_SPSR_EL2,               // 435
	MISCREG_ELR_EL2,                // 436
	MISCREG_SP_EL1,                 // 437
	MISCREG_SPSR_IRQ_AA64,          // 438
	MISCREG_SPSR_ABT_AA64,          // 439
	MISCREG_SPSR_UND_AA64,          // 440
	MISCREG_SPSR_FIQ_AA64,          // 441
	MISCREG_SPSR_EL3,               // 442
	MISCREG_ELR_EL3,                // 443
	MISCREG_SP_EL2,                 // 444
	MISCREG_AFSR0_EL1,              // 445
	MISCREG_AFSR1_EL1,              // 446
	MISCREG_ESR_EL1,                // 447
	MISCREG_IFSR32_EL2,             // 448
	MISCREG_AFSR0_EL2,              // 449
	MISCREG_AFSR1_EL2,              // 450
	MISCREG_ESR_EL2,                // 451
	MISCREG_FPEXC32_EL2,            // 452
	MISCREG_AFSR0_EL3,              // 453
	MISCREG_AFSR1_EL3,              // 454
	MISCREG_ESR_EL3,                // 455
	MISCREG_FAR_EL1,                // 456
	MISCREG_FAR_EL2,                // 457
	MISCREG_HPFAR_EL2,              // 458
	MISCREG_FAR_EL3,                // 459
	MISCREG_IC_IALLUIS,             // 460
	MISCREG_PAR_EL1,                // 461
	MISCREG_IC_IALLU,               // 462
	MISCREG_DC_IVAC_Xt,             // 463
	MISCREG_DC_ISW_Xt,              // 464
	MISCREG_AT_S1E1R_Xt,            // 465
	MISCREG_AT_S1E1W_Xt,            // 466
	MISCREG_AT_S1E0R_Xt,            // 467
	MISCREG_AT_S1E0W_Xt,            // 468
	MISCREG_DC_CSW_Xt,              // 469
	MISCREG_DC_CISW_Xt,             // 470
	MISCREG_DC_ZVA_Xt,              // 471
	MISCREG_IC_IVAU_Xt,             // 472
	MISCREG_DC_CVAC_Xt,             // 473
	MISCREG_DC_CVAU_Xt,             // 474
	MISCREG_DC_CIVAC_Xt,            // 475
	MISCREG_AT_S1E2R_Xt,            // 476
	MISCREG_AT_S1E2W_Xt,            // 477
	MISCREG_AT_S12E1R_Xt,           // 478
	MISCREG_AT_S12E1W_Xt,           // 479
	MISCREG_AT_S12E0R_Xt,           // 480
	MISCREG_AT_S12E0W_Xt,           // 481
	MISCREG_AT_S1E3R_Xt,            // 482
	MISCREG_AT_S1E3W_Xt,            // 483
	MISCREG_TLBI_VMALLE1IS,         // 484
	MISCREG_TLBI_VAE1IS_Xt,         // 485
	MISCREG_TLBI_ASIDE1IS_Xt,       // 486
	MISCREG_TLBI_VAAE1IS_Xt,        // 487
	MISCREG_TLBI_VALE1IS_Xt,        // 488
	MISCREG_TLBI_VAALE1IS_Xt,       // 489
	MISCREG_TLBI_VMALLE1,           // 490
	MISCREG_TLBI_VAE1_Xt,           // 491
	MISCREG_TLBI_ASIDE1_Xt,         // 492
	MISCREG_TLBI_VAAE1_Xt,          // 493
	MISCREG_TLBI_VALE1_Xt,          // 494
	MISCREG_TLBI_VAALE1_Xt,         // 495
	MISCREG_TLBI_IPAS2E1IS_Xt,      // 496
	MISCREG_TLBI_IPAS2LE1IS_Xt,     // 497
	MISCREG_TLBI_ALLE2IS,           // 498
	MISCREG_TLBI_VAE2IS_Xt,         // 499
	MISCREG_TLBI_ALLE1IS,           // 500
	MISCREG_TLBI_VALE2IS_Xt,        // 501
	MISCREG_TLBI_VMALLS12E1IS,      // 502
	MISCREG_TLBI_IPAS2E1_Xt,        // 503
	MISCREG_TLBI_IPAS2LE1_Xt,       // 504
	MISCREG_TLBI_ALLE2,             // 505
	MISCREG_TLBI_VAE2_Xt,           // 506
	MISCREG_TLBI_ALLE1,             // 507
	MISCREG_TLBI_VALE2_Xt,          // 508
	MISCREG_TLBI_VMALLS12E1,        // 509
	MISCREG_TLBI_ALLE3IS,           // 510
	MISCREG_TLBI_VAE3IS_Xt,         // 511
	MISCREG_TLBI_VALE3IS_Xt,        // 512
	MISCREG_TLBI_ALLE3,             // 513
	MISCREG_TLBI_VAE3_Xt,           // 514
	MISCREG_TLBI_VALE3_Xt,          // 515
	MISCREG_PMINTENSET_EL1,         // 516
	MISCREG_PMINTENCLR_EL1,         // 517
	MISCREG_PMCR_EL0,               // 518
	MISCREG_PMCNTENSET_EL0,         // 519
	MISCREG_PMCNTENCLR_EL0,         // 520
	MISCREG_PMOVSCLR_EL0,           // 521
	MISCREG_PMSWINC_EL0,            // 522
	MISCREG_PMSELR_EL0,             // 523
	MISCREG_PMCEID0_EL0,            // 524
	MISCREG_PMCEID1_EL0,            // 525
	MISCREG_PMCCNTR_EL0,            // 526
	MISCREG_PMXEVTYPER_EL0,         // 527
	MISCREG_PMCCFILTR_EL0,          // 528
	MISCREG_PMXEVCNTR_EL0,          // 529
	MISCREG_PMUSERENR_EL0,          // 530
	MISCREG_PMOVSSET_EL0,           // 531
	MISCREG_MAIR_EL1,               // 532
	MISCREG_AMAIR_EL1,              // 533
	MISCREG_MAIR_EL2,               // 534
	MISCREG_AMAIR_EL2,              // 535
	MISCREG_MAIR_EL3,               // 536
	MISCREG_AMAIR_EL3,              // 537
	MISCREG_L2CTLR_EL1,             // 538
	MISCREG_L2ECTLR_EL1,            // 539
	MISCREG_VBAR_EL1,               // 540
	MISCREG_RVBAR_EL1,              // 541
	MISCREG_ISR_EL1,                // 542
	MISCREG_VBAR_EL2,               // 543
	MISCREG_RVBAR_EL2,              // 544
	MISCREG_VBAR_EL3,               // 545
	MISCREG_RVBAR_EL3,              // 546
	MISCREG_RMR_EL3,                // 547
	MISCREG_CONTEXTIDR_EL1,         // 548
	MISCREG_TPIDR_EL1,              // 549
	MISCREG_TPIDR_EL0,              // 550
	MISCREG_TPIDRRO_EL0,            // 551
	MISCREG_TPIDR_EL2,              // 552
	MISCREG_TPIDR_EL3,              // 553
	MISCREG_CNTKCTL_EL1,            // 554
	MISCREG_CNTFRQ_EL0,             // 555
	MISCREG_CNTPCT_EL0,             // 556
	MISCREG_CNTVCT_EL0,             // 557
	MISCREG_CNTP_TVAL_EL0,          // 558
	MISCREG_CNTP_CTL_EL0,           // 559
	MISCREG_CNTP_CVAL_EL0,          // 560
	MISCREG_CNTV_TVAL_EL0,          // 561
	MISCREG_CNTV_CTL_EL0,           // 562
	MISCREG_CNTV_CVAL_EL0,          // 563
	MISCREG_PMEVCNTR0_EL0,          // 564
	MISCREG_PMEVCNTR1_EL0,          // 565
	MISCREG_PMEVCNTR2_EL0,          // 566
	MISCREG_PMEVCNTR3_EL0,          // 567
	MISCREG_PMEVCNTR4_EL0,          // 568
	MISCREG_PMEVCNTR5_EL0,          // 569
	MISCREG_PMEVTYPER0_EL0,         // 570
	MISCREG_PMEVTYPER1_EL0,         // 571
	MISCREG_PMEVTYPER2_EL0,         // 572
	MISCREG_PMEVTYPER3_EL0,         // 573
	MISCREG_PMEVTYPER4_EL0,         // 574
	MISCREG_PMEVTYPER5_EL0,         // 575
	MISCREG_CNTVOFF_EL2,            // 576
	MISCREG_CNTHCTL_EL2,            // 577
	MISCREG_CNTHP_TVAL_EL2,         // 578
	MISCREG_CNTHP_CTL_EL2,          // 579
	MISCREG_CNTHP_CVAL_EL2,         // 580
	MISCREG_CNTPS_TVAL_EL1,         // 581
	MISCREG_CNTPS_CTL_EL1,          // 582
	MISCREG_CNTPS_CVAL_EL1,         // 583
	MISCREG_IL1DATA0_EL1,           // 584
	MISCREG_IL1DATA1_EL1,           // 585
	MISCREG_IL1DATA2_EL1,           // 586
	MISCREG_IL1DATA3_EL1,           // 587
	MISCREG_DL1DATA0_EL1,           // 588
	MISCREG_DL1DATA1_EL1,           // 589
	MISCREG_DL1DATA2_EL1,           // 590
	MISCREG_DL1DATA3_EL1,           // 591
	MISCREG_DL1DATA4_EL1,           // 592
	MISCREG_L2ACTLR_EL1,            // 593
	MISCREG_CPUACTLR_EL1,           // 594
	MISCREG_CPUECTLR_EL1,           // 595
	MISCREG_CPUMERRSR_EL1,          // 596
	MISCREG_L2MERRSR_EL1,           // 597
	MISCREG_CBAR_EL1,               // 598
	MISCREG_CONTEXTIDR_EL2,         // 599

	// Dummy registers
	MISCREG_NOP,                    // 600
	MISCREG_RAZ,                    // 601
	MISCREG_CP14_UNIMPL,            // 602
	MISCREG_CP15_UNIMPL,            // 603
	MISCREG_A64_UNIMPL,             // 604
	MISCREG_UNKNOWN,                // 605

	NUM_MISCREGS                    // 606
};

enum MiscRegInfo {
    MISCREG_IMPLEMENTED,
    MISCREG_UNVERIFIABLE,   // Does the value change on every read (e.g. a
    // arch generic counter)
            MISCREG_WARN_NOT_FAIL,  // If MISCREG_IMPLEMENTED is deasserted, it
    // tells whether the instruction should raise a
    // warning or fail
            MISCREG_MUTEX,  // True if the register corresponds to a pair of
    // mutually exclusive registers
            MISCREG_BANKED,  // True if the register is banked between the two
    // security states, and this is the parent node of the
    // two banked registers
            MISCREG_BANKED_CHILD, // The entry is one of the child registers that
    // forms a banked set of regs (along with the
    // other child regs)

    // Access permissions
    // User mode
            MISCREG_USR_NS_RD,
    MISCREG_USR_NS_WR,
    MISCREG_USR_S_RD,
    MISCREG_USR_S_WR,
    // Privileged modes other than hypervisor or monitor
            MISCREG_PRI_NS_RD,
    MISCREG_PRI_NS_WR,
    MISCREG_PRI_S_RD,
    MISCREG_PRI_S_WR,
    // Hypervisor mode
            MISCREG_HYP_RD,
    MISCREG_HYP_WR,
    // Monitor mode, SCR.NS == 0
            MISCREG_MON_NS0_RD,
    MISCREG_MON_NS0_WR,
    // Monitor mode, SCR.NS == 1
            MISCREG_MON_NS1_RD,
    MISCREG_MON_NS1_WR,

    NUM_MISCREG_INFOS
};


inline MiscRegIndex decodeCP14Reg(unsigned crn, unsigned opc1, unsigned crm, unsigned opc2)
{
	switch (crn) {
	case 0:
		switch (opc1) {
		case 0:
			switch (opc2) {
			case 0:
				switch (crm) {
				case 0:
					return MISCREG_DBGDIDR;
				case 1:
					return MISCREG_DBGDSCRint;
				}
				break;
			}
			break;
		case 7:
			switch (opc2) {
			case 0:
				switch (crm) {
				case 0:
					return MISCREG_JIDR;
				}
				break;
			}
			break;
		}
		break;
	case 1:
		switch (opc1) {
		case 6:
			switch (crm) {
			case 0:
				switch (opc2) {
				case 0:
					return MISCREG_TEEHBR;
				}
				break;
			}
			break;
		case 7:
			switch (crm) {
			case 0:
				switch (opc2) {
				case 0:
					return MISCREG_JOSCR;
				}
				break;
			}
			break;
		}
		break;
	case 2:
		switch (opc1) {
		case 7:
			switch (crm) {
			case 0:
				switch (opc2) {
				case 0:
					return MISCREG_JMCR;
				}
				break;
			}
			break;
		}
		break;
	}
	return MISCREG_CP14_UNIMPL;
}

inline MiscRegIndex decodeCP15Reg(unsigned crn, unsigned opc1, unsigned crm, unsigned opc2)
{
	switch (crn) {
	case 0:
		switch (opc1) {
		case 0:
			switch (crm) {
			case 0:
				switch (opc2) {
				case 1:
					return MISCREG_CTR;
				case 2:
					return MISCREG_TCMTR;
				case 3:
					return MISCREG_TLBTR;
				case 5:
					return MISCREG_MPIDR;
				case 6:
					return MISCREG_REVIDR;
				default:
					return MISCREG_MIDR;
				}
				break;
			case 1:
				switch (opc2) {
				case 0:
					return MISCREG_ID_PFR0;
				case 1:
					return MISCREG_ID_PFR1;
				case 2:
					return MISCREG_ID_DFR0;
				case 3:
					return MISCREG_ID_AFR0;
				case 4:
					return MISCREG_ID_MMFR0;
				case 5:
					return MISCREG_ID_MMFR1;
				case 6:
					return MISCREG_ID_MMFR2;
				case 7:
					return MISCREG_ID_MMFR3;
				}
				break;
			case 2:
				switch (opc2) {
				case 0:
					return MISCREG_ID_ISAR0;
				case 1:
					return MISCREG_ID_ISAR1;
				case 2:
					return MISCREG_ID_ISAR2;
				case 3:
					return MISCREG_ID_ISAR3;
				case 4:
					return MISCREG_ID_ISAR4;
				case 5:
					return MISCREG_ID_ISAR5;
				case 6:
				case 7:
					return MISCREG_RAZ; // read as zero
				}
				break;
			default:
				return MISCREG_RAZ; // read as zero
			}
			break;
		case 1:
			if (crm == 0) {
				switch (opc2) {
				case 0:
					return MISCREG_CCSIDR;
				case 1:
					return MISCREG_CLIDR;
				case 7:
					return MISCREG_AIDR;
				}
			}
			break;
		case 2:
			if (crm == 0 && opc2 == 0) {
				return MISCREG_CSSELR;
			}
			break;
		case 4:
			if (crm == 0) {
				if (opc2 == 0)
					return MISCREG_VPIDR;
				else if (opc2 == 5)
					return MISCREG_VMPIDR;
			}
			break;
		}
		break;
	case 1:
		if (opc1 == 0) {
			if (crm == 0) {
				switch (opc2) {
				case 0:
					return MISCREG_SCTLR;
				case 1:
					return MISCREG_ACTLR;
				case 0x2:
					return MISCREG_CPACR;
				}
			}
			else if (crm == 1) {
				switch (opc2) {
				case 0:
					return MISCREG_SCR;
				case 1:
					return MISCREG_SDER;
				case 2:
					return MISCREG_NSACR;
				}
			}
		}
		else if (opc1 == 4) {
			if (crm == 0) {
				if (opc2 == 0)
					return MISCREG_HSCTLR;
				else if (opc2 == 1)
					return MISCREG_HACTLR;
			}
			else if (crm == 1) {
				switch (opc2) {
				case 0:
					return MISCREG_HCR;
				case 1:
					return MISCREG_HDCR;
				case 2:
					return MISCREG_HCPTR;
				case 3:
					return MISCREG_HSTR;
				case 7:
					return MISCREG_HACR;
				}
			}
		}
		break;
	case 2:
		if (opc1 == 0 && crm == 0) {
			switch (opc2) {
			case 0:
				return MISCREG_TTBR0;
			case 1:
				return MISCREG_TTBR1;
			case 2:
				return MISCREG_TTBCR;
			}
		}
		else if (opc1 == 4) {
			if (crm == 0 && opc2 == 2)
				return MISCREG_HTCR;
			else if (crm == 1 && opc2 == 2)
				return MISCREG_VTCR;
		}
		break;
	case 3:
		if (opc1 == 0 && crm == 0 && opc2 == 0) {
			return MISCREG_DACR;
		}
		break;
	case 5:
		if (opc1 == 0) {
			if (crm == 0) {
				if (opc2 == 0) {
					return MISCREG_DFSR;
				}
				else if (opc2 == 1) {
					return MISCREG_IFSR;
				}
			}
			else if (crm == 1) {
				if (opc2 == 0) {
					return MISCREG_ADFSR;
				}
				else if (opc2 == 1) {
					return MISCREG_AIFSR;
				}
			}
		}
		else if (opc1 == 4) {
			if (crm == 1) {
				if (opc2 == 0)
					return MISCREG_HADFSR;
				else if (opc2 == 1)
					return MISCREG_HAIFSR;
			}
			else if (crm == 2 && opc2 == 0) {
				return MISCREG_HSR;
			}
		}
		break;
	case 6:
		if (opc1 == 0 && crm == 0) {
			switch (opc2) {
			case 0:
				return MISCREG_DFAR;
			case 2:
				return MISCREG_IFAR;
			}
		}
		else if (opc1 == 4 && crm == 0) {
			switch (opc2) {
			case 0:
				return MISCREG_HDFAR;
			case 2:
				return MISCREG_HIFAR;
			case 4:
				return MISCREG_HPFAR;
			}
		}
		break;
	case 7:
		if (opc1 == 0) {
			switch (crm) {
			case 0:
				if (opc2 == 4) {
					return MISCREG_NOP;
				}
				break;
			case 1:
				switch (opc2) {
				case 0:
					return MISCREG_ICIALLUIS;
				case 6:
					return MISCREG_BPIALLIS;
				}
				break;
			case 4:
				if (opc2 == 0) {
					return MISCREG_PAR;
				}
				break;
			case 5:
				switch (opc2) {
				case 0:
					return MISCREG_ICIALLU;
				case 1:
					return MISCREG_ICIMVAU;
				case 4:
					return MISCREG_CP15ISB;
				case 6:
					return MISCREG_BPIALL;
				case 7:
					return MISCREG_BPIMVA;
				}
				break;
			case 6:
				if (opc2 == 1) {
					return MISCREG_DCIMVAC;
				}
				else if (opc2 == 2) {
					return MISCREG_DCISW;
				}
				break;
			case 8:
				switch (opc2) {
				case 0:
					return MISCREG_ATS1CPR;
				case 1:
					return MISCREG_ATS1CPW;
				case 2:
					return MISCREG_ATS1CUR;
				case 3:
					return MISCREG_ATS1CUW;
				case 4:
					return MISCREG_ATS12NSOPR;
				case 5:
					return MISCREG_ATS12NSOPW;
				case 6:
					return MISCREG_ATS12NSOUR;
				case 7:
					return MISCREG_ATS12NSOUW;
				}
				break;
			case 10:
				switch (opc2) {
				case 1:
					return MISCREG_DCCMVAC;
				case 2:
					return MISCREG_DCCSW;
				case 4:
					return MISCREG_CP15DSB;
				case 5:
					return MISCREG_CP15DMB;
				}
				break;
			case 11:
				if (opc2 == 1) {
					return MISCREG_DCCMVAU;
				}
				break;
			case 13:
				if (opc2 == 1) {
					return MISCREG_NOP;
				}
				break;
			case 14:
				if (opc2 == 1) {
					return MISCREG_DCCIMVAC;
				}
				else if (opc2 == 2) {
					return MISCREG_DCCISW;
				}
				break;
			}
		}
		else if (opc1 == 4 && crm == 8) {
			if (opc2 == 0)
				return MISCREG_ATS1HR;
			else if (opc2 == 1)
				return MISCREG_ATS1HW;
		}
		break;
	case 8:
		if (opc1 == 0) {
			switch (crm) {
			case 3:
				switch (opc2) {
				case 0:
					return MISCREG_TLBIALLIS;
				case 1:
					return MISCREG_TLBIMVAIS;
				case 2:
					return MISCREG_TLBIASIDIS;
				case 3:
					return MISCREG_TLBIMVAAIS;
				}
				break;
			case 5:
				switch (opc2) {
				case 0:
					return MISCREG_ITLBIALL;
				case 1:
					return MISCREG_ITLBIMVA;
				case 2:
					return MISCREG_ITLBIASID;
				}
				break;
			case 6:
				switch (opc2) {
				case 0:
					return MISCREG_DTLBIALL;
				case 1:
					return MISCREG_DTLBIMVA;
				case 2:
					return MISCREG_DTLBIASID;
				}
				break;
			case 7:
				switch (opc2) {
				case 0:
					return MISCREG_TLBIALL;
				case 1:
					return MISCREG_TLBIMVA;
				case 2:
					return MISCREG_TLBIASID;
				case 3:
					return MISCREG_TLBIMVAA;
				}
				break;
			}
		}
		else if (opc1 == 4) {
			if (crm == 3) {
				switch (opc2) {
				case 0:
					return MISCREG_TLBIALLHIS;
				case 1:
					return MISCREG_TLBIMVAHIS;
				case 4:
					return MISCREG_TLBIALLNSNHIS;
				}
			}
			else if (crm == 7) {
				switch (opc2) {
				case 0:
					return MISCREG_TLBIALLH;
				case 1:
					return MISCREG_TLBIMVAH;
				case 4:
					return MISCREG_TLBIALLNSNH;
				}
			}
		}
		break;
	case 9:
		if (opc1 == 0) {
			switch (crm) {
			case 12:
				switch (opc2) {
				case 0:
					return MISCREG_PMCR;
				case 1:
					return MISCREG_PMCNTENSET;
				case 2:
					return MISCREG_PMCNTENCLR;
				case 3:
					return MISCREG_PMOVSR;
				case 4:
					return MISCREG_PMSWINC;
				case 5:
					return MISCREG_PMSELR;
				case 6:
					return MISCREG_PMCEID0;
				case 7:
					return MISCREG_PMCEID1;
				}
				break;
			case 13:
				switch (opc2) {
				case 0:
					return MISCREG_PMCCNTR;
				case 1:
					// Selector is PMSELR.SEL
					return MISCREG_PMXEVTYPER_PMCCFILTR;
				case 2:
					return MISCREG_PMXEVCNTR;
				}
				break;
			case 14:
				switch (opc2) {
				case 0:
					return MISCREG_PMUSERENR;
				case 1:
					return MISCREG_PMINTENSET;
				case 2:
					return MISCREG_PMINTENCLR;
				case 3:
					return MISCREG_PMOVSSET;
				}
				break;
			}
		}
		else if (opc1 == 1) {
			switch (crm) {
			case 0:
				switch (opc2) {
				case 2: // L2CTLR, L2 Control Register
					return MISCREG_L2CTLR;
				case 3:
					return MISCREG_L2ECTLR;
				}
				break;
				break;
			}
		}
		break;
	case 10:
		if (opc1 == 0) {
			// crm 0, 1, 4, and 8, with op2 0 - 7, reserved for TLB lockdown
			if (crm == 2) { // TEX Remap Registers
				if (opc2 == 0) {
					// Selector is TTBCR.EAE
					return MISCREG_PRRR_MAIR0;
				}
				else if (opc2 == 1) {
					// Selector is TTBCR.EAE
					return MISCREG_NMRR_MAIR1;
				}
			}
			else if (crm == 3) {
				if (opc2 == 0) {
					return MISCREG_AMAIR0;
				}
				else if (opc2 == 1) {
					return MISCREG_AMAIR1;
				}
			}
		}
		else if (opc1 == 4) {
			// crm 0, 1, 4, and 8, with op2 0 - 7, reserved for TLB lockdown
			if (crm == 2) {
				if (opc2 == 0)
					return MISCREG_HMAIR0;
				else if (opc2 == 1)
					return MISCREG_HMAIR1;
			}
			else if (crm == 3) {
				if (opc2 == 0)
					return MISCREG_HAMAIR0;
				else if (opc2 == 1)
					return MISCREG_HAMAIR1;
			}
		}
		break;
	case 11:
		if (opc1 <= 7) {
			switch (crm) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 15:
				// Reserved for DMA operations for TCM access
				break;
			}
		}
		break;
	case 12:
		if (opc1 == 0) {
			if (crm == 0) {
				if (opc2 == 0) {
					return MISCREG_VBAR;
				}
				else if (opc2 == 1) {
					return MISCREG_MVBAR;
				}
			}
			else if (crm == 1) {
				if (opc2 == 0) {
					return MISCREG_ISR;
				}
			}
		}
		else if (opc1 == 4) {
			if (crm == 0 && opc2 == 0)
				return MISCREG_HVBAR;
		}
		break;
	case 13:
		if (opc1 == 0) {
			if (crm == 0) {
				switch (opc2) {
				case 0:
					return MISCREG_FCSEIDR;
				case 1:
					return MISCREG_CONTEXTIDR;
				case 2:
					return MISCREG_TPIDRURW;
				case 3:
					return MISCREG_TPIDRURO;
				case 4:
					return MISCREG_TPIDRPRW;
				}
			}
		}
		else if (opc1 == 4) {
			if (crm == 0 && opc2 == 2)
				return MISCREG_HTPIDR;
		}
		break;
	case 14:
		if (opc1 == 0) {
			switch (crm) {
			case 0:
				if (opc2 == 0)
					return MISCREG_CNTFRQ;
				break;
			case 1:
				if (opc2 == 0)
					return MISCREG_CNTKCTL;
				break;
			case 2:
				if (opc2 == 0)
					return MISCREG_CNTP_TVAL;
				else if (opc2 == 1)
					return MISCREG_CNTP_CTL;
				break;
			case 3:
				if (opc2 == 0)
					return MISCREG_CNTV_TVAL;
				else if (opc2 == 1)
					return MISCREG_CNTV_CTL;
				break;
			}
		}
		else if (opc1 == 4) {
			if (crm == 1 && opc2 == 0) {
				return MISCREG_CNTHCTL;
			}
			else if (crm == 2) {
				if (opc2 == 0)
					return MISCREG_CNTHP_TVAL;
				else if (opc2 == 1)
					return MISCREG_CNTHP_CTL;
			}
		}
		break;
	case 15:
		// Implementation defined
		return MISCREG_CP15_UNIMPL;
	}
	// Unrecognized register
	return MISCREG_CP15_UNIMPL;
}

inline MiscRegIndex decodeCP15Reg64(unsigned crm, unsigned opc1)
{
	switch (crm) {
	case 2:
		switch (opc1) {
		case 0:
			return MISCREG_TTBR0;
		case 1:
			return MISCREG_TTBR1;
		case 4:
			return MISCREG_HTTBR;
		case 6:
			return MISCREG_VTTBR;
		}
		break;
	case 7:
		if (opc1 == 0)
			return MISCREG_PAR;
		break;
	case 14:
		switch (opc1) {
		case 0:
			return MISCREG_CNTPCT;
		case 1:
			return MISCREG_CNTVCT;
		case 2:
			return MISCREG_CNTP_CVAL;
		case 3:
			return MISCREG_CNTV_CVAL;
		case 4:
			return MISCREG_CNTVOFF;
		case 6:
			return MISCREG_CNTHP_CVAL;
		}
		break;
	case 15:
		if (opc1 == 0)
			return MISCREG_CPUMERRSR;
		else if (opc1 == 1)
			return MISCREG_L2MERRSR;
		break;
	}
	// Unrecognized register
	return MISCREG_CP15_UNIMPL;
}

inline MiscRegIndex decodeAArch64SysReg(unsigned op0, unsigned op1, unsigned crn, unsigned crm, unsigned op2) {
	switch (op0) {
	case 1:
		switch (crn) {
		case 7:
			switch (op1) {
			case 0:
				switch (crm) {
				case 1:
					switch (op2) {
					case 0:
						return MISCREG_IC_IALLUIS;
					}
					break;
				case 5:
					switch (op2) {
					case 0:
						return MISCREG_IC_IALLU;
					}
					break;
				case 6:
					switch (op2) {
					case 1:
						return MISCREG_DC_IVAC_Xt;
					case 2:
						return MISCREG_DC_ISW_Xt;
					}
					break;
				case 8:
					switch (op2) {
					case 0:
						return MISCREG_AT_S1E1R_Xt;
					case 1:
						return MISCREG_AT_S1E1W_Xt;
					case 2:
						return MISCREG_AT_S1E0R_Xt;
					case 3:
						return MISCREG_AT_S1E0W_Xt;
					}
					break;
				case 10:
					switch (op2) {
					case 2:
						return MISCREG_DC_CSW_Xt;
					}
					break;
				case 14:
					switch (op2) {
					case 2:
						return MISCREG_DC_CISW_Xt;
					}
					break;
				}
				break;
			case 3:
				switch (crm) {
				case 4:
					switch (op2) {
					case 1:
						return MISCREG_DC_ZVA_Xt;
					}
					break;
				case 5:
					switch (op2) {
					case 1:
						return MISCREG_IC_IVAU_Xt;
					}
					break;
				case 10:
					switch (op2) {
					case 1:
						return MISCREG_DC_CVAC_Xt;
					}
					break;
				case 11:
					switch (op2) {
					case 1:
						return MISCREG_DC_CVAU_Xt;
					}
					break;
				case 14:
					switch (op2) {
					case 1:
						return MISCREG_DC_CIVAC_Xt;
					}
					break;
				}
				break;
			case 4:
				switch (crm) {
				case 8:
					switch (op2) {
					case 0:
						return MISCREG_AT_S1E2R_Xt;
					case 1:
						return MISCREG_AT_S1E2W_Xt;
					case 4:
						return MISCREG_AT_S12E1R_Xt;
					case 5:
						return MISCREG_AT_S12E1W_Xt;
					case 6:
						return MISCREG_AT_S12E0R_Xt;
					case 7:
						return MISCREG_AT_S12E0W_Xt;
					}
					break;
				}
				break;
			case 6:
				switch (crm) {
				case 8:
					switch (op2) {
					case 0:
						return MISCREG_AT_S1E3R_Xt;
					case 1:
						return MISCREG_AT_S1E3W_Xt;
					}
					break;
				}
				break;
			}
			break;
		case 8:
			switch (op1) {
			case 0:
				switch (crm) {
				case 3:
					switch (op2) {
					case 0:
						return MISCREG_TLBI_VMALLE1IS;
					case 1:
						return MISCREG_TLBI_VAE1IS_Xt;
					case 2:
						return MISCREG_TLBI_ASIDE1IS_Xt;
					case 3:
						return MISCREG_TLBI_VAAE1IS_Xt;
					case 5:
						return MISCREG_TLBI_VALE1IS_Xt;
					case 7:
						return MISCREG_TLBI_VAALE1IS_Xt;
					}
					break;
				case 7:
					switch (op2) {
					case 0:
						return MISCREG_TLBI_VMALLE1;
					case 1:
						return MISCREG_TLBI_VAE1_Xt;
					case 2:
						return MISCREG_TLBI_ASIDE1_Xt;
					case 3:
						return MISCREG_TLBI_VAAE1_Xt;
					case 5:
						return MISCREG_TLBI_VALE1_Xt;
					case 7:
						return MISCREG_TLBI_VAALE1_Xt;
					}
					break;
				}
				break;
			case 4:
				switch (crm) {
				case 0:
					switch (op2) {
					case 1:
						return MISCREG_TLBI_IPAS2E1IS_Xt;
					case 5:
						return MISCREG_TLBI_IPAS2LE1IS_Xt;
					}
					break;
				case 3:
					switch (op2) {
					case 0:
						return MISCREG_TLBI_ALLE2IS;
					case 1:
						return MISCREG_TLBI_VAE2IS_Xt;
					case 4:
						return MISCREG_TLBI_ALLE1IS;
					case 5:
						return MISCREG_TLBI_VALE2IS_Xt;
					case 6:
						return MISCREG_TLBI_VMALLS12E1IS;
					}
					break;
				case 4:
					switch (op2) {
					case 1:
						return MISCREG_TLBI_IPAS2E1_Xt;
					case 5:
						return MISCREG_TLBI_IPAS2LE1_Xt;
					}
					break;
				case 7:
					switch (op2) {
					case 0:
						return MISCREG_TLBI_ALLE2;
					case 1:
						return MISCREG_TLBI_VAE2_Xt;
					case 4:
						return MISCREG_TLBI_ALLE1;
					case 5:
						return MISCREG_TLBI_VALE2_Xt;
					case 6:
						return MISCREG_TLBI_VMALLS12E1;
					}
					break;
				}
				break;
			case 6:
				switch (crm) {
				case 3:
					switch (op2) {
					case 0:
						return MISCREG_TLBI_ALLE3IS;
					case 1:
						return MISCREG_TLBI_VAE3IS_Xt;
					case 5:
						return MISCREG_TLBI_VALE3IS_Xt;
					}
					break;
				case 7:
					switch (op2) {
					case 0:
						return MISCREG_TLBI_ALLE3;
					case 1:
						return MISCREG_TLBI_VAE3_Xt;
					case 5:
						return MISCREG_TLBI_VALE3_Xt;
					}
					break;
				}
				break;
			}
			break;
		}
		break;
	case 2:
		switch (crn) {
		case 0:
			switch (op1) {
			case 0:
				switch (crm) {
				case 0:
					switch (op2) {
					case 2:
						return MISCREG_OSDTRRX_EL1;
					case 4:
						return MISCREG_DBGBVR0_EL1;
					case 5:
						return MISCREG_DBGBCR0_EL1;
					case 6:
						return MISCREG_DBGWVR0_EL1;
					case 7:
						return MISCREG_DBGWCR0_EL1;
					}
					break;
				case 1:
					switch (op2) {
					case 4:
						return MISCREG_DBGBVR1_EL1;
					case 5:
						return MISCREG_DBGBCR1_EL1;
					case 6:
						return MISCREG_DBGWVR1_EL1;
					case 7:
						return MISCREG_DBGWCR1_EL1;
					}
					break;
				case 2:
					switch (op2) {
					case 0:
						return MISCREG_MDCCINT_EL1;
					case 2:
						return MISCREG_MDSCR_EL1;
					case 4:
						return MISCREG_DBGBVR2_EL1;
					case 5:
						return MISCREG_DBGBCR2_EL1;
					case 6:
						return MISCREG_DBGWVR2_EL1;
					case 7:
						return MISCREG_DBGWCR2_EL1;
					}
					break;
				case 3:
					switch (op2) {
					case 2:
						return MISCREG_OSDTRTX_EL1;
					case 4:
						return MISCREG_DBGBVR3_EL1;
					case 5:
						return MISCREG_DBGBCR3_EL1;
					case 6:
						return MISCREG_DBGWVR3_EL1;
					case 7:
						return MISCREG_DBGWCR3_EL1;
					}
					break;
				case 4:
					switch (op2) {
					case 4:
						return MISCREG_DBGBVR4_EL1;
					case 5:
						return MISCREG_DBGBCR4_EL1;
					}
					break;
				case 5:
					switch (op2) {
					case 4:
						return MISCREG_DBGBVR5_EL1;
					case 5:
						return MISCREG_DBGBCR5_EL1;
					}
					break;
				case 6:
					switch (op2) {
					case 2:
						return MISCREG_OSECCR_EL1;
					}
					break;
				}
				break;
			case 2:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_TEECR32_EL1;
					}
					break;
				}
				break;
			case 3:
				switch (crm) {
				case 1:
					switch (op2) {
					case 0:
						return MISCREG_MDCCSR_EL0;
					}
					break;
				case 4:
					switch (op2) {
					case 0:
						return MISCREG_MDDTR_EL0;
					}
					break;
				case 5:
					switch (op2) {
					case 0:
						return MISCREG_MDDTRRX_EL0;
					}
					break;
				}
				break;
			case 4:
				switch (crm) {
				case 7:
					switch (op2) {
					case 0:
						return MISCREG_DBGVCR32_EL2;
					}
					break;
				}
				break;
			}
			break;
		case 1:
			switch (op1) {
			case 0:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_MDRAR_EL1;
					case 4:
						return MISCREG_OSLAR_EL1;
					}
					break;
				case 1:
					switch (op2) {
					case 4:
						return MISCREG_OSLSR_EL1;
					}
					break;
				case 3:
					switch (op2) {
					case 4:
						return MISCREG_OSDLR_EL1;
					}
					break;
				case 4:
					switch (op2) {
					case 4:
						return MISCREG_DBGPRCR_EL1;
					}
					break;
				}
				break;
			case 2:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_TEEHBR32_EL1;
					}
					break;
				}
				break;
			}
			break;
		case 7:
			switch (op1) {
			case 0:
				switch (crm) {
				case 8:
					switch (op2) {
					case 6:
						return MISCREG_DBGCLAIMSET_EL1;
					}
					break;
				case 9:
					switch (op2) {
					case 6:
						return MISCREG_DBGCLAIMCLR_EL1;
					}
					break;
				case 14:
					switch (op2) {
					case 6:
						return MISCREG_DBGAUTHSTATUS_EL1;
					}
					break;
				}
				break;
			}
			break;
		}
		break;
	case 3:
		switch (crn) {
		case 0:
			switch (op1) {
			case 0:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_MIDR_EL1;
					case 5:
						return MISCREG_MPIDR_EL1;
					case 6:
						return MISCREG_REVIDR_EL1;
					}
					break;
				case 1:
					switch (op2) {
					case 0:
						return MISCREG_ID_PFR0_EL1;
					case 1:
						return MISCREG_ID_PFR1_EL1;
					case 2:
						return MISCREG_ID_DFR0_EL1;
					case 3:
						return MISCREG_ID_AFR0_EL1;
					case 4:
						return MISCREG_ID_MMFR0_EL1;
					case 5:
						return MISCREG_ID_MMFR1_EL1;
					case 6:
						return MISCREG_ID_MMFR2_EL1;
					case 7:
						return MISCREG_ID_MMFR3_EL1;
					}
					break;
				case 2:
					switch (op2) {
					case 0:
						return MISCREG_ID_ISAR0_EL1;
					case 1:
						return MISCREG_ID_ISAR1_EL1;
					case 2:
						return MISCREG_ID_ISAR2_EL1;
					case 3:
						return MISCREG_ID_ISAR3_EL1;
					case 4:
						return MISCREG_ID_ISAR4_EL1;
					case 5:
						return MISCREG_ID_ISAR5_EL1;
					}
					break;
				case 3:
					switch (op2) {
					case 0:
						return MISCREG_MVFR0_EL1;
					case 1:
						return MISCREG_MVFR1_EL1;
					case 2:
						return MISCREG_MVFR2_EL1;
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
						return MISCREG_RAZ;
					}
					break;
				case 4:
					switch (op2) {
					case 0:
						return MISCREG_ID_AA64PFR0_EL1;
					case 1:
						return MISCREG_ID_AA64PFR1_EL1;
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
						return MISCREG_RAZ;
					}
					break;
				case 5:
					switch (op2) {
					case 0:
						return MISCREG_ID_AA64DFR0_EL1;
					case 1:
						return MISCREG_ID_AA64DFR1_EL1;
					case 4:
						return MISCREG_ID_AA64AFR0_EL1;
					case 5:
						return MISCREG_ID_AA64AFR1_EL1;
					case 2:
					case 3:
					case 6:
					case 7:
						return MISCREG_RAZ;
					}
					break;
				case 6:
					switch (op2) {
					case 0:
						return MISCREG_ID_AA64ISAR0_EL1;
					case 1:
						return MISCREG_ID_AA64ISAR1_EL1;
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
						return MISCREG_RAZ;
					}
					break;
				case 7:
					switch (op2) {
					case 0:
						return MISCREG_ID_AA64MMFR0_EL1;
					case 1:
						return MISCREG_ID_AA64MMFR1_EL1;
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
						return MISCREG_RAZ;
					}
					break;
				}
				break;
			case 1:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_CCSIDR_EL1;
					case 1:
						return MISCREG_CLIDR_EL1;
					case 7:
						return MISCREG_AIDR_EL1;
					}
					break;
				}
				break;
			case 2:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_CSSELR_EL1;
					}
					break;
				}
				break;
			case 3:
				switch (crm) {
				case 0:
					switch (op2) {
					case 1:
						return MISCREG_CTR_EL0;
					case 7:
						return MISCREG_DCZID_EL0;
					}
					break;
				}
				break;
			case 4:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_VPIDR_EL2;
					case 5:
						return MISCREG_VMPIDR_EL2;
					}
					break;
				}
				break;
			}
			break;
		case 1:
			switch (op1) {
			case 0:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_SCTLR_EL1;
					case 1:
						return MISCREG_ACTLR_EL1;
					case 2:
						return MISCREG_CPACR_EL1;
					}
					break;
				}
				break;
			case 4:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_SCTLR_EL2;
					case 1:
						return MISCREG_ACTLR_EL2;
					}
					break;
				case 1:
					switch (op2) {
					case 0:
						return MISCREG_HCR_EL2;
					case 1:
						return MISCREG_MDCR_EL2;
					case 2:
						return MISCREG_CPTR_EL2;
					case 3:
						return MISCREG_HSTR_EL2;
					case 7:
						return MISCREG_HACR_EL2;
					}
					break;
				}
				break;
			case 6:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_SCTLR_EL3;
					case 1:
						return MISCREG_ACTLR_EL3;
					}
					break;
				case 1:
					switch (op2) {
					case 0:
						return MISCREG_SCR_EL3;
					case 1:
						return MISCREG_SDER32_EL3;
					case 2:
						return MISCREG_CPTR_EL3;
					}
					break;
				case 3:
					switch (op2) {
					case 1:
						return MISCREG_MDCR_EL3;
					}
					break;
				}
				break;
			}
			break;
		case 2:
			switch (op1) {
			case 0:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_TTBR0_EL1;
					case 1:
						return MISCREG_TTBR1_EL1;
					case 2:
						return MISCREG_TCR_EL1;
					}
					break;
				}
				break;
			case 4:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_TTBR0_EL2;
					case 2:
						return MISCREG_TCR_EL2;
					}
					break;
				case 1:
					switch (op2) {
					case 0:
						return MISCREG_VTTBR_EL2;
					case 2:
						return MISCREG_VTCR_EL2;
					}
					break;
				}
				break;
			case 6:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_TTBR0_EL3;
					case 2:
						return MISCREG_TCR_EL3;
					}
					break;
				}
				break;
			}
			break;
		case 3:
			switch (op1) {
			case 4:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_DACR32_EL2;
					}
					break;
				}
				break;
			}
			break;
		case 4:
			switch (op1) {
			case 0:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_SPSR_EL1;
					case 1:
						return MISCREG_ELR_EL1;
					}
					break;
				case 1:
					switch (op2) {
					case 0:
						return MISCREG_SP_EL0;
					}
					break;
				case 2:
					switch (op2) {
					case 0:
						return MISCREG_SPSEL;
					case 2:
						return MISCREG_CURRENTEL;
					}
					break;
				}
				break;
			case 3:
				switch (crm) {
				case 2:
					switch (op2) {
					case 0:
						return MISCREG_NZCV;
					case 1:
						return MISCREG_DAIF;
					}
					break;
				case 4:
					switch (op2) {
					case 0:
						return MISCREG_FPCR;
					case 1:
						return MISCREG_FPSR;
					}
					break;
				case 5:
					switch (op2) {
					case 0:
						return MISCREG_DSPSR_EL0;
					case 1:
						return MISCREG_DLR_EL0;
					}
					break;
				}
				break;
			case 4:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_SPSR_EL2;
					case 1:
						return MISCREG_ELR_EL2;
					}
					break;
				case 1:
					switch (op2) {
					case 0:
						return MISCREG_SP_EL1;
					}
					break;
				case 3:
					switch (op2) {
					case 0:
						return MISCREG_SPSR_IRQ_AA64;
					case 1:
						return MISCREG_SPSR_ABT_AA64;
					case 2:
						return MISCREG_SPSR_UND_AA64;
					case 3:
						return MISCREG_SPSR_FIQ_AA64;
					}
					break;
				}
				break;
			case 6:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_SPSR_EL3;
					case 1:
						return MISCREG_ELR_EL3;
					}
					break;
				case 1:
					switch (op2) {
					case 0:
						return MISCREG_SP_EL2;
					}
					break;
				}
				break;
			}
			break;
		case 5:
			switch (op1) {
			case 0:
				switch (crm) {
				case 1:
					switch (op2) {
					case 0:
						return MISCREG_AFSR0_EL1;
					case 1:
						return MISCREG_AFSR1_EL1;
					}
					break;
				case 2:
					switch (op2) {
					case 0:
						return MISCREG_ESR_EL1;
					}
					break;
				}
				break;
			case 4:
				switch (crm) {
				case 0:
					switch (op2) {
					case 1:
						return MISCREG_IFSR32_EL2;
					}
					break;
				case 1:
					switch (op2) {
					case 0:
						return MISCREG_AFSR0_EL2;
					case 1:
						return MISCREG_AFSR1_EL2;
					}
					break;
				case 2:
					switch (op2) {
					case 0:
						return MISCREG_ESR_EL2;
					}
					break;
				case 3:
					switch (op2) {
					case 0:
						return MISCREG_FPEXC32_EL2;
					}
					break;
				}
				break;
			case 6:
				switch (crm) {
				case 1:
					switch (op2) {
					case 0:
						return MISCREG_AFSR0_EL3;
					case 1:
						return MISCREG_AFSR1_EL3;
					}
					break;
				case 2:
					switch (op2) {
					case 0:
						return MISCREG_ESR_EL3;
					}
					break;
				}
				break;
			}
			break;
		case 6:
			switch (op1) {
			case 0:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_FAR_EL1;
					}
					break;
				}
				break;
			case 4:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_FAR_EL2;
					case 4:
						return MISCREG_HPFAR_EL2;
					}
					break;
				}
				break;
			case 6:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_FAR_EL3;
					}
					break;
				}
				break;
			}
			break;
		case 7:
			switch (op1) {
			case 0:
				switch (crm) {
				case 4:
					switch (op2) {
					case 0:
						return MISCREG_PAR_EL1;
					}
					break;
				}
				break;
			}
			break;
		case 9:
			switch (op1) {
			case 0:
				switch (crm) {
				case 14:
					switch (op2) {
					case 1:
						return MISCREG_PMINTENSET_EL1;
					case 2:
						return MISCREG_PMINTENCLR_EL1;
					}
					break;
				}
				break;
			case 3:
				switch (crm) {
				case 12:
					switch (op2) {
					case 0:
						return MISCREG_PMCR_EL0;
					case 1:
						return MISCREG_PMCNTENSET_EL0;
					case 2:
						return MISCREG_PMCNTENCLR_EL0;
					case 3:
						return MISCREG_PMOVSCLR_EL0;
					case 4:
						return MISCREG_PMSWINC_EL0;
					case 5:
						return MISCREG_PMSELR_EL0;
					case 6:
						return MISCREG_PMCEID0_EL0;
					case 7:
						return MISCREG_PMCEID1_EL0;
					}
					break;
				case 13:
					switch (op2) {
					case 0:
						return MISCREG_PMCCNTR_EL0;
					case 1:
						return MISCREG_PMXEVTYPER_EL0;
					case 2:
						return MISCREG_PMXEVCNTR_EL0;
					}
					break;
				case 14:
					switch (op2) {
					case 0:
						return MISCREG_PMUSERENR_EL0;
					case 3:
						return MISCREG_PMOVSSET_EL0;
					}
					break;
				}
				break;
			}
			break;
		case 10:
			switch (op1) {
			case 0:
				switch (crm) {
				case 2:
					switch (op2) {
					case 0:
						return MISCREG_MAIR_EL1;
					}
					break;
				case 3:
					switch (op2) {
					case 0:
						return MISCREG_AMAIR_EL1;
					}
					break;
				}
				break;
			case 4:
				switch (crm) {
				case 2:
					switch (op2) {
					case 0:
						return MISCREG_MAIR_EL2;
					}
					break;
				case 3:
					switch (op2) {
					case 0:
						return MISCREG_AMAIR_EL2;
					}
					break;
				}
				break;
			case 6:
				switch (crm) {
				case 2:
					switch (op2) {
					case 0:
						return MISCREG_MAIR_EL3;
					}
					break;
				case 3:
					switch (op2) {
					case 0:
						return MISCREG_AMAIR_EL3;
					}
					break;
				}
				break;
			}
			break;
		case 11:
			switch (op1) {
			case 1:
				switch (crm) {
				case 0:
					switch (op2) {
					case 2:
						return MISCREG_L2CTLR_EL1;
					case 3:
						return MISCREG_L2ECTLR_EL1;
					}
					break;
				}
				break;
			}
			break;
		case 12:
			switch (op1) {
			case 0:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_VBAR_EL1;
					case 1:
						return MISCREG_RVBAR_EL1;
					}
					break;
				case 1:
					switch (op2) {
					case 0:
						return MISCREG_ISR_EL1;
					}
					break;
				}
				break;
			case 4:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_VBAR_EL2;
					case 1:
						return MISCREG_RVBAR_EL2;
					}
					break;
				}
				break;
			case 6:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_VBAR_EL3;
					case 1:
						return MISCREG_RVBAR_EL3;
					case 2:
						return MISCREG_RMR_EL3;
					}
					break;
				}
				break;
			}
			break;
		case 13:
			switch (op1) {
			case 0:
				switch (crm) {
				case 0:
					switch (op2) {
					case 1:
						return MISCREG_CONTEXTIDR_EL1;
					case 4:
						return MISCREG_TPIDR_EL1;
					}
					break;
				}
				break;
			case 3:
				switch (crm) {
				case 0:
					switch (op2) {
					case 2:
						return MISCREG_TPIDR_EL0;
					case 3:
						return MISCREG_TPIDRRO_EL0;
					}
					break;
				}
				break;
			case 4:
				switch (crm) {
				case 0:
					switch (op2) {
					case 1:
						return MISCREG_CONTEXTIDR_EL2;
					case 2:
						return MISCREG_TPIDR_EL2;
					}
					break;
				}
				break;
			case 6:
				switch (crm) {
				case 0:
					switch (op2) {
					case 2:
						return MISCREG_TPIDR_EL3;
					}
					break;
				}
				break;
			}
			break;
		case 14:
			switch (op1) {
			case 0:
				switch (crm) {
				case 1:
					switch (op2) {
					case 0:
						return MISCREG_CNTKCTL_EL1;
					}
					break;
				}
				break;
			case 3:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_CNTFRQ_EL0;
					case 1:
						return MISCREG_CNTPCT_EL0;
					case 2:
						return MISCREG_CNTVCT_EL0;
					}
					break;
				case 2:
					switch (op2) {
					case 0:
						return MISCREG_CNTP_TVAL_EL0;
					case 1:
						return MISCREG_CNTP_CTL_EL0;
					case 2:
						return MISCREG_CNTP_CVAL_EL0;
					}
					break;
				case 3:
					switch (op2) {
					case 0:
						return MISCREG_CNTV_TVAL_EL0;
					case 1:
						return MISCREG_CNTV_CTL_EL0;
					case 2:
						return MISCREG_CNTV_CVAL_EL0;
					}
					break;
				case 8:
					switch (op2) {
					case 0:
						return MISCREG_PMEVCNTR0_EL0;
					case 1:
						return MISCREG_PMEVCNTR1_EL0;
					case 2:
						return MISCREG_PMEVCNTR2_EL0;
					case 3:
						return MISCREG_PMEVCNTR3_EL0;
					case 4:
						return MISCREG_PMEVCNTR4_EL0;
					case 5:
						return MISCREG_PMEVCNTR5_EL0;
					}
					break;
				case 12:
					switch (op2) {
					case 0:
						return MISCREG_PMEVTYPER0_EL0;
					case 1:
						return MISCREG_PMEVTYPER1_EL0;
					case 2:
						return MISCREG_PMEVTYPER2_EL0;
					case 3:
						return MISCREG_PMEVTYPER3_EL0;
					case 4:
						return MISCREG_PMEVTYPER4_EL0;
					case 5:
						return MISCREG_PMEVTYPER5_EL0;
					}
					break;
				case 15:
					switch (op2) {
					case 7:
						return MISCREG_PMCCFILTR_EL0;
					}
				}
				break;
			case 4:
				switch (crm) {
				case 0:
					switch (op2) {
					case 3:
						return MISCREG_CNTVOFF_EL2;
					}
					break;
				case 1:
					switch (op2) {
					case 0:
						return MISCREG_CNTHCTL_EL2;
					}
					break;
				case 2:
					switch (op2) {
					case 0:
						return MISCREG_CNTHP_TVAL_EL2;
					case 1:
						return MISCREG_CNTHP_CTL_EL2;
					case 2:
						return MISCREG_CNTHP_CVAL_EL2;
					}
					break;
				}
				break;
			case 7:
				switch (crm) {
				case 2:
					switch (op2) {
					case 0:
						return MISCREG_CNTPS_TVAL_EL1;
					case 1:
						return MISCREG_CNTPS_CTL_EL1;
					case 2:
						return MISCREG_CNTPS_CVAL_EL1;
					}
					break;
				}
				break;
			}
			break;
		case 15:
			switch (op1) {
			case 0:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_IL1DATA0_EL1;
					case 1:
						return MISCREG_IL1DATA1_EL1;
					case 2:
						return MISCREG_IL1DATA2_EL1;
					case 3:
						return MISCREG_IL1DATA3_EL1;
					}
					break;
				case 1:
					switch (op2) {
					case 0:
						return MISCREG_DL1DATA0_EL1;
					case 1:
						return MISCREG_DL1DATA1_EL1;
					case 2:
						return MISCREG_DL1DATA2_EL1;
					case 3:
						return MISCREG_DL1DATA3_EL1;
					case 4:
						return MISCREG_DL1DATA4_EL1;
					}
					break;
				}
				break;
			case 1:
				switch (crm) {
				case 0:
					switch (op2) {
					case 0:
						return MISCREG_L2ACTLR_EL1;
					}
					break;
				case 2:
					switch (op2) {
					case 0:
						return MISCREG_CPUACTLR_EL1;
					case 1:
						return MISCREG_CPUECTLR_EL1;
					case 2:
						return MISCREG_CPUMERRSR_EL1;
					case 3:
						return MISCREG_L2MERRSR_EL1;
					}
					break;
				case 3:
					switch (op2) {
					case 0:
						return MISCREG_CBAR_EL1;

					}
					break;
				}
				break;
			}
			break;
		}
		break;
	}

	return MISCREG_UNKNOWN;
}

static bitset<NUM_MISCREG_INFOS> miscRegInfo[NUM_MISCREGS] = {
        // MISCREG_CPSR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_SPSR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_SPSR_FIQ
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_SPSR_IRQ
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_SPSR_SVC
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_SPSR_MON
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_SPSR_ABT
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_SPSR_HYP
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_SPSR_UND
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_ELR_HYP
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_FPSID
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_FPSCR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_MVFR1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_MVFR0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_FPEXC
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),

        // Helper registers
        // MISCREG_CPSR_MODE
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_CPSR_Q
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_FPSCR_Q
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_FPSCR_EXC
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_LOCKADDR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_LOCKFLAG
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PRRR_MAIR0
        bitset<NUM_MISCREG_INFOS>(string("00000000000000011001")),
        // MISCREG_PRRR_MAIR0_NS
        bitset<NUM_MISCREG_INFOS>(string("00000000000000101001")),
        // MISCREG_PRRR_MAIR0_S
        bitset<NUM_MISCREG_INFOS>(string("00000000000000101001")),
        // MISCREG_NMRR_MAIR1
        bitset<NUM_MISCREG_INFOS>(string("00000000000000011001")),
        // MISCREG_NMRR_MAIR1_NS
        bitset<NUM_MISCREG_INFOS>(string("00000000000000101001")),
        // MISCREG_NMRR_MAIR1_S
        bitset<NUM_MISCREG_INFOS>(string("00000000000000101001")),
        // MISCREG_PMXEVTYPER_PMCCFILTR
        bitset<NUM_MISCREG_INFOS>(string("00000000000000001001")),
        // MISCREG_SCTLR_RST
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_SEV_MAILBOX
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),

        // AArch32 CP14 registers
        // MISCREG_DBGDIDR
        bitset<NUM_MISCREG_INFOS>(string("01011111111111000001")),
        // MISCREG_DBGDSCRint
        bitset<NUM_MISCREG_INFOS>(string("01011111111111000001")),
        // MISCREG_DBGDCCINT
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGDTRTXint
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGDTRRXint
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGWFAR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGVCR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGDTRRXext
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGDSCRext
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000100")),
        // MISCREG_DBGDTRTXext
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGOSECCR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGBVR0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGBVR1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGBVR2
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGBVR3
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGBVR4
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGBVR5
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGBCR0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGBCR1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGBCR2
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGBCR3
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGBCR4
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGBCR5
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGWVR0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGWVR1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGWVR2
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGWVR3
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGWCR0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGWCR1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGWCR2
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGWCR3
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGDRAR
        bitset<NUM_MISCREG_INFOS>(string("01011111111111000000")),
        // MISCREG_DBGBXVR4
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGBXVR5
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGOSLAR
        bitset<NUM_MISCREG_INFOS>(string("10101111111111000000")),
        // MISCREG_DBGOSLSR
        bitset<NUM_MISCREG_INFOS>(string("01011111111111000000")),
        // MISCREG_DBGOSDLR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGPRCR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGDSAR
        bitset<NUM_MISCREG_INFOS>(string("01011111111111000000")),
        // MISCREG_DBGCLAIMSET
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGCLAIMCLR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_DBGAUTHSTATUS
        bitset<NUM_MISCREG_INFOS>(string("01011111111111000000")),
        // MISCREG_DBGDEVID2
        bitset<NUM_MISCREG_INFOS>(string("01011111111111000000")),
        // MISCREG_DBGDEVID1
        bitset<NUM_MISCREG_INFOS>(string("01011111111111000000")),
        // MISCREG_DBGDEVID0
        bitset<NUM_MISCREG_INFOS>(string("01011111111111000000")),
        // MISCREG_TEECR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_JIDR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_TEEHBR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_JOSCR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_JMCR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),

        // AArch32 CP15 registers
        // MISCREG_MIDR
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_CTR
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_TCMTR
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_TLBTR
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_MPIDR
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_REVIDR
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000100")),
        // MISCREG_ID_PFR0
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_PFR1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_DFR0
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_AFR0
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_MMFR0
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_MMFR1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_MMFR2
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_MMFR3
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_ISAR0
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_ISAR1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_ISAR2
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_ISAR3
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_ISAR4
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_ISAR5
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_CCSIDR
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_CLIDR
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_AIDR
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_CSSELR
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_CSSELR_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_CSSELR_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_VPIDR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_VMPIDR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_SCTLR
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_SCTLR_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_SCTLR_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_ACTLR
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_ACTLR_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_ACTLR_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_CPACR
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_SCR
        bitset<NUM_MISCREG_INFOS>(string("11110011000000000001")),
        // MISCREG_SDER
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_NSACR
        bitset<NUM_MISCREG_INFOS>(string("11110111010000000001")),
        // MISCREG_HSCTLR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_HACTLR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_HCR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_HDCR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_HCPTR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_HSTR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_HACR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000100")),
        // MISCREG_TTBR0
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_TTBR0_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_TTBR0_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_TTBR1
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_TTBR1_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_TTBR1_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_TTBCR
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_TTBCR_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_TTBCR_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_HTCR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_VTCR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_DACR
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_DACR_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_DACR_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_DFSR
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_DFSR_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_DFSR_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_IFSR
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_IFSR_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_IFSR_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_ADFSR
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010100")),
        // MISCREG_ADFSR_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100100")),
        // MISCREG_ADFSR_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100100")),
        // MISCREG_AIFSR
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010100")),
        // MISCREG_AIFSR_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100100")),
        // MISCREG_AIFSR_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100100")),
        // MISCREG_HADFSR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_HAIFSR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_HSR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_DFAR
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_DFAR_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_DFAR_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_IFAR
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_IFAR_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_IFAR_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_HDFAR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_HIFAR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_HPFAR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_ICIALLUIS
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000100")),
        // MISCREG_BPIALLIS
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000100")),
        // MISCREG_PAR
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_PAR_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_PAR_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_ICIALLU
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_ICIMVAU
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000100")),
        // MISCREG_CP15ISB
        bitset<NUM_MISCREG_INFOS>(string("10101010101010000001")),
        // MISCREG_BPIALL
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000100")),
        // MISCREG_BPIMVA
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000100")),
        // MISCREG_DCIMVAC
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000100")),
        // MISCREG_DCISW
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000100")),
        // MISCREG_ATS1CPR
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_ATS1CPW
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_ATS1CUR
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_ATS1CUW
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_ATS12NSOPR
        bitset<NUM_MISCREG_INFOS>(string("10101010000000000001")),
        // MISCREG_ATS12NSOPW
        bitset<NUM_MISCREG_INFOS>(string("10101010000000000001")),
        // MISCREG_ATS12NSOUR
        bitset<NUM_MISCREG_INFOS>(string("10101010000000000001")),
        // MISCREG_ATS12NSOUW
        bitset<NUM_MISCREG_INFOS>(string("10101010000000000001")),
        // MISCREG_DCCMVAC
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_DCCSW
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000100")),
        // MISCREG_CP15DSB
        bitset<NUM_MISCREG_INFOS>(string("10101010101010000001")),
        // MISCREG_CP15DMB
        bitset<NUM_MISCREG_INFOS>(string("10101010101010000001")),
        // MISCREG_DCCMVAU
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000100")),
        // MISCREG_DCCIMVAC
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000100")),
        // MISCREG_DCCISW
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000100")),
        // MISCREG_ATS1HR
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000001")),
        // MISCREG_ATS1HW
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000001")),
        // MISCREG_TLBIALLIS
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBIMVAIS
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBIASIDIS
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBIMVAAIS
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBIMVALIS
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000000")),
        // MISCREG_TLBIMVAALIS
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000000")),
        // MISCREG_ITLBIALL
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_ITLBIMVA
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_ITLBIASID
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_DTLBIALL
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_DTLBIMVA
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_DTLBIASID
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBIALL
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBIMVA
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBIASID
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBIMVAA
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBIMVAL
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000000")),
        // MISCREG_TLBIMVAAL
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000000")),
        // MISCREG_TLBIIPAS2IS
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000000")),
        // MISCREG_TLBIIPAS2LIS
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000000")),
        // MISCREG_TLBIALLHIS
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000001")),
        // MISCREG_TLBIMVAHIS
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000001")),
        // MISCREG_TLBIALLNSNHIS
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000001")),
        // MISCREG_TLBIMVALHIS
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000000")),
        // MISCREG_TLBIIPAS2
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000000")),
        // MISCREG_TLBIIPAS2L
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000000")),
        // MISCREG_TLBIALLH
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000001")),
        // MISCREG_TLBIMVAH
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000001")),
        // MISCREG_TLBIALLNSNH
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000001")),
        // MISCREG_TLBIMVALH
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000000")),
        // MISCREG_PMCR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMCNTENSET
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMCNTENCLR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMOVSR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMSWINC
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMSELR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMCEID0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMCEID1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMCCNTR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMXEVTYPER
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMCCFILTR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMXEVCNTR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMUSERENR
        bitset<NUM_MISCREG_INFOS>(string("11111111110101000001")),
        // MISCREG_PMINTENSET
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_PMINTENCLR
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_PMOVSSET
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000000")),
        // MISCREG_L2CTLR
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_L2ECTLR
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000000")),
        // MISCREG_PRRR
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_PRRR_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_PRRR_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_MAIR0
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_MAIR0_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_MAIR0_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_NMRR
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_NMRR_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_NMRR_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_MAIR1
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_MAIR1_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_MAIR1_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_AMAIR0
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_AMAIR0_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_AMAIR0_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_AMAIR1
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_AMAIR1_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_AMAIR1_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_HMAIR0
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_HMAIR1
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_HAMAIR0
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000100")),
        // MISCREG_HAMAIR1
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000100")),
        // MISCREG_VBAR
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_VBAR_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_VBAR_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_MVBAR
        bitset<NUM_MISCREG_INFOS>(string("11110011000000000001")),
        // MISCREG_RMR
        bitset<NUM_MISCREG_INFOS>(string("11110011000000000000")),
        // MISCREG_ISR
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_HVBAR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_FCSEIDR
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000100")),
        // MISCREG_CONTEXTIDR
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_CONTEXTIDR_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_CONTEXTIDR_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_TPIDRURW
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_TPIDRURW_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100111111100001")),
        // MISCREG_TPIDRURW_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_TPIDRURO
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_TPIDRURO_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110101100001")),
        // MISCREG_TPIDRURO_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_TPIDRPRW
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_TPIDRPRW_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100110000100001")),
        // MISCREG_TPIDRPRW_S
        bitset<NUM_MISCREG_INFOS>(string("00110011000000100001")),
        // MISCREG_HTPIDR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_CNTFRQ
        bitset<NUM_MISCREG_INFOS>(string("11110101010101000011")),
        // MISCREG_CNTKCTL
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_CNTP_TVAL
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_CNTP_TVAL_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100111111100001")),
        // MISCREG_CNTP_TVAL_S
        bitset<NUM_MISCREG_INFOS>(string("00110011001111100000")),
        // MISCREG_CNTP_CTL
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_CNTP_CTL_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100111111100001")),
        // MISCREG_CNTP_CTL_S
        bitset<NUM_MISCREG_INFOS>(string("00110011001111100000")),
        // MISCREG_CNTV_TVAL
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_CNTV_CTL
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_CNTHCTL
        bitset<NUM_MISCREG_INFOS>(string("01001000000000000000")),
        // MISCREG_CNTHP_TVAL
        bitset<NUM_MISCREG_INFOS>(string("01001000000000000000")),
        // MISCREG_CNTHP_CTL
        bitset<NUM_MISCREG_INFOS>(string("01001000000000000000")),
        // MISCREG_IL1DATA0
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000000")),
        // MISCREG_IL1DATA1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000000")),
        // MISCREG_IL1DATA2
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000000")),
        // MISCREG_IL1DATA3
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000000")),
        // MISCREG_DL1DATA0
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000000")),
        // MISCREG_DL1DATA1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000000")),
        // MISCREG_DL1DATA2
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000000")),
        // MISCREG_DL1DATA3
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000000")),
        // MISCREG_DL1DATA4
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000000")),
        // MISCREG_RAMINDEX
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000000")),
        // MISCREG_L2ACTLR
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000000")),
        // MISCREG_CBAR
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000000")),
        // MISCREG_HTTBR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_VTTBR
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_CNTPCT
        bitset<NUM_MISCREG_INFOS>(string("01010101010101000001")),
        // MISCREG_CNTVCT
        bitset<NUM_MISCREG_INFOS>(string("01010101010101000011")),
        // MISCREG_CNTP_CVAL
        bitset<NUM_MISCREG_INFOS>(string("00000000000000010001")),
        // MISCREG_CNTP_CVAL_NS
        bitset<NUM_MISCREG_INFOS>(string("11001100111111100001")),
        // MISCREG_CNTP_CVAL_S
        bitset<NUM_MISCREG_INFOS>(string("00110011001111100000")),
        // MISCREG_CNTV_CVAL
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_CNTVOFF
        bitset<NUM_MISCREG_INFOS>(string("11001100000000000001")),
        // MISCREG_CNTHP_CVAL
        bitset<NUM_MISCREG_INFOS>(string("01001000000000000000")),
        // MISCREG_CPUMERRSR
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000000")),
        // MISCREG_L2MERRSR
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000100")),

        // AArch64 registers (Op0=2)
        // MISCREG_MDCCINT_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_OSDTRRX_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_MDSCR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_OSDTRTX_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_OSECCR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGBVR0_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGBVR1_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGBVR2_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGBVR3_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGBVR4_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGBVR5_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGBCR0_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGBCR1_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGBCR2_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGBCR3_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGBCR4_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGBCR5_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGWVR0_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGWVR1_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGWVR2_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGWVR3_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGWCR0_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGWCR1_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGWCR2_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGWCR3_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_MDCCSR_EL0
        bitset<NUM_MISCREG_INFOS>(string("01011111111111000001")),
        // MISCREG_MDDTR_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_MDDTRTX_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_MDDTRRX_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGVCR32_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_MDRAR_EL1
        bitset<NUM_MISCREG_INFOS>(string("01011111111111000001")),
        // MISCREG_OSLAR_EL1
        bitset<NUM_MISCREG_INFOS>(string("10101111111111000001")),
        // MISCREG_OSLSR_EL1
        bitset<NUM_MISCREG_INFOS>(string("01011111111111000001")),
        // MISCREG_OSDLR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGPRCR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGCLAIMSET_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGCLAIMCLR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DBGAUTHSTATUS_EL1
        bitset<NUM_MISCREG_INFOS>(string("01011111111111000001")),
        // MISCREG_TEECR32_EL1
        bitset<NUM_MISCREG_INFOS>(string("00000000000000000001")),
        // MISCREG_TEEHBR32_EL1
        bitset<NUM_MISCREG_INFOS>(string("00000000000000000001")),

        // AArch64 registers (Op0=1,3)
        // MISCREG_MIDR_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_MPIDR_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_REVIDR_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_PFR0_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_PFR1_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_DFR0_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_AFR0_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_MMFR0_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_MMFR1_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_MMFR2_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_MMFR3_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_ISAR0_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_ISAR1_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_ISAR2_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_ISAR3_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_ISAR4_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_ISAR5_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_MVFR0_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_MVFR1_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_MVFR2_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_AA64PFR0_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_AA64PFR1_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_AA64DFR0_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_AA64DFR1_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_AA64AFR0_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_AA64AFR1_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_AA64ISAR0_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_AA64ISAR1_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_AA64MMFR0_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ID_AA64MMFR1_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_CCSIDR_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_CLIDR_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_AIDR_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_CSSELR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_CTR_EL0
        bitset<NUM_MISCREG_INFOS>(string("01010101010101000001")),
        // MISCREG_DCZID_EL0
        bitset<NUM_MISCREG_INFOS>(string("01010101010101000001")),
        // MISCREG_VPIDR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_VMPIDR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_SCTLR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_ACTLR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_CPACR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_SCTLR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_ACTLR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_HCR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_MDCR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_CPTR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_HSTR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_HACR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_SCTLR_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_ACTLR_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_SCR_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_SDER32_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_CPTR_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_MDCR_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_TTBR0_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_TTBR1_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_TCR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_TTBR0_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_TCR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_VTTBR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_VTCR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_TTBR0_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_TCR_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_DACR32_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_SPSR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_ELR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_SP_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_SPSEL
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_CURRENTEL
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_NZCV
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DAIF
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_FPCR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_FPSR
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DSPSR_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_DLR_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_SPSR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_ELR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_SP_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_SPSR_IRQ_AA64
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_SPSR_ABT_AA64
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_SPSR_UND_AA64
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_SPSR_FIQ_AA64
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_SPSR_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_ELR_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_SP_EL2
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_AFSR0_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_AFSR1_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_ESR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_IFSR32_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_AFSR0_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_AFSR1_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_ESR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_FPEXC32_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_AFSR0_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_AFSR1_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_ESR_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_FAR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_FAR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_HPFAR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_FAR_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_IC_IALLUIS
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000101")),
        // MISCREG_PAR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_IC_IALLU
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000101")),
        // MISCREG_DC_IVAC_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000101")),
        // MISCREG_DC_ISW_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000101")),
        // MISCREG_AT_S1E1R_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_AT_S1E1W_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_AT_S1E0R_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_AT_S1E0W_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_DC_CSW_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000101")),
        // MISCREG_DC_CISW_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000101")),
        // MISCREG_DC_ZVA_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100010000101")),
        // MISCREG_IC_IVAU_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010101010000001")),
        // MISCREG_DC_CVAC_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010101010000101")),
        // MISCREG_DC_CVAU_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010101010000101")),
        // MISCREG_DC_CIVAC_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010101010000101")),
        // MISCREG_AT_S1E2R_Xt
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000001")),
        // MISCREG_AT_S1E2W_Xt
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000001")),
        // MISCREG_AT_S12E1R_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101000000000000001")),
        // MISCREG_AT_S12E1W_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101000000000000001")),
        // MISCREG_AT_S12E0R_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101000000000000001")),
        // MISCREG_AT_S12E0W_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101000000000000001")),
        // MISCREG_AT_S1E3R_Xt
        bitset<NUM_MISCREG_INFOS>(string("10100000000000000001")),
        // MISCREG_AT_S1E3W_Xt
        bitset<NUM_MISCREG_INFOS>(string("10100000000000000001")),
        // MISCREG_TLBI_VMALLE1IS
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBI_VAE1IS_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBI_ASIDE1IS_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBI_VAAE1IS_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBI_VALE1IS_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBI_VAALE1IS_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBI_VMALLE1
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBI_VAE1_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBI_ASIDE1_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBI_VAAE1_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBI_VALE1_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBI_VAALE1_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101010100000000001")),
        // MISCREG_TLBI_IPAS2E1IS_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101000000000000001")),
        // MISCREG_TLBI_IPAS2LE1IS_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101000000000000001")),
        // MISCREG_TLBI_ALLE2IS
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000001")),
        // MISCREG_TLBI_VAE2IS_Xt
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000001")),
        // MISCREG_TLBI_ALLE1IS
        bitset<NUM_MISCREG_INFOS>(string("10101000000000000001")),
        // MISCREG_TLBI_VALE2IS_Xt
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000001")),
        // MISCREG_TLBI_VMALLS12E1IS
        bitset<NUM_MISCREG_INFOS>(string("10101000000000000001")),
        // MISCREG_TLBI_IPAS2E1_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101000000000000001")),
        // MISCREG_TLBI_IPAS2LE1_Xt
        bitset<NUM_MISCREG_INFOS>(string("10101000000000000001")),
        // MISCREG_TLBI_ALLE2
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000001")),
        // MISCREG_TLBI_VAE2_Xt
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000001")),
        // MISCREG_TLBI_ALLE1
        bitset<NUM_MISCREG_INFOS>(string("10101000000000000001")),
        // MISCREG_TLBI_VALE2_Xt
        bitset<NUM_MISCREG_INFOS>(string("10001000000000000001")),
        // MISCREG_TLBI_VMALLS12E1
        bitset<NUM_MISCREG_INFOS>(string("10101000000000000001")),
        // MISCREG_TLBI_ALLE3IS
        bitset<NUM_MISCREG_INFOS>(string("10100000000000000001")),
        // MISCREG_TLBI_VAE3IS_Xt
        bitset<NUM_MISCREG_INFOS>(string("10100000000000000001")),
        // MISCREG_TLBI_VALE3IS_Xt
        bitset<NUM_MISCREG_INFOS>(string("10100000000000000001")),
        // MISCREG_TLBI_ALLE3
        bitset<NUM_MISCREG_INFOS>(string("10100000000000000001")),
        // MISCREG_TLBI_VAE3_Xt
        bitset<NUM_MISCREG_INFOS>(string("10100000000000000001")),
        // MISCREG_TLBI_VALE3_Xt
        bitset<NUM_MISCREG_INFOS>(string("10100000000000000001")),
        // MISCREG_PMINTENSET_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_PMINTENCLR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_PMCR_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMCNTENSET_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMCNTENCLR_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMOVSCLR_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMSWINC_EL0
        bitset<NUM_MISCREG_INFOS>(string("10101010101111000001")),
        // MISCREG_PMSELR_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMCEID0_EL0
        bitset<NUM_MISCREG_INFOS>(string("01010101011111000001")),
        // MISCREG_PMCEID1_EL0
        bitset<NUM_MISCREG_INFOS>(string("01010101011111000001")),
        // MISCREG_PMCCNTR_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMXEVTYPER_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMCCFILTR_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMXEVCNTR_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMUSERENR_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111110101000001")),
        // MISCREG_PMOVSSET_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_MAIR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_AMAIR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_MAIR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_AMAIR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_MAIR_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_AMAIR_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_L2CTLR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_L2ECTLR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_VBAR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_RVBAR_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_ISR_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_VBAR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_RVBAR_EL2
        bitset<NUM_MISCREG_INFOS>(string("01010100000000000001")),
        // MISCREG_VBAR_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_RVBAR_EL3
        bitset<NUM_MISCREG_INFOS>(string("01010000000000000001")),
        // MISCREG_RMR_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_CONTEXTIDR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_TPIDR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_TPIDR_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_TPIDRRO_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111110101000001")),
        // MISCREG_TPIDR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_TPIDR_EL3
        bitset<NUM_MISCREG_INFOS>(string("11110000000000000001")),
        // MISCREG_CNTKCTL_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_CNTFRQ_EL0
        bitset<NUM_MISCREG_INFOS>(string("11110101010101000001")),
        // MISCREG_CNTPCT_EL0
        bitset<NUM_MISCREG_INFOS>(string("01010101010101000001")),
        // MISCREG_CNTVCT_EL0
        bitset<NUM_MISCREG_INFOS>(string("01010101010101000011")),
        // MISCREG_CNTP_TVAL_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_CNTP_CTL_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_CNTP_CVAL_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_CNTV_TVAL_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_CNTV_CTL_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_CNTV_CVAL_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMEVCNTR0_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMEVCNTR1_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMEVCNTR2_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMEVCNTR3_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMEVCNTR4_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMEVCNTR5_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMEVTYPER0_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMEVTYPER1_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMEVTYPER2_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMEVTYPER3_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMEVTYPER4_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_PMEVTYPER5_EL0
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_CNTVOFF_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),
        // MISCREG_CNTHCTL_EL2
        bitset<NUM_MISCREG_INFOS>(string("01111000000000000000")),
        // MISCREG_CNTHP_TVAL_EL2
        bitset<NUM_MISCREG_INFOS>(string("01111000000000000000")),
        // MISCREG_CNTHP_CTL_EL2
        bitset<NUM_MISCREG_INFOS>(string("01111000000000000000")),
        // MISCREG_CNTHP_CVAL_EL2
        bitset<NUM_MISCREG_INFOS>(string("01111000000000000000")),
        // MISCREG_CNTPS_TVAL_EL1
        bitset<NUM_MISCREG_INFOS>(string("01111000000000000000")),
        // MISCREG_CNTPS_CTL_EL1
        bitset<NUM_MISCREG_INFOS>(string("01111000000000000000")),
        // MISCREG_CNTPS_CVAL_EL1
        bitset<NUM_MISCREG_INFOS>(string("01111000000000000000")),
        // MISCREG_IL1DATA0_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_IL1DATA1_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_IL1DATA2_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_IL1DATA3_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_DL1DATA0_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_DL1DATA1_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_DL1DATA2_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_DL1DATA3_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_DL1DATA4_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_L2ACTLR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_CPUACTLR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_CPUECTLR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_CPUMERRSR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000001")),
        // MISCREG_L2MERRSR_EL1
        bitset<NUM_MISCREG_INFOS>(string("11111111110000000100")),
        // MISCREG_CBAR_EL1
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_CONTEXTIDR_EL2
        bitset<NUM_MISCREG_INFOS>(string("11111100000000000001")),

        // Dummy registers
        // MISCREG_NOP
        bitset<NUM_MISCREG_INFOS>(string("11111111111111000001")),
        // MISCREG_RAZ
        bitset<NUM_MISCREG_INFOS>(string("01010101010000000001")),
        // MISCREG_CP14_UNIMPL
        bitset<NUM_MISCREG_INFOS>(string("00000000000000000100")),
        // MISCREG_CP15_UNIMPL
        bitset<NUM_MISCREG_INFOS>(string("00000000000000000100")),
        // MISCREG_A64_UNIMPL
        bitset<NUM_MISCREG_INFOS>(string("00000000000000000100")),
        // MISCREG_UNKNOWN
        bitset<NUM_MISCREG_INFOS>(string("00000000000000000001"))
};

#endif //DECODE_INST_MISCREGINFO_H
