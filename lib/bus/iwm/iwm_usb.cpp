#ifdef BUILD_APPLE

#if SMARTPORT == USB

#include <string.h>

#include "iwm_usb.h"
#include "iwm.h"

uint8_t _phases;

sp_cmd_state_t sp_command_mode;

void iwm_usb::setup_gpio()
{
}

void iwm_usb::setup_spi()
{
}

void iwm_usb::spi_end()
{
}

bool iwm_usb::req_wait_for_falling_timeout(int t)
{
  return false;
}

bool iwm_usb::req_wait_for_rising_timeout(int t)
{
  return false;
}

int iwm_usb::iwm_send_packet_spi()
{
  return 0;
}

int iwm_usb::iwm_read_packet_spi(int n)
{
  return iwm_read_packet_spi(packet_buffer, n);
}

int iwm_usb::iwm_read_packet_spi(uint8_t* buffer, int n)
{
  return 0;
}

void iwm_usb::encode_packet(uint8_t source, iwm_packet_type_t packet_type, uint8_t status, const uint8_t* data, uint16_t num)
{
}

size_t iwm_usb::decode_data_packet(uint8_t* output_data)
{
  return decode_data_packet(packet_buffer, output_data);
}

size_t iwm_usb::decode_data_packet(uint8_t* input_data, uint8_t* output_data)
{
  return 0;
}

iwm_usb smartport;

#endif

#endif
