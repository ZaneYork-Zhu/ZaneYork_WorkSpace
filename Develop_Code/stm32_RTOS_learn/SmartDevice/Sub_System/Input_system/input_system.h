#ifndef     __INPUT_SYSTEM_H__
#define     __INPUT_SYSTEM_H__

#ifndef NULL
#define NULL    ((void *)0)  /* 定义 NULL 为指向空的指针 */
#endif

#define    INPUT_BUFFER_SIZE  10

#define    TIME_T            long int  /* 时间类型，使用 long int 以支持更长的时间戳 */
#define    OLED_T            int
#define    KEY_T             int
#define    Data_T            char


/* 输入事件类型 */
typedef enum
{
    INPUT_EVENT_TYPE_KEY = 0,
    INPUT_EVENT_TYPE_TOUCH,  /* 触摸事件 */
    INPUT_EVENT_TYPE_OLED,  /* OLED 显示事件 */
    INPUT_EVENT_TYPE_TIME,  /* 时间事件 */
}INPUT_EVENT_TYPE;

/* 输入事件【数据】结构体 */
typedef struct inputSystem
{
    /* 输入时间 */
    TIME_T time;
    /* 输入事件类型 */
    INPUT_EVENT_TYPE eType;  
    /* OLED 显示坐标 */
    OLED_T coordinate;
    /* 输入按键 */
    KEY_T key;
    /* 输入按键值 */
    KEY_T keyValue;    
    /* 输入数据 */
    Data_T str[INPUT_BUFFER_SIZE];
    
    /* input system data */
} InputEvent,*PInputEvent;

/* 输入设备结构体 */
typedef struct InputDevice  
{
    /* 输入设备名称标识 */
    char *name;
    
    /* 获取输入事件状态 */
    int (*GetInputEventState)(PInputEvent ptInputEvent);
    /*  设备初始化函数  */
    int (*DeviceInit)(void);
    /*  设备注销初始化函数  */
    int (*DeviceDeInit)(void);
    /* 输入设备链表结构体 */
    struct InputDevice *pNextDevice;  /* 下一个输入设备 */
}InputDevice,*PInputDevice;

/* 添加输入设备 */
void AddInputDevices(void);
/* 输入设备注册函数 */
void InputDeviceRegister(PInputDevice ptInputDevice);
/* 初始化输入设备链表 */
void InitInputDevices(void) ;




#endif  // __INPUT_SYSTEM_H__
