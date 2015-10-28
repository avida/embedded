#pragma ince

namespace protocol
{
   
class SPI
{
public:
   SPI(bool master);
   void SetMaster(bool master);
   char SendReceiveByte(char* byte);
};
}