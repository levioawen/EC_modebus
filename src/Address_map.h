//Input register

#define ec_address 0x0000
#define ec_k_valueLow_address 0x0001
#define ec_k_valueHigh_address 0x0002
#define ph_address 0x0010
#define ph_k_valueNeutral_address 0x0011
#define ph_k_valueAcid_address 0x0012

//Holding register
#define temperature_in_out_address 0x0000
#define modbus_id_address 0x0001
#define modbus_baud_rate_address 0x0002
//Coils
#define ec_calib_high_start_address 0x0000
#define ec_calib_low_start_address 0x0001
#define ec_calib_high_end_address 0x0002
#define ec_calib_low_end_address 0x0003
#define ec_calib_error_address 0x0009

#define ph_calib_neutral_start_address 0x0010
#define ph_calib_acid_start_address 0x0011
#define ph_calib_neutral_end_address 0x0012
#define ph_calib_acid_end_address 0x0013
#define ph_calib_error_address 0x0019

#define temperature_option_address 0x0020


