#ifndef __IDG_H_
#define __IDG_H_


#include "main.h"

void iwdg_init(uint32_t prer, uint16_t rlr);        /* ��ʼ��IWDG����ʹ��IWDG */
void iwdg_feed(void);                               /* ι�� */


void iwdg_text_init(void);
void iwdg_text_while(void);

#endif
