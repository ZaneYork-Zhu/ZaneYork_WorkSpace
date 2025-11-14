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
 * @file app_ble.c
 * @author Nations Firmware Team
 * @version v1.0.2
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */
#include <string.h>
#include "n32wb03x.h"
#include "ns_sec.h"
#include "app_ble.h"
#include "app_usart.h"
#include "app_gpio.h"
#include "app_rdtss.h"
#include "app_dis.h"
#include "app_batt.h"
#include "ns_timer.h"
#include "ns_aes.h"
#if (BLE_APP_NS_IUS)
#include "app_ns_ius.h"
#endif //BLE_APP_NS_IUS
#include "app_user_config.h"
/** @addtogroup 
 * @{
 */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t cipher = 1;
#define DATA_BLOCK 5
uint8_t test_input [DATA_BLOCK][16] = {0} ;
uint8_t test_cipher[DATA_BLOCK][16] = {0} ;
uint8_t test_plain [DATA_BLOCK][16] = {0} ;

#define CCM_MIC_LEN  16
uint8_t key[16] = {"0123456789abcdef"};
uint8_t nonce[13] = {"0000111122223"};
uint8_t add_auth_data[2] = {"12"};
uint8_t iv[16]  = {"1111222233334444"};

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void app_ble_connected(void);
void app_ble_disconnected(void);

#define AES_LOG NS_LOG_DEBUG  // uart ouput all AES test data will block BLE pairing
/**
 * @brief  output data in HEX format
 * @param    
 * @return 
 * @note   Note
 */
void print_text(uint8_t *data, uint32_t len)
{
    for(int i=0;i<len;i++){
        if(((i%0x10) == 0) && (i >0)) AES_LOG("\r\n");
        AES_LOG("%02X ",data[i]);
    }
    AES_LOG("\r\n");
}



void app_aes_ccm_cb(bool mic_error, uint32_t src_info)
{
    
    if(cipher == 1)
    {
        AES_LOG("ccm plain: \r\n");
        print_text(test_plain[0],16*(DATA_BLOCK-1));
        
        if(mic_error)
        {
            NS_LOG_INFO("Tag(mic) authentication fail!\r\n");
        }
        else{
            NS_LOG_INFO("Tag(mic) authentication pass!\r\n");
        }        
        if(memcmp(test_input,test_plain,16*(DATA_BLOCK-1)) == 0)
        {
            NS_LOG_INFO("CCM data decrypt pass!\r\n");
        }
        else{
            NS_LOG_INFO("CCM data decrypt fail!\r\n");
        }
        

    }
    else{
        AES_LOG("ccm cipher: \r\n");
        print_text(test_cipher[0],16*(DATA_BLOCK-1));
        
        AES_LOG("Tag(mic): \r\n");
        print_text(test_cipher[DATA_BLOCK-1],CCM_MIC_LEN);
        
        #ifdef USE_ERROR_MIC_TO_DECIPHER
        // change mic to decipher, then the mic_error set flag when decipher
        test_cipher[DATA_BLOCK-1][0] = 0; 
        #endif
    }
}

void app_aes_ccm_demo(void)
{
    static uint8_t key_lsb[AES_BLOCK_SIZE];
    if(cipher)
    {
        cipher = 0;
        uint8_t *p = test_input[0];
        for(uint8_t i=0;i<16*DATA_BLOCK;i++)
        {
            p[i] = rand();
        }
        NS_LOG_INFO("\r\n\r\nAES CCM Test Demo\r\n");
        AES_LOG("key: ");
        print_text(key,16);
        AES_LOG("nonce: ");
        print_text(nonce,13);
        AES_LOG("add_auth_data: ");
        print_text(add_auth_data,2);
        
        AES_LOG("input_text: \r\n");
        print_text(test_input[0],16*(DATA_BLOCK-1));
        
        ns_ase_swap_array(key_lsb,key,AES_BLOCK_SIZE);
        ns_aes_ccm(key_lsb, nonce, test_input[0], test_cipher[0], 16*(DATA_BLOCK-1), CCM_MIC_LEN , true, add_auth_data, 2, app_aes_ccm_cb, 0);
        
        //do the decrypt 2ms later
        ns_timer_create(5,app_aes_ccm_demo);
    }
    else{
        ns_ase_swap_array(key_lsb,key,AES_BLOCK_SIZE); 
        ns_aes_ccm(key_lsb, nonce, test_cipher[0], test_plain[0], 16*(DATA_BLOCK-1), CCM_MIC_LEN , false, add_auth_data, 2, app_aes_ccm_cb, 0);
        cipher = 1;
        //repeat cbc encrypt test 1000ms later
//        ns_timer_create(1000,app_aes_ccm_demo);
    }
}



void app_aes_cbc_cb(uint8_t status, const uint8_t* aes_res, uint32_t src_info)
{
    if(src_info == DECRYPT_SRC_MARK)
    {
        AES_LOG("cbc_plain: \r\n");
        print_text(test_plain[0],16*DATA_BLOCK);
        if(memcmp(test_input,test_plain,16*DATA_BLOCK) == 0)
        {
            NS_LOG_INFO("CBC encrypt and decrypt pass!\r\n");
        }
        else{
            NS_LOG_INFO("CBC encrypt and decrypt fail!\r\n");
        }
    }
    else{
        AES_LOG("cbc_cipher: \r\n");
        print_text(test_cipher[0],16*DATA_BLOCK);
    }
}
    

void app_aes_cbc_demo(void)
{
    if(cipher)
    {
        cipher = 0;
        uint8_t *p = test_input[0];
        for(uint8_t i=0;i<16*DATA_BLOCK;i++)
        {
            p[i] = rand();
        }
        NS_LOG_INFO("\r\n\r\nAES CBC Test Demo\r\n");
        AES_LOG("key: ");
        print_text(key,16);
        AES_LOG("iv: ");
        print_text(iv,16);
        
        AES_LOG("input_text: \r\n");
        print_text(test_input[0],16*DATA_BLOCK);
        
        ns_aes_cbc(key, iv, test_input[0],test_cipher[0],true, 16*DATA_BLOCK, app_aes_cbc_cb);
        
        //do the decrypt 2ms later
        ns_timer_create(5,app_aes_cbc_demo);
    }
    else{
        ns_aes_cbc(key, iv, test_cipher[0],test_plain[0],false, 16*DATA_BLOCK, app_aes_cbc_cb);
        cipher = 1;
        //repeat cbc encrypt test 1000ms later
        ns_timer_create(1000,app_aes_cbc_demo);
    }
}



void app_aes_result_cb(uint8_t status, const uint8_t* aes_res, uint32_t src_info)
{
    if(src_info == DECRYPT_SRC_MARK ) //decrypt
    {
        memcpy(test_plain[0],aes_res,16);
        AES_LOG("plaintext : ");
        print_text(test_plain[0],16);   
        if(memcmp(test_input[0],test_plain[0],16) == 0)
        {
            NS_LOG_INFO("AES encrypt and decrypt pass!\r\n");
        }
        else{
            NS_LOG_INFO("AES encrypt and decrypt fail!\r\n");
        }
    }
    else{
        memcpy(test_cipher[0],aes_res,16);
        AES_LOG("ciphertext: ");
        print_text(test_cipher[0],16);   
    }
}

/**
 * @brief  aes example fucntion
 * @param   
 * @return 
 * @note   Note
 */
void ase_test_demo(void)
{
    if(cipher)
    {
        NS_LOG_INFO("\r\n\r\nAES Test Demo\r\n");
        AES_LOG("128bit key: ");
        print_text(key,16);
        uint8_t *p_in = test_input[0];
        for(uint8_t i=0;i<16;i++)
        {
            p_in[i] = rand();
        }
        AES_LOG("randString: ");
        print_text(p_in,16);
        
        ns_aes_ciphering(key,p_in,app_aes_result_cb);
        cipher = 0;
        
         //do the decrypt 2ms later
        ns_timer_create(2,ase_test_demo);
    }
    else{
        cipher = 1;
        
        ns_aes_deciphering(key,test_cipher[0], app_aes_result_cb);
        //repeat
//        ns_timer_create(500,ase_test_demo);
    }
}


/**
 * @brief  user message handler
 * @param  
 * @return 
 * @note   
 */
void app_user_msg_handler(ke_msg_id_t const msgid, void const *p_param)
{
    
    switch (msgid)
    {
    	case APP_CUSTS_TEST_EVT:
            app_usart_tx_process();
    		break;
//    	case :
//    		break;
    	default:
    		break;
    }
}

/**
 * @brief  ble message handler
 * @param  
 * @return 
 * @note   
 */
void app_ble_msg_handler(struct ble_msg_t const *p_ble_msg)
{
    switch (p_ble_msg->msg_id)
    {
        case APP_BLE_OS_READY:
            NS_LOG_INFO("APP_BLE_OS_READY\r\n");
            ns_timer_create(10,ase_test_demo);
            ns_timer_create(1000,app_aes_cbc_demo);
            ns_timer_create(2000,app_aes_ccm_demo);
            break;
        case APP_BLE_GAP_CONNECTED:
            app_ble_connected();
            break;
        case APP_BLE_GAP_DISCONNECTED:
            app_ble_disconnected();
            break;

        default:
            break;
    }
}

/**
 * @brief  advertising message handler
 * @param  
 * @return 
 * @note   
 */
void app_ble_adv_msg_handler(enum app_adv_mode adv_mode)
{
    switch (adv_mode)
    {
        case APP_ADV_MODE_DIRECTED:
            
            break;
        case APP_ADV_MODE_FAST:
            
            break;
        case APP_ADV_MODE_SLOW:
            
            break;
        default:
            break;
    }

}
    

/**
 * @brief  ble GAP initialization
 * @param  
 * @return 
 * @note   
 */
void app_ble_gap_params_init(void)
{
    struct ns_gap_params_t dev_info = {0};
    uint8_t *p_mac = SystemGetMacAddr();
    //get UUID from trim stored
    if(p_mac != NULL)
    {
        //set the uuid as mac address
        memcpy(dev_info.mac_addr.addr, p_mac , BD_ADDR_LEN); 
    }
    else{
        memcpy(dev_info.mac_addr.addr, "\x01\x02\x03\x04\x05\x06" , BD_ADDR_LEN);
    }
    

    /* init params*/
    dev_info.mac_addr_type = GAPM_STATIC_ADDR;
    dev_info.appearance = 0;
    dev_info.dev_role = GAP_ROLE_PERIPHERAL;
    
    dev_info.dev_name_len = sizeof(CUSTOM_DEVICE_NAME)-1;
    memcpy(dev_info.dev_name, CUSTOM_DEVICE_NAME, dev_info.dev_name_len); 
   
    dev_info.dev_conn_param.intv_min = MSECS_TO_UNIT(MIN_CONN_INTERVAL,MSECS_UNIT_1_25_MS);
    dev_info.dev_conn_param.intv_max = MSECS_TO_UNIT(MAX_CONN_INTERVAL,MSECS_UNIT_1_25_MS);
    dev_info.dev_conn_param.latency  = SLAVE_LATENCY;
    dev_info.dev_conn_param.time_out = MSECS_TO_UNIT(CONN_SUP_TIMEOUT,MSECS_UNIT_10_MS);
    dev_info.conn_param_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    
    ns_ble_gap_init(&dev_info);
    
}


/**
 * @brief  ble advertising initialization
 * @param  
 * @return 
 * @note   
 */
void app_ble_adv_init(void)
{
    struct ns_adv_params_t user_adv = {0};
    
    //init advertising data 
    user_adv.adv_data_len = CUSTOM_USER_ADVERTISE_DATA_LEN;
    memcpy(user_adv.adv_data,CUSTOM_USER_ADVERTISE_DATA,CUSTOM_USER_ADVERTISE_DATA_LEN);
    user_adv.scan_rsp_data_len = CUSTOM_USER_ADV_SCNRSP_DATA_LEN;
    memcpy(user_adv.scan_rsp_data,CUSTOM_USER_ADV_SCNRSP_DATA,CUSTOM_USER_ADV_SCNRSP_DATA_LEN);
    
    user_adv.attach_appearance  = false;
    user_adv.attach_name        = true;
    user_adv.ex_adv_enable      = false;
    user_adv.adv_phy            = PHY_1MBPS_VALUE;
    
    //init advertising params
    user_adv.directed_adv.enable = false;

    user_adv.fast_adv.enable    = true;
    user_adv.fast_adv.duration  = CUSTOM_ADV_FAST_DURATION;
    user_adv.fast_adv.adv_intv  = CUSTOM_ADV_FAST_INTERVAL;
    
    user_adv.slow_adv.enable    = true;  
    user_adv.slow_adv.duration  = CUSTOM_ADV_SLOW_DURATION;
    user_adv.slow_adv.adv_intv  = CUSTOM_ADV_SLOW_INTERVAL;
    
    user_adv.ble_adv_msg_handler = app_ble_adv_msg_handler;
    
    ns_ble_adv_init(&user_adv);
    

}

void app_ble_sec_init(void)
{
    struct ns_sec_init_t sec_init = {0};
    
    sec_init.rand_pin_enable = false;
    sec_init.pin_code = 123456;
    
    sec_init.pairing_feat.auth      = ( SEC_PARAM_BOND | (SEC_PARAM_MITM<<2) | (SEC_PARAM_LESC<<3) | (SEC_PARAM_KEYPRESS<<4) );
    sec_init.pairing_feat.iocap     = SEC_PARAM_IO_CAPABILITIES;
    sec_init.pairing_feat.key_size  = SEC_PARAM_KEY_SIZE;
    sec_init.pairing_feat.oob       = SEC_PARAM_OOB;
    sec_init.pairing_feat.ikey_dist = SEC_PARAM_IKEY;
    sec_init.pairing_feat.rkey_dist = SEC_PARAM_RKEY;
    sec_init.pairing_feat.sec_req   = SEC_PARAM_SEC_MODE_LEVEL;
    
    sec_init.bond_enable            = BOND_STORE_ENABLE;
    sec_init.bond_db_addr           = BOND_DATA_BASE_ADDR;
    sec_init.bond_max_peer          = MAX_BOND_PEER;
    sec_init.bond_sync_delay        = 2000;
    
    sec_init.ns_sec_msg_handler     = NULL;
    
    ns_sec_init(&sec_init);
}

void app_ble_prf_init(void)
{
    #if (BLE_APP_DIS)
    //add device informaiton server
    ns_ble_add_prf_func_register(app_dis_add_dis);    
    #endif //BLE_APP_DIS
    #if (BLE_APP_BATT)
    //add battery level server
    ns_ble_add_prf_func_register(app_batt_add_bas);
    #endif //BLE_APP_BATT 
    #if (BLE_APP_NS_IUS)
    ns_ble_add_prf_func_register(app_ns_ius_add_ns_ius);
    #endif //BLE_APP_NS_IUS 
    
    //add raw data transmit server(rdts)
    ns_ble_add_prf_func_register(app_rdtss_add_rdts);
    

    
}


/**
 * @brief  ble initialization
 * @param  
 * @return 
 * @note   
 */
void app_ble_init(void)
{
    struct ns_stack_cfg_t app_handler = {0};
    app_handler.ble_msg_handler  = app_ble_msg_handler;
    app_handler.user_msg_handler = app_user_msg_handler;
    app_handler.lsc_cfg          = BLE_LSC_LSI_32000HZ;
    //initialization ble stack
    ns_ble_stack_init(&app_handler);
    
    app_ble_gap_params_init();
    app_ble_sec_init();
    app_ble_adv_init();
    app_ble_prf_init();
    //start adv
    ns_ble_adv_start();
}

/**
 * @brief  ble connected
 * @param  
 * @return 
 * @note   
 */
void app_ble_connected(void)
{
    //enable usart receive
    app_usart_dma_enable(ENABLE);
    LedOn(LED2_PORT,LED2_PIN);   
    
    #if (BLE_APP_BATT)
    // Enable Battery Service
    app_batt_enable_prf(app_env.conidx);
    #endif //(BLE_APP_BATT)
}

/**
 * @brief  ble disconnected
 * @param  
 * @return 
 * @note   
 */
void app_ble_disconnected(void)
{
    // Restart Advertising
    ns_ble_adv_start();
    
    //disable usart receive
    app_usart_dma_enable(DISABLE);
    LedOff(LED2_PORT,LED2_PIN);
}





/**
 * @}
 */
