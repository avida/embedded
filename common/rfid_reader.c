#include <rfid_reader.h>

#include <avr/delay.h>

#include <uart.h>
extern uart::UART serial;

#define CommandReg 0x01
#define FIFODataReg 0x09
#define AutoTestReg 0x36
#define VersionReg 0x37
#define TestADCReg 0x3b
#define AnalogTestReg 0x38
#define FIFOLevelReg 0x0a
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

RFIDReader::RFIDReader(protocol::SPI& spi, gpio::IPinOutput& reset): m_spi(spi), m_reset(reset)
{
}

void RFIDReader::Init()
{
   m_reset = false;
   m_reset = true;
   _delay_ms(50);
   WriteRegister(TModeReg, 0x80);
   WriteRegister(TPrescalerReg, 0xa9);
   WriteRegister(TReloadRegH, 0x03);
   WriteRegister(TReloadRegL, 0xe8);
}

void RFIDReader::SelfTest()
{
   SendCommand(CMD_SOFT_RESET);
   _delay_ms(50);
   while(ReadRegister(CommandReg) & (1<<4));
   serial << "start test \n";
   // flash FIFO
   WriteRegister(FIFOLevelReg, 1<<7);

   for (auto i=0; i < 25; ++i)
   {
      WriteRegister(FIFODataReg, 0);
   }
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
   for (auto i = 0; i < 25; ++i )
   {
      serial <<" " << ReadRegister(FIFODataReg);
   }
   serial << "\n";
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
   EnableRegister(CommandReg,true);
   char command = 1 << 5;
   command |= cmd;
   m_spi.SendByte(command);
}

void RFIDReader::WriteRegister(char reg, char data)
{ 
   EnableRegister(reg, true);
   m_spi.SendByte(data);
}

char RFIDReader::ReadRegister(char reg)
{
   EnableRegister(reg, false);
   char bt = m_spi.ReceiveByte();
   return bt;
}

} // namespace
