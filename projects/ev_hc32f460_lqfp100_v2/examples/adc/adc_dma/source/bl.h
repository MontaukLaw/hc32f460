#ifndef _BL_H_
#define _BL_H_

void bl_task(void);

void bl_link_status_check(void);

void ble_task(void);

extern uint8_t bl_conn_status;

extern uint8_t mac_add[];

extern uint8_t bl_tx_buf[];

#endif /* _BL_H_ */
