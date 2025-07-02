#include "display_device.h"
#include <string.h>
/* 全局变量，用于保存所有注册的输入设备 */
static PDisplayDevice g_ptDisplayDevicesList = {0};

 
/** 
* @brief 注册单个输入设备到链表 
* @param ptDisplayDevice 输入设备指针 
* 
* 该函数是底层的设备插入函数，将单个输入设备添加到全局输入设备链表中。 
* 采用头插法，如果链表为空，则新设备成为链表的头部。 
* 此函数不关心设备的具体类型，只负责维护链表结构。 
*/ 
void DisplayDeviceRegister(PDisplayDevice ptDisplayDevice) 
{ 
    if (ptDisplayDevice == NULL) { 
        return; // 错误处理：输入设备指针为空 
    } 
    // 将新设备的 pNextDevice 指向当前链表头 
    ptDisplayDevice->pNextDevice = g_ptDisplayDevicesList; 
    // 更新链表头指针指向新设备 
    g_ptDisplayDevicesList = ptDisplayDevice; 
} 

/**
 * 获取显示设备
 * @param name 设备名称
 * @return 显示设备指针
 * @note 函数返回的指针为全局变量，请勿释放
 * 该函数用于根据设备名称获取对应的显示设备指针。
 * 如果找不到对应的设备，则返回NULL。
 * * @details 该函数完成以下工作:
 * 1. 遍历全局显示设备链表，查找与指定名称的设备匹配的显示设备。
 * * 2. 如果找到匹配的设备，则返回该设备的指针。
 * * 3. 如果没有找到匹配的设备，则返回NULL。
 *
 */
PDisplayDevice __GetDisplayDevice(char *name){
    PDisplayDevice pTemp = g_ptDisplayDevicesList;
    while (pTemp != NULL) {
        if (strcmp(pTemp->name, name) == 0) {
            return pTemp; // 找到匹配的设备，返回其指针
        }
        pTemp = pTemp->pNextDevice; // 移动到下一个设备
    }
    return NULL; // 没有找到匹配的设备，返回NULL
}

 

 
