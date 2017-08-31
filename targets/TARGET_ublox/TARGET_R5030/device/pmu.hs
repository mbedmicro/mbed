; 
; Copyright (C) u-blox
; All rights reserved.
; This source file is the sole property of u-blox. Reproduction or utilization
; of this source in whole or part is forbidden without the written consent of
; u-blox.
; 
; FILE: pmu.hs

NR_PMUEVCNT EQU 4
PMU_PMEVCNTR EQU 0x000
PMU_PMEVCNTR_FIELDS EQU NR_PMUEVCNT
PMU_PMEVTYPER EQU 0x400
PMU_PMEVTYPER_FIELDS EQU NR_PMUEVCNT
PMU_PMCNTENSET EQU 0xC00
PMU_PMCNTENCLR EQU 0xC20
PMU_PMINTENSET EQU 0xC40
PMU_PMINTENCLR EQU 0xC60
PMU_PMOVSR EQU 0xC80
PMU_PMSWINC EQU 0xCA0
PMU_PMAXIDEST EQU 0xD80
PMU_PMFLTSR EQU 0xD84
PMU_PMFLTENSET EQU 0xD90
PMU_PMFLTENCLR EQU 0xDA0
PMU_PMCFGR EQU 0xE00
PMU_PMCR EQU 0xE04
PMU_PMLAR EQU 0xFB0
PMU_PMLSR EQU 0xFB4
PMU_PMAUTHSTATUS EQU 0xFB8
PMU_PMDEVID EQU 0xFC8
PMU_PMDEVTYPE EQU 0xFCC
PMU_PERIPHERALID4 EQU 0xFD0
PMU_PERIPHERALID5 EQU 0xFD4
PMU_PERIPHERALID6 EQU 0xFD8
PMU_PERIPHERALID7 EQU 0xFDC
PMU_PERIPHERALID0 EQU 0xFE0
PMU_PERIPHERALID1 EQU 0xFE4
PMU_PERIPHERALID2 EQU 0xFE8
PMU_PERIPHERALID3 EQU 0xFEC
PMU_COMPONENTID0 EQU 0xFF0
PMU_COMPONENTID1 EQU 0xFF4
PMU_COMPONENTID2 EQU 0xFF8
PMU_COMPONENTID3 EQU 0xFFC
; EOF: pmu.hs
   END
