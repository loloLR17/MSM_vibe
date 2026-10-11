# Defines only; no connection, reset, halt, memory write or flash.
# Invoke while halted, after verifying flash against the matching BIN.
define tr2-rx-snapshot
  printf "tr2_modbus_application_binding_result = %u\n", *(unsigned int *)&tr2_modbus_application_binding_result
  printf "tr2_rtu_rx_ready = %u\n", *(unsigned int *)&tr2_rtu_rx_ready
  printf "tr2_rtu_rx_poll_count = %u\n", *(unsigned int *)&tr2_rtu_rx_poll_count
  printf "tr2_rtu_rx_byte_count = %u\n", *(unsigned int *)&tr2_rtu_rx_byte_count
  printf "tr2_rtu_rx_frame_count = %u\n", *(unsigned int *)&tr2_rtu_rx_frame_count
  printf "tr2_rtu_rx_valid_adu_count = %u\n", *(unsigned int *)&tr2_rtu_rx_valid_adu_count
  printf "tr2_rtu_rx_invalid_adu_count = %u\n", *(unsigned int *)&tr2_rtu_rx_invalid_adu_count
  printf "tr2_rtu_rx_crc_error_count = %u\n", *(unsigned int *)&tr2_rtu_rx_crc_error_count
  printf "tr2_rtu_rx_error_count = %u\n", *(unsigned int *)&tr2_rtu_rx_error_count
  printf "tr2_rtu_rx_last_error = %u\n", *(unsigned int *)&tr2_rtu_rx_last_error
  printf "tr2_rtu_rx_last_frame_length = %u\n", *(unsigned int *)&tr2_rtu_rx_last_frame_length
  printf "tr2_rtu_rx_last_decode_result = %u\n", *(unsigned int *)&tr2_rtu_rx_last_decode_result
  x/8bx &tr2_rtu_rx_last_adu
end
