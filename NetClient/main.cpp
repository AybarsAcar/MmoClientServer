#include <iostream>
#include <olc_net.h>

enum class CustomMsgTypes : uint32_t
{
  FireBullet,
  MovePlayer
};

class CustomClient : public olc::net::client_interface<CustomMsgTypes>
{
public:
  bool FireBullet(float x, float y)
  {
    olc::net::message<CustomMsgTypes> msg;
    msg.header.id = CustomMsgTypes::FireBullet;

    msg << x << y;

    Send(msg);
  }
};

int main()
{
  olc::net::message<CustomMsgTypes> msg;

  msg.header.id = CustomMsgTypes::FireBullet;

  int a = 1;
  bool b = true;
  float c = 3.14123f;

  struct
  {
    float x;
    float y;
  } d[5];

//  build a message
  msg << a << b << c << d;

  std::cout << msg << std::endl;

  a = 99;
  b = false;
  c = 99.0f;

//  extract from the message
  msg >> d >> c >> b >> a;


  CustomClient c;
  c.Connect("community.onelonecoder.com", 60000)
  c.FireBullet(2.0f, 5.0f);

  return 0;
}
