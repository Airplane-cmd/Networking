#include "net_message.hpp"
#include "net_common.hpp"
#include <iostream>
enum class CustomMsgTypes : uint32_t
{
  TypeOne,
  TypeTwo
};
int main()
{
  web::net::message<CustomMsgTypes> msg;
  msg.header.id = CustomMsgTypes::TypeOne;
  uint8_t a = 1;
  bool b = 0;
  float c = 3.14159f;
  struct
  {
    float x;
    float y;
  } d[5];
  msg << a << b << c << d;
   
  a = 99;
  b = 1;
  c = 32.9f;

  msg >> d >> c >> b >> a;
  return 0;
}
