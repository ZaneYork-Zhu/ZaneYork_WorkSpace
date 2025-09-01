/** 
 * @note: 
 *   本代码实现了一个环形缓冲区用于输入事件的存储和管理。环形缓冲区是一种用于循环存储数据的缓冲区结构， 
 *   其核心思想是将线性的数组空间视为一个环形空间，通过两个指针（读指针和写指针）来管理数据的读写操作， 
 *   从而实现数据的循环利用，避免了普通线性缓冲区在数据读写过程中可能出现的频繁移动数据或空间浪费的问题。 
 * 
 * 1. 数据结构： 
 *    环形缓冲区使用一个固定大小的数组 `buffer` 来存储输入事件，同时使用两个指针 `pRead`（读指针）和 `pWrite`（写指针） 
 *    分别指示当前的读位置和写位置。`BUFFER_SIZE` 定义了缓冲区的最大容量。 
 * 
 * 2. 读取操作： 
 *    当调用 `GetInputEvent` 函数时，首先计算下一个读地址 `nextReadAddr`，它是通过 `(g_tInputBuffer.pRead + 1) % BUFFER_SIZE` 计算得到的， 
 *    这样可以确保指针在到达数组末尾时能够回到数组开头，实现环形效果。 
 *    然后检查下一个读地址是否等于写地址，如果不相等，说明缓冲区中有数据可以读取。此时将读指针指向的数据读取出来，并更新读指针， 
 *    最后返回 `INPUT_EVENT_STATE_USING` 表示成功读取。 
 *    如果下一个读地址等于写地址，说明缓冲区为空，返回 `INPUT_EVENT_STATE_EMPTY`。 
 * 
 * 3. 写入操作： 
 *    当调用 `PutInputEvent` 函数时，首先计算下一个写地址 `nextWriteAddr`，同样通过 `(g_tInputBuffer.pWrite + 1) % BUFFER_SIZE` 计算得到。 
 *    接着检查下一个写地址是否等于读地址，如果不相等，说明缓冲区还有空间可以写入数据。此时将数据写入写指针指向的位置，并更新写指针， 
 *    返回 `INPUT_EVENT_STATE_USING` 表示成功写入。 
 *    如果下一个写地址等于读地址，说明缓冲区已满，不能再写入数据，返回 `INPUT_EVENT_STATE_FULL`。 
 */ 
#include "input_buffer.h"
/*输入事件缓冲区*/
static InputEventBuffer g_tInputBuffer = {0};

/*获取一个输入事件*/
INPUT_EVENT_STATE GetInputEvent(PInputEvent ptInputEvent)
{
    /*检查输入事件指针是否为NULL*/
    if(ptInputEvent == NULL) {
        return INPUT_EVENT_STATE_ERROR; /*错误状态*/
    }
    /*检查缓冲区是否为空*/
    /*如果读指针等于写指针，说明缓冲区为空*/
    if(g_tInputBuffer.pRead == g_tInputBuffer.pWrite){
        return INPUT_EVENT_STATE_ERROR;
    }
    else{
        /*检测缓冲区是否为空*/
        int nextReadAddr = (int)(g_tInputBuffer.pRead + 1) % BUFFER_SIZE; /*下一个读地址*/
        /*如果下一个读地址等于写地址，说明缓冲区为空*/
        if(nextReadAddr != g_tInputBuffer.pWrite) {
            /*如果下一个读地址不等于写地址，说明有数据可以读取[没有写满]*/
            *ptInputEvent = g_tInputBuffer.buffer[g_tInputBuffer.pRead]; /*读取数据*/
            g_tInputBuffer.pRead = nextReadAddr; /*更新读地址*/
            return INPUT_EVENT_STATE_USING; /*成功读取*/
        } 
        else {
            /*没有数据可以读取*/
            return INPUT_EVENT_STATE_EMPTY; /*缓冲区为空*/
        }
    }
}
/*写入一个输入事件*/
INPUT_EVENT_STATE PutInputEvent(PInputEvent ptInputEvent)
{
    /*检查输入事件指针是否为NULL*/
    if(ptInputEvent == NULL) {
        return INPUT_EVENT_STATE_ERROR; /*错误状态*/
    }
    int nextWriteAddr = (int)(g_tInputBuffer.pWrite + 1) % BUFFER_SIZE; /*下一个写地址*/
    /*如果下一个写地址等于读地址，说明缓冲区已满*/
    if(nextWriteAddr != g_tInputBuffer.pRead) {
        /*如果下一个写地址不等于读地址，说明有空间可以写入[没有写满]*/
        g_tInputBuffer.buffer[g_tInputBuffer.pWrite] = *ptInputEvent; /*写入数据*/
        g_tInputBuffer.pWrite = nextWriteAddr; /*更新写地址*/
        return INPUT_EVENT_STATE_USING; /*成功写入*/
    } 
    else {
        /*缓冲区已满，不能再写入数据*/
        return INPUT_EVENT_STATE_FULL; /*缓冲区已满*/
    }
}
