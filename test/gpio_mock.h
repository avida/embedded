#include <gpio.h>
namespace gpio
{
class TestPinOutput: public IPinOutput
{
public:
TestPinOutput();
TestPinOutput& operator= (bool val);
operator bool() const;
private:
   bool m_state;
};
}