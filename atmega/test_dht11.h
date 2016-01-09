void test_main()
{
   sensors::DHT11 dht(gpio::B, 3);
   while (1)
   {
      if (dht.ReadData())
      {
         serial << "humidity: " << dht.GetHumidity()
                << "\n temp: "  << dht.GetTemperature()  << "\n";

      }
      else
      {
         serial << "error reading sensor data\n";
      }
      _delay_ms(5000);
   }
}