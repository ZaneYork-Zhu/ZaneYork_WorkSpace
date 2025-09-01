#include <stdio.h>
#include "input_system.h"
#include "input_buffer.h"
 
void input_test(void)
{
	InputEvent event;
	
	AddInputDevices();
	InitInputDevices();
	while (1)
	{
		if (GetInputEvent(&event) == INPUT_EVENT_STATE_USING)
		{
			printf("get input event:\r\n");
			printf("type: %d\r\n", event.eType);
			printf("time: %d\r\n", event.time);
			printf("key : %d\r\n", event.key);
			printf("pressure : %d\r\n", event.keyValue);
		} 
	}
}
