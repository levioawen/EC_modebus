#include "stdint.h"




int read_temp_skip_rom(void);
uint8_t ow_search(uint8_t * new_addr);
void ow_reset_search(void);
void select_(uint8_t *rom);
int read_temp(uint8_t rom[8]);
void one_wire_search(void);
int read_temp_saved_sensors(int * sensor_temp);
void ow_ini();
void query_temp_saved_sensors();
int get_temp_saved_sensors(int *sensor_temp);
void ow_read_addr();