/*
 * Project SmartRoom-insurance
 * Description:
 * Author:
 * Date:
 */

const String accessToken = "ca7e850a1df7632615141b1c1e51c9de8e347da9";
String tempAccess = "https://api.particle.io/v1/devices/e00fce6879a39f9da7886944/temp?access_token=ca7e850a1df7632615141b1c1e51c9de8e347da9";

int i = 0;

void myHandler(const char *event, const char *data)
{
  i++;
  Serial.printf("%d: event=%s data=%s", i, event, (data ? data : "NULL"));
}


void setup() {
  Particle.connect();
  Particle.subscribe("tempC", myHandler);
//  Particle.function("getTemp", &getTemp);
}

void loop() {
//  Serial.printf("%f\n", tempC / 100.0);
  delay(20000);
}