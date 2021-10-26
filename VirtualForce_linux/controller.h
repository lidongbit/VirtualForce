#ifndef  __CONTROLLER_H__
#define __CONTROLLER_H__
extern void *controller_msg_process(void *p);
extern void *controller_display_result(void *p);
extern void controller_display_res();
extern void controller_msg_parase();
extern void setCurrentThreadHighPriority(int value, int p);
extern void *contoller_send_thread(void *p);
#endif
