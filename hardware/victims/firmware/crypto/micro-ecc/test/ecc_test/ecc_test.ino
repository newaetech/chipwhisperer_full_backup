#include <uECC.h>

#include <j0g.h>
#include <js0n.h>

#include <lwm.h>

#include <bitlash.h>

#include <GS.h>

#include <SPI.h>
#include <Wire.h>
#include <Scout.h>
#include <Shell.h>


#include <uECC.h>

extern "C" {

static int RNG(uint8_t *p_dest, unsigned p_size)
{
  // Use the least-significant bits from the ADC for an unconnected pin (or connected to a source of random noise)
  // This can take a long time to generate random data if the result of analogRead(0) doesn't change very frequently.
  while(p_size) {
    uint8_t l_val = 0;
    for(unsigned i=0; i<8; ++i)
    {
      int l_init = analogRead(0);
      int l_count = 0;
      while(analogRead(0) == l_init)
      {
        ++l_count;
      }
      
      if(l_count == 0)
      {
         l_val = (l_val << 1) | (l_init & 0x01);
      }
      else
      {
         l_val = (l_val << 1) | (l_count & 0x01);
      }
    }
    *p_dest = l_val;
    ++p_dest;
    --p_size;
  }
  
  // NOTE: it would be a good idea to hash the resulting random data using SHA-256 or similar.
  return 1;
}

}

void setup()
{
  Scout.setup();
  
  Serial.print("Testing ecc\n");
  
  uECC_set_rng(&RNG);
}

void loop() {
  uint8_t l_private1[uECC_BYTES];
  uint8_t l_private2[uECC_BYTES];
  
  uint8_t l_public1[uECC_BYTES * 2];
  uint8_t l_public2[uECC_BYTES * 2];
  
  uint8_t l_secret1[uECC_BYTES];
  uint8_t l_secret2[uECC_BYTES];
  
  unsigned long a = millis();
  uECC_make_key(l_public1, l_private1);
  unsigned long b = millis();
  
  Serial.print("Made key 1 in "); Serial.println(b-a);
  a = millis();
  uECC_make_key(l_public2, l_private2);
  b = millis();
  Serial.print("Made key 2 in "); Serial.println(b-a);

  a = millis();
  int r = uECC_shared_secret(l_public2, l_private1, l_secret1);
  b = millis();
  Serial.print("Shared secret 1 in "); Serial.println(b-a);
  if(!r)
  {
    Serial.print("shared_secret() failed (1)\n");
    return;
  }

  a = millis();
  r = uECC_shared_secret(l_public1, l_private2, l_secret2);
  b = millis();
  Serial.print("Shared secret 2 in "); Serial.println(b-a);
  if(!r)
  {
    Serial.print("shared_secret() failed (2)\n");
    return;
  }
    
  if(memcmp(l_secret1, l_secret2, sizeof(l_secret1)) != 0)
  {
    Serial.print("Shared secrets are not identical!\n");
  }
  else
  {
    Serial.print("Shared secrets are identical\n");
  }
}

