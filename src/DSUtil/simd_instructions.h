/*
 * (C) 2003-2006 Gabest
 * (C) 2006-2014 see Authors.txt
 *
 * This file is part of MPC-BE.
 *
 * MPC-BE is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * MPC-BE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

MMX_INSTRUCTION(paddb,_mm_add_pi8)
MMX_INSTRUCTION(paddsb,_mm_adds_pi8)
MMX_INSTRUCTION(paddusb,_mm_adds_pu8)
MMX_INSTRUCTION(paddw,_mm_add_pi16)
MMX_INSTRUCTION(paddsw,_mm_adds_pi16)
MMX_INSTRUCTION(paddusw,_mm_adds_pu16)
MMX_INSTRUCTION(paddd,_mm_add_pi32)

MMX_INSTRUCTION(pmaddwd,_mm_madd_pi16)

MMX_INSTRUCTION(psubb,_mm_sub_pi8)
MMX_INSTRUCTION(psubsb,_mm_subs_pi8)
MMX_INSTRUCTION(psubusb,_mm_subs_pu8)
MMX_INSTRUCTION(psubw,_mm_sub_pi16)
MMX_INSTRUCTION(psubsw,_mm_subs_pi16)
MMX_INSTRUCTION(psubusw,_mm_subs_pu16)
MMX_INSTRUCTION(psubd,_mm_sub_pi32)

MMX_INSTRUCTION(pmullw,_mm_mullo_pi16)
MMX_INSTRUCTION(pmulhw,_mm_mulhi_pi16)

MMX_INSTRUCTION(pand,_mm_and_si64)
MMX_INSTRUCTION(pandn,_mm_andnot_si64)
MMX_INSTRUCTION(por,_mm_or_si64)
MMX_INSTRUCTION(pxor,_mm_xor_si64)

MMX_INSTRUCTION(pcmpeqb,_mm_cmpeq_pi8)
MMX_INSTRUCTION(pcmpeqw,_mm_cmpeq_pi16)
MMX_INSTRUCTION(pcmpeqd,_mm_cmpeq_pi32)
MMX_INSTRUCTION(pcmpgtb,_mm_cmpgt_pi8)
MMX_INSTRUCTION(pcmpgtw,_mm_cmpgt_pi16)
MMX_INSTRUCTION(pcmpgtd,_mm_cmpgt_pi32)

MMX_INSTRUCTION(packuswb,_mm_packs_pu16)
MMX_INSTRUCTION(packsswb,_mm_packs_pi16)
MMX_INSTRUCTION(packssdw,_mm_packs_pi32)

MMX_INSTRUCTION(punpcklbw,_mm_unpacklo_pi8)
MMX_INSTRUCTION(punpckhbw,_mm_unpackhi_pi8)

MMX_INSTRUCTION(punpcklwd,_mm_unpacklo_pi16)
MMX_INSTRUCTION(punpckhwd,_mm_unpackhi_pi16)
MMX_INSTRUCTION(punpckldq,_mm_unpacklo_pi32)
MMX_INSTRUCTION(punpckhdq,_mm_unpackhi_pi32)

MMX_INSTRUCTION(pminub,_mm_min_pu8)
MMX_INSTRUCTION(pminsw,_mm_min_pi16)
MMX_INSTRUCTION(pmaxub,_mm_max_pu8)
MMX_INSTRUCTION(pmaxsw,_mm_max_pi16)

MMX_INSTRUCTION(pavgb,_mm_avg_pu8)

MMX_INSTRUCTION(psadbw,_mm_sad_pu8)

#ifdef __SSE2__

SSE2I_INSTRUCTION(pand,_mm_and_si128)
SSE2I_INSTRUCTION(por,_mm_or_si128)
SSE2I_INSTRUCTION(pxor,_mm_xor_si128)
SSE2I_INSTRUCTION(packuswb,_mm_packus_epi16)
SSE2I_INSTRUCTION(packsswb,_mm_packs_epi16)
SSE2I_INSTRUCTION(packssdw,_mm_packs_epi32)
SSE2I_INSTRUCTION(punpcklbw,_mm_unpacklo_epi8)
SSE2I_INSTRUCTION(punpckhbw,_mm_unpackhi_epi8)
SSE2I_INSTRUCTION(punpcklwd,_mm_unpacklo_epi16)
SSE2I_INSTRUCTION(punpckhwd,_mm_unpackhi_epi16)
SSE2I_INSTRUCTION(punpckldq,_mm_unpacklo_epi32)
SSE2I_INSTRUCTION(pmullw,_mm_mullo_epi16)
SSE2I_INSTRUCTION(pmulhw,_mm_mulhi_epi16)
SSE2I_INSTRUCTION(paddsb,_mm_adds_epi8)
SSE2I_INSTRUCTION(paddb,_mm_add_epi8)
SSE2I_INSTRUCTION(paddw,_mm_add_epi16)
SSE2I_INSTRUCTION(paddsw,_mm_adds_epi16)
SSE2I_INSTRUCTION(paddusw,_mm_adds_epu16)
SSE2I_INSTRUCTION(paddd,_mm_add_epi32)
SSE2I_INSTRUCTION(psubw,_mm_sub_epi16)
SSE2I_INSTRUCTION(psubsw,_mm_subs_epi16)
SSE2I_INSTRUCTION(psubusb,_mm_subs_epu8)
SSE2I_INSTRUCTION(psubd,_mm_sub_epi32)
SSE2I_INSTRUCTION(pmaddwd,_mm_madd_epi16)
SSE2I_INSTRUCTION(pavgb,_mm_avg_epu8)
SSE2I_INSTRUCTION(pcmpeqb,_mm_cmpeq_epi8)
SSE2I_INSTRUCTION(pcmpeqw,_mm_cmpeq_epi16)
SSE2I_INSTRUCTION(pcmpgtb,_mm_cmpgt_epi8)
SSE2I_INSTRUCTION(pcmpgtw,_mm_cmpgt_epi16)
SSE2I_INSTRUCTION(paddusb,_mm_adds_epu8)

#endif
