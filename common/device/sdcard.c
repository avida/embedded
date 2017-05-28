#include "sdcard.h"
#include <utils.h>
#include <uart.h>
#include <string.h>

// Some copy-paste from Arduino Sd2Card Library
// Got tired of reading docs :(
typedef struct CSDV1 {
  // byte 0
  unsigned reserved1 : 6;
  unsigned csd_ver : 2;
  // byte 1
  uint8_t taac;
  // byte 2
  uint8_t nsac;
  // byte 3
  uint8_t tran_speed;
  // byte 4
  uint8_t ccc_high;
  // byte 5
  unsigned read_bl_len : 4;
  unsigned ccc_low : 4;
  // byte 6
  unsigned c_size_high : 2;
  unsigned reserved2 : 2;
  unsigned dsr_imp : 1;
  unsigned read_blk_misalign :1;
  unsigned write_blk_misalign : 1;
  unsigned read_bl_partial : 1;
  // byte 7
  uint8_t c_size_mid;
  // byte 8
  unsigned vdd_r_curr_max : 3;
  unsigned vdd_r_curr_min : 3;
  unsigned c_size_low :2;
  // byte 9
  unsigned c_size_mult_high : 2;
  unsigned vdd_w_cur_max : 3;
  unsigned vdd_w_curr_min : 3;
  // byte 10
  unsigned sector_size_high : 6;
  unsigned erase_blk_en : 1;
  unsigned c_size_mult_low : 1;
  // byte 11
  unsigned wp_grp_size : 7;
  unsigned sector_size_low : 1;
  // byte 12
  unsigned write_bl_len_high : 2;
  unsigned r2w_factor : 3;
  unsigned reserved3 : 2;
  unsigned wp_grp_enable : 1;
  // byte 13
  unsigned reserved4 : 5;
  unsigned write_partial : 1;
  unsigned write_bl_len_low : 2;
  // byte 14
  unsigned reserved5: 2;
  unsigned file_format : 2;
  unsigned tmp_write_protect : 1;
  unsigned perm_write_protect : 1;
  unsigned copy : 1;
  unsigned file_format_grp : 1;
  // byte 15
  unsigned always1 : 1;
  unsigned crc : 7;
}csd1_t;
//------------------------------------------------------------------------------
// CSD for version 2.00 cards
typedef struct CSDV2 {
  // byte 0
  unsigned reserved1 : 6;
  unsigned csd_ver : 2;
  // byte 1
  uint8_t taac;
  // byte 2
  uint8_t nsac;
  // byte 3
  uint8_t tran_speed;
  // byte 4
  uint8_t ccc_high;
  // byte 5
  unsigned read_bl_len : 4;
  unsigned ccc_low : 4;
  // byte 6
  unsigned reserved2 : 4;
  unsigned dsr_imp : 1;
  unsigned read_blk_misalign :1;
  unsigned write_blk_misalign : 1;
  unsigned read_bl_partial : 1;
  // byte 7
  unsigned reserved3 : 2;
  unsigned c_size_high : 6;
  // byte 8
  uint8_t c_size_mid;
  // byte 9
  uint8_t c_size_low;
  // byte 10
  unsigned sector_size_high : 6;
  unsigned erase_blk_en : 1;
  unsigned reserved4 : 1;
  // byte 11
  unsigned wp_grp_size : 7;
  unsigned sector_size_low : 1;
  // byte 12
  unsigned write_bl_len_high : 2;
  unsigned r2w_factor : 3;
  unsigned reserved5 : 2;
  unsigned wp_grp_enable : 1;
  // byte 13
  unsigned reserved6 : 5;
  unsigned write_partial : 1;
  unsigned write_bl_len_low : 2;
  // byte 14
  unsigned reserved7: 2;
  unsigned file_format : 2;
  unsigned tmp_write_protect : 1;
  unsigned perm_write_protect : 1;
  unsigned copy : 1;
  unsigned file_format_grp : 1;
  // byte 15
  unsigned always1 : 1;
  unsigned crc : 7;
}csd2_t;
//------------------------------------------------------------------------------
// union of old and new style CSD register
union csd_t {
  csd1_t v1;
  csd2_t v2;
};

const uint8_t CMD_INIT = 0;
const uint8_t CMD_SPI_MODE = 1;
const uint8_t CMD_GET_CSD = 9;
const uint8_t CMD_SET_SECTOR_SZ = 16;
const uint8_t CMD_READ_BLOCK = 17;
const uint8_t CMD_WRITE_BLOCK = 24;

namespace device
{

union 
{
struct
{
   uint8_t cmd;
   char parameters[4];
   uint8_t crc;
};
char cmdBuffer[6];
}spiCmd;

#define SDCARD_SECTOR_SIZE 512
char sector_buf[SDCARD_SECTOR_SIZE];

void printBuf(const char *buf, uint8_t len)
{
   for (uint8_t i =0; i < len; i++)
   {
      serial << buf[i] << " ";
   }
   serial << "\n";
}

#define DATA_READY_RESP 0xfe
#define NOT_READY_BYTE 0xff

char SDCard::WaitForResponse(int max)
{
   char resp;
   while(--max >= 0)
   {
      resp = m_spi.ExchangeChar(NOT_READY_BYTE);
      if (resp != NOT_READY_BYTE)
         return resp;
   }
   return NOT_READY_BYTE;
}

bool SDCard::WaitByte(char byte, int max)
{
   while(--max >= 0)
   {
      if (m_spi.ExchangeChar(NOT_READY_BYTE) == byte)
         return true;
   }
   return false;
}

uint32_t SDCard::GetSize()
{
   SendCommand(CMD_GET_CSD, 0);
   auto resp = WaitForResponse();
   if (!WaitByte(DATA_READY_RESP))
      serial << "error reading csd\n";
   memset(sector_buf, 0xff, 16);
   m_spi.TransferBytes(sector_buf, 16);
   csd_t *csd = (csd_t*)sector_buf;

  if (csd->v1.csd_ver == 0) {
    uint8_t read_bl_len = csd->v1.read_bl_len;
    uint16_t c_size = (csd->v1.c_size_high << 10)
                      | (csd->v1.c_size_mid << 2) | csd->v1.c_size_low;
    uint8_t c_size_mult = (csd->v1.c_size_mult_high << 1)
                          | csd->v1.c_size_mult_low;
    return (uint32_t)(c_size + 1) << (c_size_mult + read_bl_len - 7);
  } else if (csd->v2.csd_ver == 1) {
    uint32_t c_size = ((uint32_t)csd->v2.c_size_high << 16)
                      | (csd->v2.c_size_mid << 8) | csd->v2.c_size_low;
    return (c_size + 1) << 10;
  } else {
    return 0;
  }
}

void SDCard::Init()
{
   m_spi.SetControlPin();
   *m_cs = true;
   for (auto i = 0; i < 10; ++i )
      m_spi.ExchangeChar(0xff);

   *m_cs = false;
   SendCommand(CMD_INIT, 0);
   auto resp = WaitForResponse();
   
   serial <<"cmd: " << 0 << " responce: " << resp << "\n";
   SendCommand(CMD_SPI_MODE, 0);
   resp = WaitForResponse();
   serial <<"cmd: " << 1 << " responce: " << resp << "\n";
   *m_cs = true;
   *m_cs = false;
   SendCommand(CMD_SET_SECTOR_SZ, SDCARD_SECTOR_SIZE);
   resp = WaitForResponse();
   serial <<"cmd: " << 16 << " responce: " << resp << "\n";

   serial << "size: " << GetSize() << "\n";
   // printBuf(sector_buf, 16);
   // resp = WaitForResponse();
   // serial <<"cmd: " << 17 << " responce: " << resp << "\n";
   // resp = WaitForResponse();
   // serial <<"cmd: " << 17 << " responce: " << resp << "\n";
   *m_cs = true;
}

void SDCard::WriteSector(uint16_t sector)
{

}

void SDCard::ReadSector(int16_t sector)
{

}

void SDCard::SendCommand(uint8_t cmd, uint32_t parameters)
{
   spiCmd.cmd = 0x40 | cmd;
   spiCmd.parameters[0] = parameters >> 24 & 0xff;
   spiCmd.parameters[1] = parameters >> 16 & 0xff;
   spiCmd.parameters[2] = parameters >> 8 & 0xff;
   spiCmd.parameters[3] = parameters & 0xff;

   spiCmd.crc = cmd == 0 ? 0x95 : 0xff;
   serial << cmd << ": "  << spiCmd.cmdBuffer[1] << " " << spiCmd.cmdBuffer[2] << " "<< spiCmd.cmdBuffer[3] << " " << spiCmd.cmdBuffer[4] << "\n";
   m_spi.TransferBytes(spiCmd.cmdBuffer, 6);
   // *m_cs = true;
}

}
