#ifndef __INPUT_BUFFER_H__
#define __INPUT_BUFFER_H__

#include "input_system.h"


/*环形缓冲区大小*/
#define BUFFER_SIZE 10

typedef enum {
    INPUT_EVENT_STATE_IDLE = 0, /*空闲状态*/
    INPUT_EVENT_STATE_USING, /*使用中状态*/
    INPUT_EVENT_STATE_FULL, /*缓冲区已满*/
    INPUT_EVENT_STATE_EMPTY, /*缓冲区为空*/
    INPUT_EVENT_STATE_ERROR /*错误状态*/
} INPUT_EVENT_STATE;

typedef struct {
    InputEvent buffer[BUFFER_SIZE]; /*环形缓冲区*/
    volatile unsigned int pWrite;/*写地址指针*/
    volatile unsigned int pRead;/*读地址指针*/
} InputEventBuffer;

/*获取(读取)一个输入事件*/
INPUT_EVENT_STATE GetInputEvent(PInputEvent ptInputEvent);
/*写入一个输入事件*/
INPUT_EVENT_STATE PutInputEvent(PInputEvent ptInputEvent);

#endif /* __INPUT_BUFFER_H__ */
