#ifndef __WWDG_H__
#define __WWDG_H__

#include "main.h"

void wwdg_text_init(void);
void wwdg_text_while(void);
void wwdg_init(uint8_t tr, uint8_t wr, uint32_t fprer); /*窗口看门狗初始化*/

#endif
