/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file main.h
 * @author Nations Firmware Team
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */
#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32wb03x.h"
#include "log.h"
#include "string.h"
#define  KEYSCAN_INFO_CHECK         2
static const uint32_t keyscan_map[][KEYSCAN_INFO_CHECK] = 
{
    0x00,0x00,               //NULL
    0x00000000,0x00000001,
    0x00000000,0x00000002,
    0x00000000,0x00000004,
    0x00000000,0x00000008,
    0x00000000,0x00000010,
    0x00000000,0x00000020,
    0x00000000,0x00000040,
    0x00000000,0x00000080,
    0x00000000,0x00000100,
    0x00000000,0x00000200,
    0x00000000,0x00000400,
    0x00000000,0x00000800,
    0x00000000,0x00001000,
    0x00000000,0x00002000,
    0x00000000,0x00004000,
    0x00000000,0x00010000,
    0x00000000,0x00020000,
    0x00000000,0x00040000,
    0x00000000,0x00080000,
    0x00000000,0x00100000,
    0x00000000,0x00200000,
    0x00000000,0x00400000,
    0x00000000,0x00800000,
    0x00000000,0x01000000,
    0x00000000,0x02000000,
    0x00000000,0x04000000,
    0x00000000,0x08000000,
    0x00000000,0x10000000,
    0x00000001,0x00000000,
    0x00000002,0x00000000,
    0x00000004,0x00000000,
    0x00000008,0x00000000,
    0x00000010,0x00000000,
    0x00000020,0x00000000,
    0x00000040,0x00000000,
    0x00000080,0x00000000,
    0x00010000,0x00000000,
    0x00020000,0x00000000,
    0x00040000,0x00000000,
    0x00080000,0x00000000,
    0x00100000,0x00000000,
    0x00200000,0x00000000,
    0x00400000,0x00000000,
    0x00800000,0x00000000,

    0x00,0x00,              //NULL

};

enum KEY_NUM{
    KEYSCAN_NULL = 0,
    
    KEYSCAN_KEY1,
    KEYSCAN_KEY2,
    KEYSCAN_KEY3,
    KEYSCAN_KEY4,
    KEYSCAN_KEY5,
    KEYSCAN_KEY6,
    KEYSCAN_KEY7,
    KEYSCAN_KEY8,
    KEYSCAN_KEY9, 
    KEYSCAN_KEY10,
    KEYSCAN_KEY11,
    KEYSCAN_KEY12,
    KEYSCAN_KEY13,
    KEYSCAN_KEY14,
    KEYSCAN_KEY15,
    KEYSCAN_KEY16, 
    KEYSCAN_KEY17,
    KEYSCAN_KEY18,
    KEYSCAN_KEY19,
    KEYSCAN_KEY20,
    KEYSCAN_KEY21,
    KEYSCAN_KEY22,
    KEYSCAN_KEY23,
    KEYSCAN_KEY24,
    KEYSCAN_KEY25,
    KEYSCAN_KEY26,
    KEYSCAN_KEY27,
    KEYSCAN_KEY28,
    KEYSCAN_KEY29,
    KEYSCAN_KEY30,
    KEYSCAN_KEY31,
    KEYSCAN_KEY32,
    KEYSCAN_KEY33,
    KEYSCAN_KEY34,
    KEYSCAN_KEY35,
    KEYSCAN_KEY36,
    KEYSCAN_KEY37,
    KEYSCAN_KEY38,
    KEYSCAN_KEY39,
    KEYSCAN_KEY40,
    KEYSCAN_KEY41,
    KEYSCAN_KEY42,
    KEYSCAN_KEY43,
    KEYSCAN_KEY44,

    KEYSCAN_KEY_MAX
}; 


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */
