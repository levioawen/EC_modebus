//
// Created by Andrey on 01.03.2024.
//

#ifndef EDC_ADDRESS_TABLE_H
#define EDC_ADDRESS_TABLE_H

#endif //EDC_ADDRESS_TABLE_H


//Discrete Inputs:
#define CALC_ERR_ADDRESS 0x0000
#define STATUS_1_ADDRESS 0x0001
#define STAY_ADDRESS 0x0002
#define STATUS_2_ADDRESS 0x0003
#define STABLE_S_ADDRESS 0x0004

//Coils:
#define HL_DATA_ADDRESS 0x0000
#define CALC_START_ADDRESS 0x0001
#define CALC_STOP_ADDRESS 0x0002

#define CHANGE_ID_ADDRESS 0x0100
#define BAN_ID_1_ADDRESS 0x0101
//дополнить или как это сделать придумать..
#define BAN_ID_247_ADDRESS 0x01F7


//Input Registers:
#define _5V_ADDRESS 0x0000
#define V0_ADDRESS 0x0001
#define VOUT_ADDRESS 0x0002
#define VSEN_ADDRESS 0x0003
#define Ro_ADDRESS 0x0004
#define S_ADDRESS 0x0005
#define EC_ADDRESS 0x0006
#define TDS_ADDRESS 0x0007


//Holding Registers:
#define FREQUENCY_ADDRESS 0x0000
#define KNOWN_TDS_1_ADDRESS 0x0000
#define KNOWN_TDS_2_ADDRESS 0x0001
#define Ka_L_ADDRESS 0x0002
#define Ka_H_ADDRESS 0x0004
#define Kb_ADDRESS 0x0005
#define STABLE_S_ADDRESS 0x0006
#define UNSTABLE_S_ADDRESS 0x0007
#define CALC_SAVE_ADDRESS 0x0008
#define Kt_ADDRESS 0x0009
#define Kp_ADDRESS 0x000A
#define t_ADDRESS 0x000B
#define T_ADDRESS 0x000C
#define DISPLAY_ADDRESS 0x000D


#define ACK_ID_ADDRESS 0x0100
#define ACK_SPEED_ADDRESS 0x0101
#define ACK_TYPE_ADDRESS 0x0102
#define SET_ID_ADDRESS 0x0110
#define SET_SPEED_ADDRESS 0x0111
#define SET_TYPE_ADDRESS 0x0112
#define USER_DATA_ADDRESS 0x0120





