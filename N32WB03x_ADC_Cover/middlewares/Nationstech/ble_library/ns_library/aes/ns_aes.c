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
 * @file ns_aes.c
 * @author Nations Firmware Team
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

/** @addtogroup 
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "ns_aes.h"
#include "ns_log.h"
/* Private typedef -----------------------------------------------------------*/
/// Structure definition used for AES with a cache
struct aes_cache_env
{
    /// AES Environment structure
    struct aes_func_env aes_env;
    /// Cached Key
    uint8_t key_cache[KEY_LEN];
    /// Cached value
    uint8_t val_cache[KEY_LEN];
};
/// AES environment structure
struct aes_env_tag
{
    /// List of AES function to execute
    struct co_list  queue;
    /// AES under execution
    bool aes_ongoing;
};
extern struct aes_env_tag aes_env;
/* Private define ------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//volatile bool m_aes_ready = false;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  conver data betweem big endian and little endian
 *
 * @param[out] p_out  Output number: p_out = (MSB <=> LSB)
 * @param[in]  p_in   input data to swap
 * @param[in]  size   number of bytes to swap
 */
void ns_ase_swap_array(uint8_t * p_out, uint8_t const * p_in, uint8_t size)
{
    uint8_t const * p_first = p_in;
    uint8_t * p_last = p_out + size - 1;
    while (p_last >= p_out)
    {
        *p_last = *p_first;
        p_first++;
        p_last--;
    }
}

/**
 * @brief Perform a XOR of two numbers (output parameters are LSB).
 *
 * @param[out] result Output number: result = ((a ^ b) => LSB)
 * @param[in]  a      first operand MSB 
 * @param[in]  b      second operand MSB
 * @param[in]  size   number of bytes to XOR
 */
void ns_aes_xor_128_swap(uint8_t* result, const uint8_t* a, const uint8_t* b, uint8_t size)
{
    uint8_t i;
    uint8_t offset = size - 1;

    for(i = 0; i < size ; i++)
    {
        result[offset - i] = a[i] ^ b[i];
    }
}
/**
 * @brief Perform a XOR of two numbers (output and first parameters are LSB).
 *
 * @param[out] result Output number: result = a ^ b => LSB (128 bits block)
 * @param[in]  a      first operand LSB (128 bits block)
 * @param[in]  b      second operand MSB
 * @param[in]  size   number of bytes to XOR
 * @param[in]  offset Position offset in the payload
 */
void ns_aes_xor_128_lsb(uint8_t* result, const uint8_t* a, const uint8_t* b, uint8_t size, uint8_t offset)
{
    uint8_t i;
    offset = AES_BLOCK_SIZE - offset - 1;

    for(i = 0; i < size ; i++)
    {
        result[offset - i] = a[offset - i] ^ b[i];
    }
}


/**
 * @brief Perform a XOR of two numbers (output and first parameters are MSB).
 *
 * @param[out] result Output number: result = a ^ b => MSB (variable size block)
 * @param[in]  a      first operand MSB (variable size block)
 * @param[in]  b      second operand LSB (128 bits block)
 * @param[in]  size   number of bytes to XOR
 */
void ns_aes_xor_128_msb(uint8_t* result, const uint8_t* a, const uint8_t* b, uint8_t size)
{
    uint8_t i;

    for(i = 0; i < size ; i++)
    {
        result[i] = a[i] ^ b[AES_BLOCK_SIZE - i - 1];
    }
}

#if AES_DECRYPT_ENABLE
/**
 * @brief  AES result handler
 * @param    
 * @return 
 * @note   Note
 */
void ns_aes_result_handler(uint8_t status, uint8_t* result)
{
    // extract first element of the list that contains
    struct aes_func_env* env = (struct aes_func_env*)      co_list_pop_front(&(aes_env.queue));
    struct aes_func_env* env_next = (struct aes_func_env*) co_list_pick(&(aes_env.queue));
    aes_env.aes_ongoing      = false;
    
    // Reset mode to encrypt
    ip_aescntl_aes_mode_setf(0);
    // Prepare new AES Run if requested
    if(env_next != NULL)
    {
        aes_env.aes_ongoing = true;
        if(env_next->src_info == DECRYPT_SRC_MARK)
        {
            ip_aescntl_aes_mode_setf(1);
        }
        rwip_aes_encrypt(env_next->key, env_next->val);
    }

    // Check that AES result has a requester
    if(env)
    {
        bool finished = true;

        // check status of current result
        if(status == CO_ERROR_NO_ERROR)
        {
            // continue function computation
            if(env->aes_continue_cb != NULL)
            {
                finished = env->aes_continue_cb(env, result);
            }
        }

        // if function execution is over
        if(finished)
        {
            // Inform requester of end of AES based algorithm
            if(env->aes_res_cb != NULL)
            {
                env->aes_res_cb(status, result, env->src_info);
            }

            // remove function environment
            ke_free(env);
        }
    }
}
/**
 *
 * @brief Handles crypto event (to provide results out of interrupt context
 *
 */
__STATIC void ns_rwip_crypt_evt_handler(void)
{
    uint8_t aes_result[KEY_LEN];

    // Clear event
    ke_event_clear(KE_EVENT_AES_END);

    // Load AES result
    em_rd(aes_result, EM_ENC_OUT_OFFSET, KEY_LEN);
    
    // inform AES result handler
    ns_aes_result_handler(CO_ERROR_NO_ERROR, aes_result);
}


bool ns_aes_continue_cb(struct aes_cache_env* aes_env, uint8_t* aes_res)
{
    uint8_t res_cache[KEY_LEN];
    ns_ase_swap_array(res_cache, aes_res, KEY_LEN);
    aes_env->aes_env.aes_res_cb(true,res_cache,aes_env->aes_env.src_info);
    aes_env->aes_env.aes_res_cb = NULL; //result has been return, cancel res_cb
    return true;
}


/**
 * @brief  aes deciphering
 * @param    
 * @return 
 * @note   Note
 */
void ns_aes_deciphering(uint8_t *key, uint8_t *val, aes_func_result_cb res_cb)
{
    // allocate environment for the AES execution
    struct aes_cache_env* env = (struct aes_cache_env*) aes_alloc(sizeof(struct aes_cache_env), 
                                    (aes_func_continue_cb)ns_aes_continue_cb, res_cb, DECRYPT_SRC_MARK);
    
    // Copy keys and val
    ns_ase_swap_array(env->key_cache, key, KEY_LEN);
    ns_ase_swap_array(env->val_cache, val, KEY_LEN);

    // Start AES execution
    ns_aes_start(&(env->aes_env), env->key_cache, env->val_cache);
}

#endif
/**
 * @brief  aes ciphering
 * @param    
 * @return 
 * @note   Note
 */
void ns_aes_ciphering(uint8_t *key, uint8_t *val, aes_func_result_cb res_cb)
{
    // allocate environment for the AES execution
    struct aes_cache_env* env = (struct aes_cache_env*) aes_alloc(sizeof(struct aes_cache_env), 
                                (aes_func_continue_cb)ns_aes_continue_cb, res_cb, 0);
    
    // Copy keys and val
    ns_ase_swap_array(env->key_cache, key, KEY_LEN);
    ns_ase_swap_array(env->val_cache, val, KEY_LEN);

    // Start AES execution
    ns_aes_start(&(env->aes_env), env->key_cache, env->val_cache);
}


/**
 * @brief AES Cypher request function.
 *
 * This will queue AES request in the AES execution queue
 * When the AES result is received, the AES continue callback is executed.
 *
 * If AES continue function returns that AES execution is over, a message will be send to destination task
 * with latest AES result.
 *
 * @param[in] env  AES environment
 * @param[in] key  Key used for cyphering
 * @param[in] val  Value to cypher
 */
void ns_aes_start(struct aes_func_env* env, const uint8_t* key, const uint8_t *val)
{
    // put function environment at end of list.
    co_list_push_back(&(aes_env.queue), &(env->hdr));

    // store parameter information
    env->key = key;
    env->val = val;

    // AES encryption can be immediately performed
    if(!aes_env.aes_ongoing)
    {
        if(env->src_info == DECRYPT_SRC_MARK)
        {
            // decrypt mode
            ip_aescntl_aes_mode_setf(1);
            //config AES event callback
            ke_event_callback_set(KE_EVENT_AES_END, &ns_rwip_crypt_evt_handler);
        }
        aes_env.aes_ongoing = true;
        rwip_aes_encrypt(env->key, env->val);
    }
}


/**
 * @}
 */

