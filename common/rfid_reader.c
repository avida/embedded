#include <rfid_reader.h>

#include <utils.h>
#include <string.h>

#include <uart.h>
extern uart::UART serial;

#define CommandReg 0x01
#define FIFODataReg 0x09
#define FIFOLevelReg 0x0a
#define AutoTestReg 0x36
#define VersionReg 0x37
#define TestADCReg 0x3b
#define AnalogTestReg 0x38
#define TModeReg 0x2a
#define TPrescalerReg 0x2b
#define  TReloadRegH 0x2c
#define TReloadRegL 0x2d
#define TxASKReg 0x15
#define DivIrqReg 0x05

#define CMD_IDLE 0
#define CMD_MEM 0x01
#define CMD_CALC_CRC 0x03
#define CMD_SOFT_RESET 0x0f

namespace device
{

char RFIDReader::rfid_buffer[BUFFER_SIZE + 1] = {0x00};

RFIDReader::RFIDReader(protocol::SPI& spi, gpio::IPinOutput& reset): m_spi(spi), m_reset(reset)
{
}

void RFIDReader::Init()
{
   m_reset = true;
   utils::Delay_ms(50);
   WriteRegister(TModeReg, 0x80);
   WriteRegister(TPrescalerReg, 0xa9);
   WriteRegister(TReloadRegH, 0x03);
   WriteRegister(TReloadRegL, 0xe8);
}

void RFIDReader::SelfTest()
{

   SendCommand(CMD_SOFT_RESET);
   utils::Delay_ms(50);
   while(ReadRegister(CommandReg) & (1<<4));

   serial << "start test \n";
   // flash FIFO
   WriteRegister(FIFOLevelReg,  1<<7);

   memset(rfid_buffer + 1, 0, 25);
   WriteFIFO(25);
   SendCommand(CMD_MEM);
   WriteRegister(AutoTestReg, 0x09);
   WriteRegister(FIFODataReg, 0);

   SendCommand(CMD_CALC_CRC);

   for (auto i =0; i < 0xff; ++i)
   {
      if (ReadRegister(DivIrqReg) & 0x04)
         break;
   }
   SendCommand(CMD_IDLE);

   ReadFIFO(25);
   for (auto i = 0; i < 25; ++i )
   {
      serial <<" " << rfid_buffer[i+1];
   }
   serial << "\n";
   WriteRegister(AutoTestReg, 0);
}  

char RFIDReader::Version()
{
   return ReadRegister(VersionReg);
}

void  RFIDReader::EnableRegister(char address, bool write)
{
   char command = write ? 0 : 1 << 7;
   command |= address << 1;
   m_spi.SendByte(command);
}

void RFIDReader::SendCommand(char cmd)
{
   WriteRegister(CommandReg, cmd);
}

void RFIDReader::WriteRegister(char reg, char data)
{ 
   char command = 0;
   command |= reg << 1;
   rfid_buffer[0] = command;
   rfid_buffer[1] = data;
   m_spi.TransferBytes(rfid_buffer, 2);
}

char RFIDReader::ReadRegister(char reg)
{
   char command = 1 << 7;
   command |= reg << 1;
   rfid_buffer[0] = command;
   m_spi.TransferBytes(rfid_buffer, 2);
   return rfid_buffer[1];
}

void RFIDReader::ReadFIFO(int bytes)
{
   char command = 1 << 7;
   command |= FIFODataReg << 1;
   //rfid_buffer[0] = command;
   memset(rfid_buffer , command, bytes + 1);
   m_spi.TransferBytes(rfid_buffer, bytes + 1);
}

void RFIDReader::WriteFIFO(int bytes)
{
   char command = 0;
   command |= FIFODataReg << 1;
   rfid_buffer[0] = command;
   m_spi.TransferBytes(rfid_buffer, bytes + 1);
}


} // namespace
