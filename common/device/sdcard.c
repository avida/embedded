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
const uint8_t CMD_CHECK_VER = 8;
const uint8_t CMD_GET_CSD = 9;
const uint8_t CMD_SET_SECTOR_SZ = 16;
const uint8_t CMD_READ_BLOCK = 17;
const uint8_t CMD_WRITE_BLOCK = 24;

const uint8_t R1_RDY_STATE = 0X00;
const uint8_t R1_IDLE_STATE = 0X01;
const uint8_t R1_ILLEGAL_COMMAND = 0X04;
const uint8_t DATA_START_BLOCK = 0XFE;

const uint8_t NOT_READY_BYTE  = 0xFF;

const uint8_t DATA_RES_MASK = 0X1F;
const uint8_t DATA_RES_ACCEPTED = 0X05;
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

char * SDCard::GetBufferPtr()
{
   return sector_buf;
}

char SDCard::WaitForResponse(int max)
{
   char resp;
   while(--max >= 0)
   {
      resp = m_spi.ExchangeChar(NOT_READY_BYTE);
      // serial << "wresp:" << resp << "\n";
      if (resp != NOT_READY_BYTE)
         return resp;
   }
   return NOT_READY_BYTE;
}

bool SDCard::WaitByte(char byte, int max)
{
   while(--max >= 0)
   {
      auto bt = m_spi.ExchangeChar(NOT_READY_BYTE);
      // serial << "bt: " << bt << "\n";
      if (bt == byte)
         return true;
   }
   return false;
}

uint32_t SDCard::GetSize()
{
   auto resp = SendCommand(CMD_GET_CSD, 0);
   if (!WaitByte(DATA_START_BLOCK))
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

bool SDCard::RepeateCmdUntil(uint8_t cmd, uint32_t params, uint8_t resp, int max)
{
   return 0;
}

void SDCard::Init()
{
   m_spi.SetControlPin();
   *m_cs = true;
   for (auto i = 0; i < 10; ++i )
      m_spi.ExchangeChar(0xff);

   auto cnt = 0;

   while(SendCommand(CMD_INIT, 0) != R1_IDLE_STATE && ++cnt < 100);
      
   auto resp = SendCommand(CMD_CHECK_VER, 0x1aa);
   if (resp == R1_ILLEGAL_COMMAND)
   {
      m_type = SD1;
   }
   else
   {
      for (uint8_t i = 0; i < 3; i++)m_spi.ExchangeChar(0xff);
      resp = m_spi.ExchangeChar(0xff);
      if (resp != 0xaa)
      {
         serial <<resp <<  " Unknown sdcard type\n";
         m_type = Unknown;
         return;
      }
      m_type = SD2;
   }
   // resp = SendCommand(CMD_SET_SECTOR_SZ, SDCARD_SECTOR_SIZE);
   cnt = 0;
   while(SendACommand(41, m_type == SD2 ?  0X40000000 : 0) != 0x00)
   {
      if (++cnt > 255)
      {
         serial << "Failed to init card\n";
         m_type = Unknown;
         return;
      }
   }
   if (m_type == SD2)
   {
      if (SendCommand(58, 0) != 0)
      {
         serial << "Error: " <<__LINE__ << "\n";
         m_type = Unknown;
         return;
      }
      resp = m_spi.ExchangeChar(0xff);
      if (resp == 0xc0)
      {
         // serial << "SDHc\n";
         m_type = SDHC;
      }
      for (uint8_t i = 0; i < 3; i++) m_spi.ExchangeChar(0xff);
      
   }
 
   // SendCommand(CMD_SET_SECTOR_SZ, SDCARD_SECTOR_SIZE);
   
   serial << "size: " << GetSize() << "\n";
   *m_cs = true;
}

bool SDCard::WriteSector(uint32_t sector)
{
   if (m_type != SDHC)
      sector *= SDCARD_SECTOR_SIZE;
   if (SendCommand(CMD_WRITE_BLOCK, sector))
   {
      serial << "error writing data\n";
      *m_cs = true;
      return false;
   }
   m_spi.ExchangeChar(DATA_START_BLOCK);
   m_spi.TransferBytes(sector_buf, SDCARD_SECTOR_SIZE);
   // dummy crc
   m_spi.ExchangeChar(0xff);
   m_spi.ExchangeChar(0xff);
   auto resp = m_spi.ExchangeChar(0xff);
   if ( (resp & DATA_RES_MASK) != DATA_RES_ACCEPTED)
   {
      serial << "write resp: " << resp << "\n";
      *m_cs = true;
      return false;
   }

   if (WaitByte(NOT_READY_BYTE))
   {
      *m_cs = true;
      serial << "error waiting responce\n";
      return false;
   }
   *m_cs = true;

}

void SDCard::ReadSector(int16_t sector)
{
   uint32_t params = sector;
   if (m_type != SDHC)
      params *= SDCARD_SECTOR_SIZE;
   SendCommand(CMD_READ_BLOCK, params);

   if (!WaitByte(DATA_START_BLOCK,250))
      serial << "Error reading data\n";
   
   memset(sector_buf, 0xff, 20);
   m_spi.TransferBytes(sector_buf, 20);
   printBuf(sector_buf, 20);
}

char SDCard::SendCommand(uint8_t cmd, uint32_t parameters)
{
   *m_cs = false;
   spiCmd.cmd = 0x40 | cmd;
   spiCmd.parameters[0] = parameters >> 24 & 0xff;
   spiCmd.parameters[1] = parameters >> 16 & 0xff;
   spiCmd.parameters[2] = parameters >> 8 & 0xff;
   spiCmd.parameters[3] = parameters & 0xff;
   if (!WaitByte(0xff))
      return 0xff;
   switch (cmd)
   {
      case CMD_INIT:
         spiCmd.crc = 0x95;
         break;
      case CMD_CHECK_VER:
         spiCmd.crc = 0x87;
         break;
      default:
         spiCmd.crc = 0xff;
   }
   m_spi.TransferBytes(spiCmd.cmdBuffer, 6);
   auto resp = WaitForResponse();
   // serial << "cmd: " << cmd << "\n";
   // serial << "resp: " << resp << "\n";
   return resp;
   // *m_cs = true;
}
char SDCard::SendACommand(uint8_t cmd, uint32_t params)
{
   SendCommand(55,0);
   return SendCommand(cmd, params);
}

}
