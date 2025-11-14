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
 * @file ns_aes.h
 * @author Nations Firmware Team
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

/** @addtogroup 
 * @{
 */
#ifndef __NS_AES_H__
#define __NS_AES_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes -----------------------------------------------------------------*/
#include "global_func.h"
#include "aes.h"
#include "aes_int.h"
/* Public typedef -----------------------------------------------------------*/

/* Public define ------------------------------------------------------------*/  
#ifndef AES_DECRYPT_ENABLE
#define AES_DECRYPT_ENABLE 0
#endif
#define DECRYPT_SRC_MARK  0xffffffff
/* Public constants ---------------------------------------------------------*/
/* Public function prototypes -----------------------------------------------*/
void ns_ase_swap_array(uint8_t * p_out, uint8_t const * p_in, uint8_t size);
void ns_aes_xor_128_swap(uint8_t* result, const uint8_t* a, const uint8_t* b, uint8_t size);
void ns_aes_xor_128_lsb(uint8_t* result, const uint8_t* a, const uint8_t* b, uint8_t size, uint8_t offset);
void ns_aes_xor_128_msb(uint8_t* result, const uint8_t* a, const uint8_t* b, uint8_t size);

void ns_aes_start(struct aes_func_env* env, const uint8_t* key, const uint8_t *val);
#if AES_DECRYPT_ENABLE
void ns_aes_deciphering(uint8_t *key, uint8_t *val, aes_func_result_cb res_cb);
#endif
void ns_aes_ciphering(uint8_t *key, uint8_t *val, aes_func_result_cb res_cb);



/**
 * @brief Start the AES CBC crypto function. Allocate memory for the CBC and start processing it
 *        Execute result callback at end of function execution
 *
 * @param[in]  key               Pointer to the Key to be used
 * @param[in]  iv                16 Bytes iv to use for cipher/decipher
 * @param[in]  in_message        Input message for AES-CBC exectuion
 * @param[out] out_message       Output message that will contain cipher or decipher data
 * @param[in]  message_len       Length of Input/Output message without mic
 * @param[in]  cipher            True to encrypt message, False to decrypt it.
 * @param[in]  res_cb            Function that will handle the AES CCM result
 */
void ns_aes_cbc(const uint8_t* key, const uint8_t* iv, const uint8_t* in_message, uint8_t* out_message,
                bool cipher, uint16_t message_len, aes_func_result_cb res_cb);


/**
 * @brief Start the AES CCM crypto function. Allocate memory for the CCM and start processing it
 *        Execute result callback at end of function execution
 *
 * @param[in]  key               Pointer to the Key to be used (LSB mode!!!!)
 * @param[in]  nonce             13 Bytes Nonce to use for cipher/decipher (MSB)
 * @param[in]  in_message        Input message for AES-CCM exectuion (MSB)
 * @param[out] out_message       Output message that will contain cipher+mic or decipher data
 * @param[in]  message_len       Length of Input/Output message without mic
 * @param[in]  mic_len           Length of the mic to use (2, 4, 6, 8, 10, 12, 14, 16 valid)
 * @param[in]  cipher            True to encrypt message, False to decrypt it.
 * @param[in]  add_auth_data     Additional Authentication data used for computation of MIC (MSB)
 * @param[in]  add_auth_data_len Length of Additional Authentication data
 * @param[in]  res_cb            Function that will handle the AES CCM result
 * @param[in]  src_info          Information used retrieve requester
 */
void aes_ccm(const uint8_t* key, const uint8_t* nonce, const uint8_t* in_message,
             uint8_t* out_message, uint16_t message_len, uint8_t mic_len, bool cipher,
             const uint8_t* add_auth_data, uint8_t add_auth_data_len, aes_ccm_func_result_cb res_cb, uint32_t src_info);
#define ns_aes_ccm aes_ccm

#endif /* __NS_AES_H__ */
/**
 * @}
 */


