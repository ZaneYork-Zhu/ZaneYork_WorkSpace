/**
 ****************************************************************************************
 * *
 * @file aes_cbc.c
 *
 * @brief Definition file for AES CBC crypto module functions
 *
 * Copyright (C) Nations Technologies Inc.
 *
 *
 *
 *   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *   +                   Algorithm AES-CBC                              +
 *   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *   +                                                                   +
 *   +   Input    : K    ( 128-bit key )                                 +
 *   +   Input    : iv   ( 128-bit iv )                                  +
 *   +            : M    ( message to be encipher or decipher)           +
 *   +            : len  ( length of the message in octets )             +
 *   +   Output   : O    ( message after encipher or decipher)           +
 *   +                                                                   +
 *   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *   +                                                                   +
 *   +   Constants: const_Zero is 0x00000000000000000000000000000000     +
 *   +              const_Bsize is 16                                    +
 *   +                                                                   +
 *   +   Variables: M_i is the i-th block (i=1..ceil(len/const_Bsize))   +
 *   +              n      for number of blocks to be processed          +
 *   +              r      for number of octets of last block            +
 *   +              O      for number of octets of last block            +
 *   +                                                                   +
 *   +   Step 1.  n := ceil(len/const_Bsize);                            +
 *   +   Step 2.  X := iv;                                               +
 *   +   Step 3.  for i := 0 to n-1 do                                   +
 *   +                begin                                              +
 *   +                  Y := X XOR M_i;                                  +
 *   +                  X := AES-128(K,Y);                               +
 *   +                  O := O+X                                         +
 *   +                end                                                +
 *   +   Step 4.  return O;                                              +
 *   +                                                                   +
 *   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup AES
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_EMB_PRESENT || BLE_HOST_PRESENT)

#include "arch.h"     // architecture defines
#include "aes_int.h"  // AES internals
#include <string.h>   // for memcpy function
#include "co_math.h"
#include "ns_aes.h"  



struct aes_cbc_env
{
    /// AES Environment structure
    struct aes_func_env aes_env;

    /// M: Pointer to the message to be cypher
    const uint8_t*      message; // pointer to memory allocated by calling function
    const uint8_t*      iv;

    /// O: message to output
    uint8_t*            output;

    /// K: authentication key
    uint8_t      key_cache[AES_BLOCK_SIZE];
    uint8_t      val_cache[AES_BLOCK_SIZE];
    /// Length of the message
    uint16_t            message_len;
    ///  Number of blocks (1 block = 16 bytes)
    uint8_t             num_blocks;
    /// Current block to process
    uint8_t             cur_block;
};

void aes_cbc_start(struct aes_cbc_env* env, const uint8_t* key, const uint8_t* iv, const uint8_t* in_message, 
                   uint8_t* out_message,  uint16_t message_len);
bool aes_cbc_continue(struct aes_cbc_env* env, uint8_t* aes_res);

/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */


void ns_aes_cbc(const uint8_t* key, const uint8_t* iv, const uint8_t* in_message, uint8_t* out_message,
                bool cipher, uint16_t message_len, aes_func_result_cb res_cb)
{
    struct aes_cbc_env* aes_cbc;
    uint32_t src_info;
    if(cipher)
    {
        src_info = 0;
    }
    else{
        src_info = DECRYPT_SRC_MARK;
    }
    // Allocate AES CBC environent memory
    aes_cbc = (struct aes_cbc_env*) aes_alloc(sizeof(struct aes_cbc_env),
                                                (aes_func_continue_cb) aes_cbc_continue, res_cb, src_info);

    // start execution AES CBC
    aes_cbc_start(aes_cbc, key, iv, in_message, out_message, message_len);
}

void aes_cbc_start(struct aes_cbc_env* env, const uint8_t* key, const uint8_t* iv, const uint8_t* in_message, 
                   uint8_t* out_message,  uint16_t message_len)
{
    // copy information needed for AES-CBC execution
    env->message     = in_message;
    env->output      = out_message;
    env->message_len = message_len;
    env->iv          = iv;
    ns_ase_swap_array(env->key_cache,key,AES_BLOCK_SIZE); //swap array
    // Step 1: 
    // initialize execution parameters
    // num_blocks = ceil(message_len / block_size)
    env->num_blocks = (env->message_len + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE;
    env->cur_block  = 0;
    // X := iv
    
    // Step 2: handle Blocks M_1    
    const uint8_t *m_i = &(env->message[((env->cur_block) * AES_BLOCK_SIZE)]);
    // start execution of the AES-CBC computation
    
    if(env->aes_env.src_info == DECRYPT_SRC_MARK)
    {
        ns_ase_swap_array(env->val_cache,m_i,AES_BLOCK_SIZE);   //swap array
        // Start AES execution
        ns_aes_start(&(env->aes_env), env->key_cache, env->val_cache);   
    }
    else{
        // Y := X XOR M_1
        // X := AES-128(K,Y)      
        ns_aes_xor_128_swap(env->val_cache, iv, m_i, AES_BLOCK_SIZE);
        
        // Start AES execution
        ns_aes_start(&(env->aes_env), env->key_cache, env->val_cache);        
    }
}

bool aes_cbc_continue(struct aes_cbc_env* env, uint8_t* aes_res)
{
    bool finished = false;
    //  To optimize the AES-CBC algorithm, SW is implemented as follow:
    //
    //  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //  +   Step 1.  n := ceil(len/block_size); (already done in init)      +
    //  +            X := iv;                                       +
    //  +                                                                   +
    //  +   Step 2.                                                         +
    //  +                  Y := X XOR M_0;                                  +
    //  +                  X := AES-128(K,Y);                               +
    //  +                                                                   +
    //  +   Step 3.                                                         +
    //  +            for i = 1 , i < n , i++ do                             +
    //  +                begin                                              +
    //  +                  Y := X XOR M_i;                                  +
    //  +                  X := AES-128(K,Y);                               +
    //  +                  O := O + X;                                      +
    //  +                end                                                +
    //  +                                                                   +
    //  +                                                                   +
    //  +   Step 4.  return O;                                              +
    //  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // O := O + X;  
    uint8_t *o_i = &(env->output[((env->cur_block) * AES_BLOCK_SIZE)]);
    if(env->aes_env.src_info == DECRYPT_SRC_MARK)
    {
        ns_aes_xor_128_msb(o_i, env->iv, aes_res, AES_BLOCK_SIZE);
        env->iv = &(env->message[((env->cur_block) * AES_BLOCK_SIZE)]);
    }
    else{
        ns_ase_swap_array((void*)o_i, aes_res, AES_BLOCK_SIZE);
    }
    
    env->cur_block++;
    // Step 3: handle Blocks M_2 ==> M_(n)
    if(env->cur_block < env->num_blocks)
    {
        const uint8_t *m_i = &(env->message[((env->cur_block) * AES_BLOCK_SIZE)]);
        
        if(env->aes_env.src_info == DECRYPT_SRC_MARK)
        {
            ns_ase_swap_array(env->val_cache,m_i,AES_BLOCK_SIZE);   //swap array
        }
        else{
            // Y := X XOR M_i
            ns_aes_xor_128_swap(env->val_cache, o_i, m_i, AES_BLOCK_SIZE);
        }
        // X := AES-128(K,Y)
        ns_aes_start(&(env->aes_env), env->key_cache, env->val_cache);
    }
    else
    {
        // Step 4 Return result
        // mark AES-CBC over
        finished = true;
    }

    return finished;
}

#endif // (BLE_EMB_PRESENT || BLE_HOST_PRESENT)

/// @} AES_CBC
