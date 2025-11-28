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
 * @file ns_sec.c
 * @author Nations Firmware Team
 * @version v1.0.4
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */


/** @addtogroup APP
 * @{
 */

/* Includes ------------------------------------------------------------------*/

#include "rwip_config.h"

#if (BLE_APP_SEC)
#include "ns_ble.h"        // Application API Definition
#include "ns_ble_task.h"
#include "ns_sec.h"        // Application Security API Definition


#if (DISPLAY_SUPPORT)
#include "app_display.h"    // Display Application Definitions
#endif //(DISPLAY_SUPPORT)

#if (NVDS_SUPPORT)
#include "nvds.h"           // NVDS API Definitions
#endif //(NVDS_SUPPORT)

#if (BLE_APP_AM0)
#include "app_am0.h"
#endif //(BLE_APP_AM0)

#include "ahi_task.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define BOND_SPACE_VALID_FLAG           0x1234
#define BOND_STORE_LATENCY              500 //  unit 20ms
/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct app_sec_bond_data_env_tag app_sec_bond_data;
/// Application Security Environment Structure
struct app_sec_env_tag app_sec_env;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
extern __INLINE uint32_t co_rand_word(void);


/**
 * @brief  ns bond database get size
 * @param  
 * @return 
 * @note
 */
static uint8_t ns_bond_db_get_size(void)
{
    uint8_t bond_num = 0;
    uint16_t bond_space_valid = 0;
    struct local_device_bond_data_tag* p_strored = (void*)app_sec_env.sec_init.bond_db_addr;
    
    //NS_LOG_DEBUG("%s\r\n",__func__);
    
    bond_space_valid = p_strored->valid_flag;
    bond_num = p_strored->num;
    
    if(bond_space_valid == BOND_SPACE_VALID_FLAG)
    {
        if(bond_num > app_sec_env.sec_init.bond_max_peer)
        {
            bond_num = 0;
            
            // Erase full sector
            Qflash_Erase_Sector(app_sec_env.sec_init.bond_db_addr);
        }
    }
    else
    {
        bond_num = 0;
    }
    
    return bond_num;
}


/**
 * @brief  bond database init
 */
static void ns_bond_db_init(void)
{
    uint8_t peer_num = 0;
    Qflash_Init();
    NS_LOG_DEBUG("%s\r\n",__func__);
    peer_num = ns_bond_db_get_size();
    if(app_sec_env.sec_init.ns_sec_msg_handler)
    {
        struct sec_msg_t sec_msg = {NS_SEC_NULL_MSG,NULL};
        sec_msg.msg_id = NS_SEC_BOND_STATE;
        sec_msg.msg.peer_num = &peer_num;
        app_sec_env.sec_init.ns_sec_msg_handler(&sec_msg); 
    }
    
    if(peer_num != 0)
    {
        app_sec_env.bonded = true;
    }
    else
    {
        app_sec_env.bonded = false;
    }
}

/**
 * @brief  ns bond database add entry
 * @param  
 * @return 
 * @note   
 */
static void ns_bond_db_add_entry(struct app_sec_bond_data_env_tag * bond_data_param)
{
    // Load bond data from flash
    struct local_device_bond_data_tag local_device_bond_data = {0};
    struct local_device_bond_data_tag* p_strored = (void*)app_sec_env.sec_init.bond_db_addr;
    if(!app_sec_env.sec_init.bond_enable )
    {
        return;
    }
    
    local_device_bond_data.num = ns_bond_db_get_size();
    
    // Update bond data
    if(local_device_bond_data.num < app_sec_env.sec_init.bond_max_peer)
    {
        uint32_t load_size = sizeof(struct app_sec_bond_data_env_tag) * local_device_bond_data.num;
        memcpy(&local_device_bond_data.single_peer_bond_data, &(p_strored->single_peer_bond_data[0]),load_size);
        local_device_bond_data.num ++;
    }
    else
    {
        uint32_t load_size = sizeof(struct app_sec_bond_data_env_tag) * (local_device_bond_data.num-1);
        memcpy(&local_device_bond_data.single_peer_bond_data,&(p_strored->single_peer_bond_data[1]), load_size);
    }

    memcpy(&local_device_bond_data.single_peer_bond_data[local_device_bond_data.num-1], bond_data_param,
            sizeof(struct app_sec_bond_data_env_tag) );
    
    local_device_bond_data.valid_flag = BOND_SPACE_VALID_FLAG;
    
    // Erase full sector
    Qflash_Erase_Sector(app_sec_env.sec_init.bond_db_addr);
    
    // Re-write data
    Qflash_Write(app_sec_env.sec_init.bond_db_addr, (uint8_t*)(&local_device_bond_data), sizeof(struct local_device_bond_data_tag) );

}

/**
 * @brief  ns bond database load
 * @param  
 * @return 
 * @note   
 */
static void ns_bond_db_load(uint8_t index, struct app_sec_bond_data_env_tag * single_bond_data)
{
    uint32_t length = sizeof(struct app_sec_bond_data_env_tag);
    struct local_device_bond_data_tag* p_strored = (void*)app_sec_env.sec_init.bond_db_addr;
    //NS_LOG_DEBUG("%s\r\n",__func__);
    
    memcpy(single_bond_data, &(p_strored->single_peer_bond_data[index]), length);
}


/**
 * @brief  return the  address of last bonded device
 * @param  
 * @return 
 * @note   
 */
void ns_bond_last_bonded_addr(struct gap_bdaddr *p_addr)
{
    uint8_t peer_num = 0;
    struct app_sec_bond_data_env_tag  bond_data = {0};
    peer_num = ns_bond_db_get_size();
    ns_bond_db_load( peer_num-1, &bond_data);
    p_addr->addr_type = bond_data.peer_addr_type;
    memcpy(p_addr->addr.addr,bond_data.peer_addr.addr,GAP_BD_ADDR_LEN);
}

/**
 * @brief  return the peer identy address of last bonded device
 * @param  
 * @return 
 * @note   
 */
void ns_bond_last_bonded_peer_id(struct gap_bdaddr *p_addr)
{
    uint8_t peer_num = 0;
    struct app_sec_bond_data_env_tag  bond_data = {0};
    peer_num = ns_bond_db_get_size();
    ns_bond_db_load( peer_num-1, &bond_data);
    p_addr->addr_type = bond_data.irk.addr.addr_type;
    memcpy(p_addr->addr.addr,bond_data.irk.addr.addr.addr,GAP_BD_ADDR_LEN);
}

/**
 * @brief  return the ral info of last bonded device
 * @param  
 * @return 
 * @note   
 */
void ns_bond_last_bonded_ral_info(struct gap_ral_dev_info *ral_list)
{
    uint8_t peer_num = 0;
    struct app_sec_bond_data_env_tag  bond_data = {0};
    NS_LOG_DEBUG("%s\r\n", __func__);
    peer_num = ns_bond_db_get_size();
    ns_bond_db_load( peer_num-1, &bond_data);
    memcpy(ral_list->addr.addr.addr,bond_data.irk.addr.addr.addr,GAP_BD_ADDR_LEN);
    ral_list->addr.addr_type = bond_data.irk.addr.addr_type;
    memcpy(ral_list->peer_irk,bond_data.irk.irk.key,GAP_KEY_LEN);
    if (gap_env.mac_addr_type == GAPM_GEN_RSLV_ADDR)
    {
        memcpy(ral_list->local_irk,&app_env.loc_irk[0],GAP_KEY_LEN);
    }
    else
    {
        memset(ral_list->local_irk, 0, GAP_KEY_LEN);
    }
}

/**
 * @brief  resolv addr and get the irk for resolvable private random address
 * @param  
 * @return 
 * @note   
 */
void ns_bond_resolv_addr_start(struct bd_addr* addr, uint8_t idx)
{
    uint8_t peer_num = ns_bond_db_get_size(); 
    NS_LOG_DEBUG("%s, idx:%d, num:%d\r\n", __func__, idx ,peer_num);
    
    if((idx > 0) && (idx<= peer_num))
    {
        // Prepare the GAPM_RESOLV_ADDR_CMD message
        struct gapm_resolv_addr_cmd *p_cmd = KE_MSG_ALLOC_DYN(GAPM_RESOLV_ADDR_CMD,
                                                         TASK_GAPM, TASK_APP,
                                                         gapm_resolv_addr_cmd,KEY_LEN);
        // Set operation code
        p_cmd->operation  = GAPM_RESOLV_ADDR;
        p_cmd->nb_key  = 1;

        memcpy(p_cmd->addr.addr, addr, sizeof(bd_addr_t));
        
        struct app_sec_bond_data_env_tag  bond_data = {0};
        ns_bond_db_load( idx-1, &bond_data);
        memcpy(&(p_cmd->irk[0]), &(bond_data.irk.irk), KEY_LEN);
        
        // Send the message
        ke_msg_send(p_cmd);
        //get resond at GAPM_ADDR_SOLVED_IND
    }
}

/**
 * @brief  get the irk of this address
 * @param  
 * @return 
 * @note   
 */
void ns_bond_search_addr_irk(struct bd_addr* addr,uint8_t addr_type)
{
    struct app_sec_bond_data_env_tag  bond_data = {0};
    uint8_t peer_num = ns_bond_db_get_size();  
    
    NS_LOG_DEBUG("%s,type:%d,bond num:%d\r\n", __func__,addr_type,peer_num);
//    NS_LOG_DEBUG("search_addr:%02X %02X %02X %02X %02X %02X \r\n", addr->addr[0], 
//                addr->addr[1], addr->addr[2], addr->addr[3], addr->addr[4], addr->addr[5]);

    if(addr_type == GAPM_GEN_RSLV_ADDR)
    {
        //start resolv addr
        app_env.next_svc = peer_num;
        ns_bond_resolv_addr_start(addr,app_env.next_svc--);
    }
    else{
        // not need to resolv
        for(uint8_t i = peer_num; i > 0; i--)
        {
            ns_bond_db_load( i-1, &bond_data);
            if(!memcmp(addr->addr,bond_data.irk.addr.addr.addr,BD_ADDR_LEN))
            {
                //found
                memcpy(&app_env.peer_irk.key,&bond_data.irk.irk.key,GAP_KEY_LEN);
                NS_LOG_DEBUG("IRK found, static addr\r\n");
                break;
            }
        }
    }
}


/**
 * @brief erase all bond database
 */
void ns_sec_bond_db_erase_all(void)
{
    // Erase full sector
    Qflash_Erase_Sector(app_sec_env.sec_init.bond_db_addr);
    app_sec_env.bonded = false;
}


/**
 * @brief  security init
 * @param  
 * @return 
 * @note   
 */
void ns_sec_init(struct ns_sec_init_t const* init)
{
    memcpy(&app_sec_env.sec_init,init,sizeof(struct ns_sec_init_t));

    // Bond Init
    if(app_sec_env.sec_init.bond_enable)
    {
        ns_bond_db_init();
    }
}


bool ns_sec_get_bond_status(void)
{
    return app_sec_env.bonded;
}

uint8_t ns_sec_get_iocap(void)
{
    return app_sec_env.sec_init.pairing_feat.iocap;
}

void ns_sec_send_security_req(uint8_t conidx)
{
    // Send security request
    struct gapc_security_cmd *cmd = KE_MSG_ALLOC(GAPC_SECURITY_CMD,
                                                 KE_BUILD_ID(TASK_GAPC, conidx), TASK_APP,
                                                 gapc_security_cmd);
    NS_LOG_DEBUG("%s\r\n",__func__);
    cmd->operation = GAPC_SECURITY_REQ;

    cmd->auth = app_sec_env.sec_init.pairing_feat.auth;
    
    // Send the message
    ke_msg_send(cmd);
}

/**
 * @brief  start bond by central
 */
void ns_sec_send_bond_start(uint8_t conidx)
{
    // Send security request
    struct gapc_bond_cmd *cmd = KE_MSG_ALLOC(GAPC_BOND_CMD,
                                             KE_BUILD_ID(TASK_GAPC, conidx), TASK_APP,
                                             gapc_bond_cmd);
    NS_LOG_DEBUG("%s\r\n",__func__);
    cmd->operation = GAPC_BOND;

    cmd->pairing.iocap = app_sec_env.sec_init.pairing_feat.iocap ;
    cmd->pairing.oob = app_sec_env.sec_init.pairing_feat.oob ;
    cmd->pairing.auth = app_sec_env.sec_init.pairing_feat.auth ;
    cmd->pairing.key_size = app_sec_env.sec_init.pairing_feat.key_size ;
    cmd->pairing.ikey_dist = app_sec_env.sec_init.pairing_feat.ikey_dist ;
    cmd->pairing.rkey_dist = app_sec_env.sec_init.pairing_feat.rkey_dist ;
    cmd->pairing.sec_req = app_sec_env.sec_init.pairing_feat.sec_req ;

    // Send the message
    ke_msg_send(cmd);
}

/**
 * @brief  start encrypt request by central
 */
void ns_sec_send_encrypt_req(uint8_t conidx)
{
    // Send security request
    struct gapc_encrypt_cmd *cmd = KE_MSG_ALLOC(GAPC_ENCRYPT_CMD,
                                                 KE_BUILD_ID(TASK_GAPC, conidx), TASK_APP,
                                                 gapc_encrypt_cmd);
    NS_LOG_DEBUG("%s\r\n",__func__);
    cmd->operation = GAPC_ENCRYPT;
    if(app_sec_env.sec_init.bond_enable)
    {
        uint8_t peer_num = 0;
        struct app_sec_bond_data_env_tag  bond_data = {0};
        peer_num = ns_bond_db_get_size();

        for(uint8_t i = peer_num; i > 0; i--)
        {
            ns_bond_db_load( i-1, &bond_data);
            if( !memcmp(&bond_data.peer_addr, &app_env.peer_addr, GAP_BD_ADDR_LEN))
            {
                NS_LOG_DEBUG("Found bonded device\r\n"); 
                cmd->ltk.ediv =  bond_data.ediv;
                cmd->ltk.key_size =  bond_data.key_size;
                memcpy(cmd->ltk.ltk.key, bond_data.ltk.key, sizeof(struct gap_sec_key));
                memcpy(cmd->ltk.randnb.nb, bond_data.rand_nb.nb, sizeof(struct rand_nb));
                break;
            }
        }
    }
    else
    {
        cmd->ltk.ediv =  app_sec_bond_data.ediv;
        cmd->ltk.key_size =  app_sec_bond_data.key_size;
        memcpy(cmd->ltk.ltk.key, app_sec_bond_data.ltk.key, sizeof(struct gap_sec_key));
        memcpy(cmd->ltk.randnb.nb, app_sec_bond_data.rand_nb.nb, sizeof(struct rand_nb));
    }
    // Send the message
    ke_msg_send(cmd);
}

/**
 * @brief  respond pin code after NS_SEC_PINCODE_ENTER message
 * @param  
 * @return 
 * @note   
 */
void ns_sec_pincode_respond(uint8_t conidx, uint32_t pincode)
{
    NS_LOG_DEBUG("%s:pin_code: 0x%x\r\n",__func__,pincode);
    // Prepare the GAPC_BOND_CFM message
    struct gapc_bond_cfm *cfm = KE_MSG_ALLOC(GAPC_BOND_CFM,
                                             KE_BUILD_ID(TASK_GAPC, conidx), TASK_APP,
                                             gapc_bond_cfm);
    cfm->accept = true;
    cfm->request = GAPC_TK_EXCH;
    memset(cfm->data.tk.key, 0, KEY_LEN);

    cfm->data.tk.key[0] = (uint8_t)((pincode & 0x000000FF) >>  0);
    cfm->data.tk.key[1] = (uint8_t)((pincode & 0x0000FF00) >>  8);
    cfm->data.tk.key[2] = (uint8_t)((pincode & 0x00FF0000) >> 16);
    cfm->data.tk.key[3] = (uint8_t)((pincode & 0xFF000000) >> 24);   

    // Send the message
    ke_msg_send(cfm);
}

static int gapc_bond_req_ind_handler(ke_msg_id_t const msgid,
                                     struct gapc_bond_req_ind const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    NS_LOG_DEBUG("%s\r\n",__func__);
    NS_LOG_DEBUG("case: %02x\r\n",param->request);

    // Prepare the GAPC_BOND_CFM message
    struct gapc_bond_cfm *cfm = KE_MSG_ALLOC(GAPC_BOND_CFM,
                                             src_id, TASK_APP,
                                             gapc_bond_cfm);
    cfm->accept = true;

    switch (param->request)
    {
        case (GAPC_PAIRING_REQ):
        {
            cfm->request = GAPC_PAIRING_RSP;
           
            cfm->accept  = true;
            
            cfm->data.pairing_feat.auth      = app_sec_env.sec_init.pairing_feat.auth;
            cfm->data.pairing_feat.iocap     = app_sec_env.sec_init.pairing_feat.iocap;
            cfm->data.pairing_feat.key_size  = app_sec_env.sec_init.pairing_feat.key_size;
            cfm->data.pairing_feat.oob       = app_sec_env.sec_init.pairing_feat.oob;
            cfm->data.pairing_feat.ikey_dist = app_sec_env.sec_init.pairing_feat.ikey_dist;
            cfm->data.pairing_feat.rkey_dist = app_sec_env.sec_init.pairing_feat.rkey_dist;
            cfm->data.pairing_feat.sec_req   = app_sec_env.sec_init.pairing_feat.sec_req;
            
            if(app_sec_env.sec_init.ns_sec_msg_handler)
            {
                struct sec_msg_t sec_msg = {NS_SEC_NULL_MSG,NULL};
                sec_msg.msg_id = NS_SEC_PAIRING_RSP;
                sec_msg.msg.cfm = cfm;
                app_sec_env.sec_init.ns_sec_msg_handler(&sec_msg); 
            }

        } break;

        case (GAPC_LTK_EXCH):
        {
            // Counter
            uint8_t counter;
            rwip_time_t current_time;    
            
            cfm->request = GAPC_LTK_EXCH;    
            cfm->accept  = true;

            // Get current time
            current_time = rwip_time_get();
            
            // Generate all the values
            cfm->data.ltk.ediv = (uint16_t)(co_rand_word() + current_time.hs);

            for (counter = 0; counter < RAND_NB_LEN; counter++)
            {
                cfm->data.ltk.ltk.key[counter]    = (uint8_t)(co_rand_word() + current_time.hs);
                cfm->data.ltk.randnb.nb[counter]  = (uint8_t)(co_rand_word() + current_time.hus);
            }

            for (counter = RAND_NB_LEN; counter < KEY_LEN; counter++)
            {
                cfm->data.ltk.ltk.key[counter]    = (uint8_t)(co_rand_word() + current_time.hus + current_time.hs);
            }
            cfm->data.ltk.key_size = SEC_PARAM_KEY_SIZE;
            
            if (app_env.conn_env[app_env.conidx].role == ROLE_SLAVE)
            {
                memcpy(&app_sec_bond_data.ltk.key, &cfm->data.ltk.ltk.key, sizeof(struct gapc_ltk));
                memcpy(&app_sec_bond_data.rand_nb, &cfm->data.ltk.randnb, sizeof(rand_nb_t));
                app_sec_bond_data.ediv      = cfm->data.ltk.ediv;
                app_sec_bond_data.key_size  = cfm->data.ltk.key_size;
            }
            
        } break;


        case (GAPC_IRK_EXCH):
        {
            cfm->request = GAPC_IRK_EXCH;  
            
        } break;


        case (GAPC_TK_EXCH):
        {
            cfm->request = GAPC_TK_EXCH;
            if(param->data.tk_type == GAP_TK_DISPLAY || param->data.tk_type == GAP_TK_OOB)
            {
                uint32_t pin_code;
                cfm->accept  = true;
                
                if(app_sec_env.sec_init.rand_pin_enable)
                {
                    // Get current time
                    rwip_time_t current_time = rwip_time_get();
                    // Generate a PIN Code- (Between 100000 and 999999)
                    pin_code = ((100000 + (co_rand_word()%900000) + current_time.hs) % 999999);
                }
                else
                {
                     pin_code = app_sec_env.sec_init.pin_code; //fix pin code
                }
                if(app_sec_env.sec_init.ns_sec_msg_handler)
                {
                    struct sec_msg_t sec_msg = {NS_SEC_NULL_MSG,NULL};
                    sec_msg.msg_id = NS_SEC_PINCODE_DISPLAY;
                    sec_msg.msg.pincode = &pin_code;
                    app_sec_env.sec_init.ns_sec_msg_handler(&sec_msg); 
                }
                // Set the TK value
                memset(cfm->data.tk.key, 0, KEY_LEN);

                cfm->data.tk.key[0] = (uint8_t)((pin_code & 0x000000FF) >>  0);
                cfm->data.tk.key[1] = (uint8_t)((pin_code & 0x0000FF00) >>  8);
                cfm->data.tk.key[2] = (uint8_t)((pin_code & 0x00FF0000) >> 16);
                cfm->data.tk.key[3] = (uint8_t)((pin_code & 0xFF000000) >> 24);
                
            }
            else if (param->data.tk_type == GAP_TK_KEY_ENTRY)
            {
                if(app_sec_env.sec_init.ns_sec_msg_handler)
                {
                    struct sec_msg_t sec_msg = {NS_SEC_NULL_MSG,NULL} ;
                    sec_msg.msg_id = NS_SEC_PINCODE_ENTER;
                    app_sec_env.sec_init.ns_sec_msg_handler(&sec_msg); 
                }
                
                //free the msg add return without repsond
                //user code have to respond the pincode with function ns_sec_pincode_respond
                KE_MSG_FREE(cfm);
                return (KE_MSG_CONSUMED);
            }
            else
            {
                ASSERT_ERR(0);
            }
            
        } break;
        
        case (GAPC_CSRK_EXCH):
        {
            cfm->request = GAPC_CSRK_EXCH;
        
            if(gap_env.mac_addr_type == GAPM_GEN_RSLV_ADDR)
            {
                memcpy(&cfm->data.irk.irk.key, &app_env.loc_irk[0], GAP_KEY_LEN);
            }
            else
            {
                memset(&cfm->data.irk.irk.key, 0x00, KEY_LEN);
            }
            memcpy(&cfm->data.irk.addr.addr, &gap_env.mac_addr, BD_ADDR_LEN);
            cfm->data.irk.addr.addr_type = 0;   

            
        } break;
            
                
        default:
        {
            ASSERT_ERR(0);
        } break;
    }

    // Send the message
    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}

static int gapc_bond_ind_handler(ke_msg_id_t const msgid,
                                 struct gapc_bond_ind const *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id)
{

    NS_LOG_DEBUG("%s\r\n",__func__);
    NS_LOG_DEBUG("case: %x\r\n",param->info);
    switch (param->info)
    {
        case (GAPC_PAIRING_SUCCEED):
        {
            NS_LOG_DEBUG("GAPC_PAIRING_SUCCEED\r\n");
            app_sec_env.bonded = true;
    
            app_sec_bond_data.auth = param->data.pairing.level;
    
            if(param->data.pairing.level & GAP_AUTH_BOND)
            {
                memcpy(&app_sec_bond_data.peer_addr.addr, app_env.peer_addr.addr, BD_ADDR_LEN);
                app_sec_bond_data.peer_addr_type = app_env.peer_addr_type;
            }
            if(app_sec_env.sec_init.bond_sync_delay > 0)
            {
                app_sec_env.store_latency = BOND_STORE_LATENCY;
                ke_timer_set(APP_BOND_STORE_EVT, TASK_APP, app_sec_env.sec_init.bond_sync_delay);
            }
            if(app_sec_env.sec_init.ns_sec_msg_handler)
            {
                struct sec_msg_t sec_msg = {NS_SEC_NULL_MSG,NULL};
                sec_msg.msg_id = NS_SEC_PAIR_SUCCEED;
                app_sec_env.sec_init.ns_sec_msg_handler(&sec_msg); 
            }
            
        } break;
        
        case (GAPC_PAIRING_FAILED):
        {
            NS_LOG_DEBUG("GAPC_PAIRING_FAILED\r\n");
            app_sec_bond_data.auth = GAP_AUTH_NONE;
            ns_ble_disconnect();
            if(app_sec_env.sec_init.ns_sec_msg_handler)
            {
                struct sec_msg_t sec_msg = {NS_SEC_NULL_MSG,NULL};
                sec_msg.msg_id = NS_SEC_PAIR_FAILED;
                app_sec_env.sec_init.ns_sec_msg_handler(&sec_msg); 
            }
            
        } break;

        case (GAPC_IRK_EXCH):
        {
            NS_LOG_DEBUG("GAPC_IRK_EXCH\r\n");
            memcpy(&app_sec_bond_data.irk, &param->data.irk, sizeof(struct gapc_irk));
        } break;


        case (GAPC_LTK_EXCH) :
        {
            #if (BLE_APP_SEC_CON == 0)
            if (app_env.conn_env[app_env.conidx].role == ROLE_MASTER )
            #endif
            {
                memcpy(&app_sec_bond_data.ltk.key, &param->data.ltk.ltk.key, sizeof(struct gapc_ltk));
                memcpy(&app_sec_bond_data.rand_nb, &param->data.ltk.randnb, sizeof(rand_nb_t));
                app_sec_bond_data.ediv      = param->data.ltk.ediv;
                app_sec_bond_data.key_size  = param->data.ltk.key_size;
            }
        }
        break;
        
        case (GAPC_CSRK_EXCH) :
        {
            NS_LOG_DEBUG("GAPC_CSRK_EXCH\r\n");
        }
        break;

        default:
        {
            NS_LOG_DEBUG("gapc_bond_ind--default\r\n");
            ASSERT_ERR(0);
        } break;
    }

    return (KE_MSG_CONSUMED);
}

static int gapc_encrypt_req_ind_handler(ke_msg_id_t const msgid,
                                        struct gapc_encrypt_req_ind const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
    struct gapc_encrypt_cfm *cfm = KE_MSG_ALLOC(GAPC_ENCRYPT_CFM,
                                                src_id, TASK_APP,
                                                gapc_encrypt_cfm);
    
    NS_LOG_DEBUG("%s\r\n",__func__);

    cfm->found    = false;

    if (app_sec_env.bonded)
    {
//        NS_LOG_DEBUG("bonded,searching ltk\r\n");
        if(app_sec_env.sec_init.bond_enable)
        {
            uint8_t peer_num = 0;
            struct app_sec_bond_data_env_tag  bond_data = {0};
            
            peer_num = ns_bond_db_get_size();

            for(uint8_t i = peer_num; i > 0; i--)
            {
                ns_bond_db_load( i-1, &bond_data);
                #if (BLE_APP_SEC_CON)
                rand_nb_t lesc_rand_nb = {0};
                if(param->ediv == 0 &&
                   !memcmp(bond_data.rand_nb.nb, lesc_rand_nb.nb, GAP_RAND_NB_LEN))
                {
                    //LESC mode,  ediv and rand_nb are 0x0
                    if((app_env.peer_addr_type == GAPM_STATIC_ADDR) && 
                       (!memcmp(app_env.peer_addr.addr, bond_data.peer_addr.addr,GAP_BD_ADDR_LEN)) )
                    {
                        NS_LOG_INFO("static addr match\r\n");
                        cfm->found    = true;
                    }
                    else if(!memcmp(app_env.peer_irk.key, bond_data.irk.irk.key,GAP_KEY_LEN))
                    {
                        NS_LOG_INFO("irk match\r\n");
                        cfm->found    = true;
                    }
                }else 
                #endif
                if( (bond_data.ediv == param->ediv) && 
                    !memcmp(&bond_data.rand_nb, &param->rand_nb.nb, sizeof(struct rand_nb)) )
                {
                    NS_LOG_INFO("ediv&rand_nb match\r\n");
                    cfm->found    = true;
                }
                if(cfm->found)
                {
                    cfm->key_size = bond_data.key_size;
                    memcpy(cfm->ltk.key, bond_data.ltk.key, sizeof(struct gap_sec_key));
                    break;
                }
            }
        }
        else
        {
            if( (app_sec_bond_data.ediv == param->ediv) && 
                !memcmp(&app_sec_bond_data.rand_nb, &param->rand_nb.nb, sizeof(struct rand_nb)) )
            {
                NS_LOG_DEBUG("Found\r\n");
                
                cfm->found    = true;
                cfm->key_size = app_sec_bond_data.key_size;
                memcpy(cfm->ltk.key, app_sec_bond_data.ltk.key, sizeof(struct gap_sec_key));
            }
        }
    }
    if(cfm->found == false)
    {
        NS_LOG_DEBUG("Not found\r\n");
        if(app_sec_env.sec_init.ns_sec_msg_handler)
        {
            struct sec_msg_t sec_msg = {NS_SEC_NULL_MSG,NULL};
            sec_msg.msg_id = NS_SEC_LTK_MISSING;
            app_sec_env.sec_init.ns_sec_msg_handler(&sec_msg); 
        }
        
        //disconnect if key miss
        struct gapc_disconnect_cmd *p_cmd = KE_MSG_ALLOC(GAPC_DISCONNECT_CMD,
                                                       src_id , TASK_APP,
                                                       gapc_disconnect_cmd);

        NS_LOG_DEBUG("%s\r\n", __func__);
        p_cmd->operation = GAPC_DISCONNECT;
        p_cmd->reason    = CO_ERROR_PIN_MISSING;

        // Send the message
        ke_msg_send(p_cmd);
    }
    else{
        if(app_sec_env.sec_init.ns_sec_msg_handler)
        {
            struct sec_msg_t sec_msg = {NS_SEC_NULL_MSG,NULL};
            sec_msg.msg_id = NS_SEC_LTK_FOUND;
            app_sec_env.sec_init.ns_sec_msg_handler(&sec_msg);
        }            
    }

    ke_msg_send(cfm);
    
    return (KE_MSG_CONSUMED);
}


static int gapc_encrypt_ind_handler(ke_msg_id_t const msgid,
                                    struct gapc_encrypt_ind const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    NS_LOG_DEBUG("%s,lvl:%d\r\n",__func__,param->pairing_lvl);
    if(app_sec_env.sec_init.ns_sec_msg_handler)
    {
        struct sec_msg_t sec_msg = {NS_SEC_NULL_MSG,NULL};
        sec_msg.msg_id = NS_SEC_ENC_SUCCEED;
        app_sec_env.sec_init.ns_sec_msg_handler(&sec_msg);
    }
    return (KE_MSG_CONSUMED);
}



static int app_sec_msg_dflt_handler(ke_msg_id_t const msgid,
                                    void *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    NS_LOG_DEBUG("%s\r\n",__func__);
    
    return (KE_MSG_CONSUMED);
}

/** 
 * @brief Handles bond sync store handle
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] p_param   Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not. 
 */
int ns_sec_bond_store_evt_handler(ke_msg_id_t const msgid, void *p_param,
                                  ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
  
    NS_LOG_DEBUG("%s\r\n",__func__);

    rwip_time_t current_time = rwip_time_get();
    rwip_time_t target_time = {0};
    target_time.hs  = ip_clkntgt1_getf();
    target_time.hus = ip_hmicrosectgt1_getf();
    int32_t duration = CLK_DIFF(current_time.hs, target_time.hs);
    if((duration > 64) || (app_sec_env.store_latency == 0))// 20ms, xMS / 0.3125
    {
         //erase and write flash 
        ns_bond_db_add_entry(&app_sec_bond_data);
        NS_LOG_INFO("Bond info stored\r\n");
        #if 0
        NS_LOG_DEBUG("save peer, type:%d, addr:%02X %02X %02X %02X %02X %02X \r\n",
                                  app_sec_bond_data.peer_addr_type,
                                  app_sec_bond_data.peer_addr.addr[0],
                                  app_sec_bond_data.peer_addr.addr[1],
                                  app_sec_bond_data.peer_addr.addr[2],
                                  app_sec_bond_data.peer_addr.addr[3],
                                  app_sec_bond_data.peer_addr.addr[4],
                                  app_sec_bond_data.peer_addr.addr[5]);
        
        NS_LOG_DEBUG("save IRK addr:%02X %02X %02X %02X %02X %02X \r\n",
                                      app_sec_bond_data.irk.addr.addr.addr[0],
                                      app_sec_bond_data.irk.addr.addr.addr[1],
                                      app_sec_bond_data.irk.addr.addr.addr[2],
                                      app_sec_bond_data.irk.addr.addr.addr[3],
                                      app_sec_bond_data.irk.addr.addr.addr[4],
                                      app_sec_bond_data.irk.addr.addr.addr[5]);
        NS_LOG_DEBUG("save IRK key:");
        for(uint8_t i = 0; i<GAP_KEY_LEN;i++)
        {
            NS_LOG_DEBUG(" %02X",app_sec_bond_data.irk.irk.key[i]);
        }
        NS_LOG_DEBUG("\r\n");
        #endif
        struct gap_ral_dev_info dev_info = {0};
        memcpy(dev_info.addr.addr.addr,app_sec_bond_data.irk.addr.addr.addr,GAP_BD_ADDR_LEN);
        dev_info.addr.addr_type = app_sec_bond_data.irk.addr.addr_type;
        memcpy(dev_info.peer_irk,app_sec_bond_data.irk.irk.key,GAP_KEY_LEN);
        if (gap_env.mac_addr_type == GAPM_GEN_RSLV_ADDR)
        {
           memcpy(dev_info.local_irk,&app_env.loc_irk[0],GAP_KEY_LEN);
        }
        else
        {
            memset(dev_info.local_irk, 0, GAP_KEY_LEN);
        }
        app_env.rand_cnt = 1;
        ns_ble_list_set_ral(&dev_info,1);    
    }
    else{
        app_sec_env.store_latency --;
        ke_timer_set(APP_BOND_STORE_EVT, TASK_APP, 20);
    }
    
    
    return (KE_MSG_CONSUMED);
}


/// Default State handlers definition
const struct ke_msg_handler app_sec_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,  (ke_msg_func_t)app_sec_msg_dflt_handler},

    {GAPC_BOND_REQ_IND,       (ke_msg_func_t)gapc_bond_req_ind_handler},
    {GAPC_BOND_IND,           (ke_msg_func_t)gapc_bond_ind_handler},

    {GAPC_ENCRYPT_REQ_IND,    (ke_msg_func_t)gapc_encrypt_req_ind_handler},
    {GAPC_ENCRYPT_IND,        (ke_msg_func_t)gapc_encrypt_ind_handler},
    
    
};

const struct app_subtask_handlers app_sec_handlers = {&app_sec_msg_handler_list[0], ARRAY_LEN(app_sec_msg_handler_list)};

#endif //(BLE_APP_SEC)

/// @} APP
