#include <iostream>
#include <stdexcept>
#include <bcm2835.h>
#include "gpio_rpi.h"
#include <device/NRF24L01.h>
#include <boost/format.hpp>
#include <sys/epoll.h>
#include <fcntl.h>

void init_hw()
{
   if(!bcm2835_init())
   {
      throw std::runtime_error("Cannot init bmc library");
   }
}

#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO_UNEXPORT "/sys/class/gpio/unexport"
namespace GPIO_EDGES
{
   const std::string none = "none";
   const std::string both = "both";
   const std::string rising = "rising";
   const std::string falling = "falling";
};

class GPIO
{
public:
GPIO(uint gpio): m_gpio(gpio)
{
   auto exp_fd = open(GPIO_EXPORT, O_WRONLY);
   if (exp_fd < 0)
   {
      throw std::runtime_error("cannot open gpio port");
   }
   auto gpio_str = std::to_string(m_gpio);
   write(exp_fd, gpio_str.c_str(), gpio_str.size());

   auto filename = (boost::format("/sys/class/gpio/gpio%d/value" ) % m_gpio).str();
   m_fd = open(filename.c_str(), O_RDONLY | O_NONBLOCK );
   if (m_fd < 0)
   {
      throw std::runtime_error("error opening file\n");
   }
   m_epfd = epoll_create(1);
   if (m_epfd < 0)
   {
      throw std::runtime_error("Error creating epoll");
   }
   m_ev.events = EPOLLIN | EPOLLET | EPOLLPRI;
   m_ev.data.fd = m_fd;
   if (epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_fd, &m_ev) == -1)
   {
      throw std::runtime_error("Error adding event to epoll");
   }
}

~GPIO()
{
   auto exp_fd = open(GPIO_UNEXPORT, O_WRONLY);
   auto gpio_str = std::to_string(m_gpio);
   write(exp_fd, gpio_str.c_str(), gpio_str.size());
   std::cout << "bb\n";
}

void set_edge(const std::string& edge)
{
   auto fname = (boost::format("/sys/class/gpio/gpio%d/edge") % m_gpio).str();
   auto fd = open(fname.c_str(), O_WRONLY);
   if (fd < 0)
   {
      throw std::runtime_error("cannot open file " + fname);
   }
   write(fd, edge.c_str(), edge.size());
}

void wait_for_edge(const std::string& edge)
{
   set_edge(edge);
   while(true)
   {
      auto events = epoll_wait(m_epfd, &m_events, 1, -1);
      std::cout << events<< std::endl;
   }

}

protected:
   uint m_gpio;
   int m_fd;
   struct epoll_event m_events, m_ev;
   int m_epfd;
};

void test_gpio_irq(uint gpio)
{
   GPIO gp(gpio);
   gp.wait_for_edge(GPIO_EDGES::rising);
}

int main()
{
try
{
//   test_gpio_irq(22);
//   return 0;
   init_hw();
   gpio::rpi::Pin pin(RPI_GPIO_P1_18);
   gpio::rpi::Pin ce(RPI_GPIO_P1_16);
   gpio::rpi::Pin spi_pin(RPI_GPIO_P1_24);
   protocol::SPI spi(&spi_pin);
   device::NRF24L01 nrf(spi, ce, 5);
   char *buf = nrf.GetBufferPtr();
   while(1)
   {
      auto status = nrf.ReadStatus();
      nrf.Listen();
      do 
      {
         status = nrf.ReadStatus();
      } while(!status.isReceived());
      std::cout << (int)nrf.ReadStatus().GetStatus() << std::endl;
      std::cout << "Receive" << std::endl;
      status = nrf.Receive();
      nrf.StandBy();
      std::cout << buf << "\n";
      nrf.SendString("PONG");
   }
}
catch(const std::runtime_error& e)
{
   std::cout << "Error occured: " << e.what() << std::endl;
}
return 0;
}
