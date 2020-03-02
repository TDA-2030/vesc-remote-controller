/*
	Copyright 2015 Benjamin Vedder	benjamin@vedder.se

	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */

/*
 * bldc_interface.c
 *
 * Compatible Firmware Versions
 * 1.3
 * 1.4
 *
 */

#include "bldc_interface.h"
#include "confgenerator.h"
#include "buffer.h"
#include "packet.h"

#include <string.h>
#include <stdio.h>



// Private variables
static unsigned char send_buffer[PACKET_MAX_PL_LEN];




vesc_info_t vesc_info={0};


// Function pointers
static void(*send_func)(unsigned char *data, unsigned int len) = 0;
static void(*forward_func)(unsigned char *data, unsigned int len) = 0;

// Function pointers for received data
static void(*rx_value_func)(mc_values *values) = 0;
static void(*rx_fw_func)(int major, int minor) = 0;
static void(*rx_rotor_pos_func)(float pos) = 0;
static void(*rx_mcconf_func)(mc_configuration *conf) = 0;
static void(*rx_appconf_func)(app_configuration *conf) = 0;
static void(*rx_detect_func)(float cycle_int_limit, float coupling_k,
		const signed char *hall_table, signed char hall_res) = 0;
static void(*rx_dec_ppm_func)(float val, float ms) = 0;
static void(*rx_dec_adc_func)(float val, float voltage) = 0;
static void(*rx_dec_chuk_func)(float val) = 0;
static void(*rx_mcconf_received_func)(void) = 0;
static void(*rx_appconf_received_func)(void) = 0;

void bldc_interface_init(void(*send_fun)(unsigned char *data, unsigned int len)) {
	send_func = send_fun;
}

void bldc_interface_set_forward_func(void(*forward_fun)(unsigned char *data, unsigned int len)) {
	forward_func = forward_fun;
}

/**
 * Send a packet using the set send function.
 *
 * @param data
 * The packet data.
 *
 * @param len
 * The data length.
 */
void bldc_interface_send_packet(unsigned char *data, unsigned int len) {
	if (send_func) {
		send_func(data, len);
	}
}

/**
 * Process a received buffer with commands and data.
 *
 * @param data
 * The buffer to process.
 *
 * @param len
 * The length of the buffer.
 */
void bldc_interface_process_packet(unsigned char *data, unsigned int len) {
	int32_t ind = 0;
	int i = 0;
	COMM_PACKET_ID packet_id;
	
	if (!len) {
		return;
	}

	if (forward_func) {
		forward_func(data, len);
		return;
	}
	
	packet_id = (COMM_PACKET_ID)data[0];
	data++;
	len--;

	switch (packet_id) {
	case COMM_FW_VERSION:{
		ind = 0;
		
		if (len < 23) {
			
			vesc_info.fw_major = data[ind++];
			vesc_info.fw_minor = data[ind++];
			
			strncpy(vesc_info.hw_name, (char*)data + ind, sizeof(vesc_info.hw_name)-1);
			ind += (strlen((char*)data + ind) + 1);
			
			memcpy(vesc_info.stm32_uuid, data + ind, 12);
			
			//pairing_done 还有一个字节指示是否配对成功		
			
			
			if (rx_fw_func) {
				rx_fw_func(vesc_info.fw_major, vesc_info.fw_minor);
			}
			
		} else {
			vesc_info.fw_major = -1;
			vesc_info.fw_minor = -1;
		}
	}break;

	case COMM_ERASE_NEW_APP:
	case COMM_WRITE_NEW_APP_DATA:
		// TODO
		break;

	case COMM_GET_VALUES:
		ind = 0;
		vesc_info.values.temp_fet_filtered = buffer_get_float16(data, 1e1, &ind);
		vesc_info.values.temp_motor_filtered = buffer_get_float16(data,  1e1, &ind);
		vesc_info.values.motor_current = buffer_get_float32(data,  1e2, &ind);
		vesc_info.values.input_current = buffer_get_float32(data,  1e2, &ind);
		vesc_info.values.avg_id = buffer_get_float32(data,  1e2, &ind);
		vesc_info.values.avg_iq = buffer_get_float32(data,  1e2, &ind);
		vesc_info.values.duty_cycle_now = buffer_get_float16(data,  1e3, &ind);
		vesc_info.values.rpm = buffer_get_float32(data,  1e0, &ind);
		vesc_info.values.vin = buffer_get_float16(data,  1e1, &ind);
		vesc_info.values.amp_hours = buffer_get_float32(data,  1e4, &ind);
		vesc_info.values.amp_hours_charged = buffer_get_float32(data,  1e4, &ind);
		vesc_info.values.watt_hours = buffer_get_float32(data,  1e4, &ind);
		vesc_info.values.watt_hours_charged = buffer_get_float32(data,  1e4, &ind);
		vesc_info.values.tachometer_value = buffer_get_int32(data,  &ind);
		vesc_info.values.tachometer_abs_value = buffer_get_int32(data,  &ind);
		vesc_info.values.fault_code = (mc_fault_code)data[ind++];
		vesc_info.values.pid_pos_now = buffer_get_float32(data,  1e6, &ind);
		vesc_info.values.controller_id = data[ind++];
		vesc_info.values.temp_mos1 = buffer_get_float16(data,  1e1, &ind);
		vesc_info.values.temp_mos2 = buffer_get_float16(data,  1e1, &ind);
		vesc_info.values.temp_mos3 = buffer_get_float16(data,  1e1, &ind);
		vesc_info.values.avg_vd = buffer_get_float32(data,  1e3, &ind);
		vesc_info.values.avg_vq = buffer_get_float32(data,  1e3, &ind);
		
		if (rx_value_func) {
			rx_value_func(&vesc_info.values);
		}
		break;

	case COMM_PRINT:
		// TODO
		break;

	case COMM_SAMPLE_PRINT:
		// TODO
		break;

	case COMM_ROTOR_POSITION:
		ind = 0;
		vesc_info.rotor_pos = buffer_get_float32(data, 100000.0, &ind);

		if (rx_rotor_pos_func) {
			rx_rotor_pos_func(vesc_info.rotor_pos);
		}
		break;

	case COMM_EXPERIMENT_SAMPLE:
		// TODO
		break;

	case COMM_GET_MCCONF:{
		if(false != confgenerator_deserialize_mcconf(data, &vesc_info.mcconf))
		{
			if (rx_mcconf_func) {
				rx_mcconf_func(&vesc_info.mcconf);
			}
		}
		
	}break;

	case COMM_GET_APPCONF:{
		if(false != confgenerator_deserialize_appconf(data, &vesc_info.appconf))
		{
			if (rx_appconf_func) {
				rx_appconf_func(&vesc_info.appconf);
			}
		}
		
	}break;
		
	case COMM_DETECT_MOTOR_PARAM:
		ind = 0;
		vesc_info.detect_cycle_int_limit = buffer_get_float32(data, 1000.0, &ind);
		vesc_info.detect_coupling_k = buffer_get_float32(data, 1000.0, &ind);
		for (i = 0;i < 8;i++) {
			vesc_info.detect_hall_table[i] = data[ind++];
		}
		vesc_info.detect_hall_res = data[ind++];

		if (rx_detect_func) {
			rx_detect_func(vesc_info.detect_cycle_int_limit, vesc_info.detect_coupling_k,
					vesc_info.detect_hall_table, vesc_info.detect_hall_res);
		}
		break;

	case COMM_GET_DECODED_PPM:
		ind = 0;
		vesc_info.dec_ppm = buffer_get_float32(data, 1000000.0, &ind);
		vesc_info.dec_ppm_len = buffer_get_float32(data, 1000000.0, &ind);

		if (rx_dec_ppm_func) {
			rx_dec_ppm_func(vesc_info.dec_ppm, vesc_info.dec_ppm_len);
		}
		break;

	case COMM_GET_DECODED_ADC:
		ind = 0;
		vesc_info.dec_adc = buffer_get_float32(data, 1000000.0, &ind);
		vesc_info.dec_adc_voltage = buffer_get_float32(data, 1000000.0, &ind);

		if (rx_dec_adc_func) {
			rx_dec_adc_func(vesc_info.dec_adc, vesc_info.dec_adc_voltage);
		}
		break;

	case COMM_GET_DECODED_CHUK:
		ind = 0;
		vesc_info.dec_chuk = buffer_get_float32(data, 1000000.0, &ind);

		if (rx_dec_chuk_func) {
			rx_dec_chuk_func(vesc_info.dec_chuk);
		}
		break;

	case COMM_SET_MCCONF:
		// This is a confirmation that the new mcconf is received.
		if (rx_mcconf_received_func) {
			rx_mcconf_received_func();
		}
		break;

	case COMM_SET_APPCONF:
		// This is a confirmation that the new appconf is received.
		if (rx_appconf_received_func) {
			rx_appconf_received_func();
		}
		break;

	default:
		break;
	}
}

/**
 * Function pointer setters. When data that is requested with the get functions
 * is received, the corresponding function pointer will be called with the
 * received data.
 *
 * @param func
 * A function to be called when the corresponding data is received.
 */

void bldc_interface_set_rx_value_func(void(*func)(mc_values *values)) {
	rx_value_func = func;
}

void bldc_interface_set_rx_fw_func(void(*func)(int major, int minor)) {
	rx_fw_func = func;
}

void bldc_interface_set_rx_rotor_pos_func(void(*func)(float pos)) {
	rx_rotor_pos_func = func;
}

void bldc_interface_set_rx_mcconf_func(void(*func)(mc_configuration *conf)) {
	rx_mcconf_func = func;
}

void bldc_interface_set_rx_appconf_func(void(*func)(app_configuration *conf)) {
	rx_appconf_func = func;
}

void bldc_interface_set_rx_detect_func(void(*func)(float cycle_int_limit, float coupling_k,
		const signed char *hall_table, signed char hall_res)) {
	rx_detect_func = func;
}

void bldc_interface_set_rx_dec_ppm_func(void(*func)(float val, float ms)) {
	rx_dec_ppm_func = func;
}

void bldc_interface_set_rx_dec_adc_func(void(*func)(float val, float voltage)) {
	rx_dec_adc_func = func;
}

void bldc_interface_set_rx_dec_chuk_func(void(*func)(float val)) {
	rx_dec_chuk_func = func;
}

void bldc_interface_set_rx_mcconf_received_func(void(*func)(void)) {
	rx_mcconf_received_func = func;
}

void bldc_interface_set_rx_appconf_received_func(void(*func)(void)) {
	rx_appconf_received_func = func;
}

// Setters
void bldc_interface_set_duty_cycle(float dutyCycle) {
	int32_t send_index = 0;
	send_buffer[send_index++] = COMM_SET_DUTY;
	buffer_append_float32(send_buffer, dutyCycle, 100000.0, &send_index);
	bldc_interface_send_packet(send_buffer, send_index);
}

void bldc_interface_set_current(float current) {
	int32_t send_index = 0;
	send_buffer[send_index++] = COMM_SET_CURRENT;
	buffer_append_float32(send_buffer, current, 1000.0, &send_index);
	bldc_interface_send_packet(send_buffer, send_index);
}

void bldc_interface_set_current_brake(float current) {
	int32_t send_index = 0;
	send_buffer[send_index++] = COMM_SET_CURRENT_BRAKE;
	buffer_append_float32(send_buffer, current, 1000.0, &send_index);
	bldc_interface_send_packet(send_buffer, send_index);
}

void bldc_interface_set_rpm(int rpm) {
	int32_t send_index = 0;
	send_buffer[send_index++] = COMM_SET_RPM;
	buffer_append_int32(send_buffer, rpm, &send_index);
	bldc_interface_send_packet(send_buffer, send_index);
}

void bldc_interface_set_pos(float pos) {
	int32_t send_index = 0;
	send_buffer[send_index++] = COMM_SET_POS;
	buffer_append_float32(send_buffer, pos, 1000000.0, &send_index);
	bldc_interface_send_packet(send_buffer, send_index);
}

void bldc_interface_set_servo_pos(float pos) {
	int32_t send_index = 0;
	send_buffer[send_index++] = COMM_SET_SERVO_POS;
	buffer_append_float16(send_buffer, pos, 1000.0, &send_index);
	bldc_interface_send_packet(send_buffer, send_index);
}

void bldc_interface_set_mcconf(const mc_configuration *mcconf) {
	send_buffer[0] = COMM_SET_MCCONF;
	int32_t len = confgenerator_serialize_mcconf(send_buffer + 1, mcconf);
    bldc_interface_send_packet(send_buffer, len + 1);
}

void bldc_interface_set_appconf(const app_configuration *appconf) {
	send_buffer[0] = COMM_SET_APPCONF;
	int32_t len = confgenerator_serialize_appconf(send_buffer + 1, appconf);
	bldc_interface_send_packet(send_buffer, len + 1);
}

// Getters
void bldc_interface_get_fw_version(void) {
	int32_t send_index = 0;
	send_buffer[send_index++] = COMM_FW_VERSION;
	bldc_interface_send_packet(send_buffer, send_index);
}

void bldc_interface_get_values(void) {
	int32_t send_index = 0;
	send_buffer[send_index++] = COMM_GET_VALUES;
	bldc_interface_send_packet(send_buffer, send_index);
}

void bldc_interface_get_mcconf(void) {
	int32_t send_index = 0;
	send_buffer[send_index++] = COMM_GET_MCCONF;
	bldc_interface_send_packet(send_buffer, send_index);
}

void bldc_interface_get_appconf(void) {
	int32_t send_index = 0;
	send_buffer[send_index++] = COMM_GET_APPCONF;
	bldc_interface_send_packet(send_buffer, send_index);
}

void bldc_interface_get_decoded_ppm(void) {
	int32_t send_index = 0;
	send_buffer[send_index++] = COMM_GET_DECODED_PPM;
	bldc_interface_send_packet(send_buffer, send_index);
}

void bldc_interface_get_decoded_adc(void) {
	int32_t send_index = 0;
	send_buffer[send_index++] = COMM_GET_DECODED_ADC;
	bldc_interface_send_packet(send_buffer, send_index);
}

void bldc_interface_get_decoded_chuk(void) {
	int32_t send_index = 0;
	send_buffer[send_index++] = COMM_GET_DECODED_CHUK;
	bldc_interface_send_packet(send_buffer, send_index);
}

// Other functions
void bldc_interface_detect_motor_param(float current, float min_rpm, float low_duty) {
	int32_t send_index = 0;
	send_buffer[send_index++] = COMM_DETECT_MOTOR_PARAM;
	buffer_append_float32(send_buffer, current, 1000.0, &send_index);
	buffer_append_float32(send_buffer, min_rpm, 1000.0, &send_index);
	buffer_append_float32(send_buffer, low_duty, 1000.0, &send_index);
	bldc_interface_send_packet(send_buffer, send_index);
}

void bldc_interface_reboot(void) {
	int32_t send_index = 0;
	send_buffer[send_index++] = COMM_REBOOT;
	bldc_interface_send_packet(send_buffer, send_index);
}

void bldc_interface_send_alive(void) {
	int32_t send_index = 0;
	send_buffer[send_index++] = COMM_ALIVE;
	bldc_interface_send_packet(send_buffer, send_index);
}

// Helpers
const char* bldc_interface_fault_to_string(mc_fault_code fault) {
	switch (fault) {
	case FAULT_CODE_NONE: return "FAULT_CODE_NONE"; 
	case FAULT_CODE_OVER_VOLTAGE: return "FAULT_CODE_OVER_VOLTAGE"; 
	case FAULT_CODE_UNDER_VOLTAGE: return "FAULT_CODE_UNDER_VOLTAGE"; 
	case FAULT_CODE_ABS_OVER_CURRENT: return "FAULT_CODE_ABS_OVER_CURRENT"; 
	case FAULT_CODE_OVER_TEMP_FET: return "FAULT_CODE_OVER_TEMP_FET"; 
	case FAULT_CODE_OVER_TEMP_MOTOR: return "FAULT_CODE_OVER_TEMP_MOTOR"; 
	default: return "FAULT_UNKNOWN"; 
	}
}
