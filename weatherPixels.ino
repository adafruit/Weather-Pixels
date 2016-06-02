
#include <ESP8266WiFi.h>
void animSetup(void);
void animConfig(uint16_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void waitForFrame(void);
void renderFrame(void);



const char* ssid     = "adafruit";
const char* password = "ffffffff";

const char* host = "query.yahooapis.com";
const int httpPort = 80;

const char* location = "boston%2C%20ma";
const char* path_prefix = "/v1/public/yql?q=select%20item.condition.code%2C%20item.condition.text%20%20from%20weather.forecast%20where%20woeid%20in%20(select%20woeid%20from%20geo.places(1)%20where%20text%3D%22";
const char* path_postfix = "%22)&format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys";

int16_t weathercode = -1;
  
void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  animConfig(0, 0, 0, 0, 0, 0);
  animSetup();
}

uint32_t timekeep=0xFFFF;

void loop() {
  uint32_t currTime = millis();
  // every 30 seconds (or if there's a rollover/first time running, update the weather!
  if ((timekeep > currTime)  || (currTime > (timekeep + 10000))) {
    timekeep = currTime;
    updateWeather();
  }

  waitForFrame();
  renderFrame();

}


void updateWeather() {
  
  Serial.print("Connecting to "); Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return;
  }
  
  // We now create a URI for the request

  String url = String(path_prefix) + String(location) + String(path_postfix);
  
  Serial.print("Requesting URL: ");  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(100);

  weathercode = -1;
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    int i = line.indexOf(String("\"code\":"));
    if (i < 0) continue;
    Serial.println(line);
    weathercode = (line.substring(i+8)).toInt();
  }
  
  Serial.println("Closing connection");

  Serial.print("Weather code: "); Serial.println(weathercode);

 /* void animConfig(
 uint16_t t,   // Time of day in fixed-point 16-bit units, where 0=midnight,
               // 32768=noon, 65536=midnight. THIS DOES NOT CORRESPOND TO
               // ANY SORT OF REAL-WORLD UNITS LIKE SECONDS, nor does it
               // handle things like seasons or Daylight Saving Time, it's
               // just an "ish" approximation to give the sky animation some
               // vague context. The time of day should be polled from the
               // same source that's providing the weather data, DO NOT use
               // millis() or micros() to attempt to follow real time, as
               // the NeoPixel library is known to mangle these interrupt-
               // based functions. TIME OF DAY IS "ISH!"
 uint8_t  c,   // Cloud cover, as a percentage (0-100).
 uint8_t  r,   // Rainfall as a "strength" value (0-255) that doesn't really
               // correspond to anything except "none" to "max."
 uint8_t  s,   // Snowfall, similar "strength" value (0-255).
 uint8_t  l,   // Lightning, ditto.
 uint8_t  w) { // Wind speed as a "strength" value (0-255) that also doesn't
               // correspond to anything real; this is the number of fixed-
               // point units that the clouds will move per frame. There are
               // 65536 units around the 'sky,' so a value of 255 will take
               // about 257 frames to make a full revolution of the LEDs,
               // which at 50 FPS would be a little over 5 seconds.
 **************************/
  int16_t timeofday = 32768;
  weathercode = 41;
  switch (weathercode) {
    case 0: // tornado!
      Serial.println("tornado");
      // lotsa cloud, no rain, and wind!
      animConfig(timeofday, 100, 0, 0, 0, 255);
      break;
    case 1: // tropical storm
      Serial.println("tropical storm");
      // no cloud, lots of rain and wind!
      animConfig(timeofday, 0, 255, 0, 0, 255);
      break;
    case 2: // hurricane
      Serial.println("hurricane");
      // some cloud, some rain and wind!
      animConfig(timeofday, 50, 100, 0, 0, 255);
      break;

    case 3: // severe thunder
      Serial.println("severe thunder");
      // some cloud, some rain, mega lightning, some wind!
      animConfig(timeofday, 20, 30, 0, 255, 50);
      break;

    case 4: // thunder
      Serial.println("thunder");
      // some cloud, some rain, some lightning, some wind!
      animConfig(timeofday, 20, 30, 0, 100, 50);
      break;

    case 5: // mixed rain + snow
    case 6: // mixed rain and sleet
    case 7: // mixed snow and sleet
      Serial.println("Rain/Snow/Sleet");
      // some cloud, some rain, some snow, no lightning, no wind!
      animConfig(timeofday, 10, 100, 100, 0, 0);
      break;

    case 8: // freezing drizzle
    case 9: // drizzle
      Serial.println("Drizzle");
      // some cloud, a little rain, no snow, no lightning, no wind!
      animConfig(timeofday, 30, 70, 0, 0, 0);
      break;

    case 10: // freezing rain
    case 11: // showers
    case 12: // showers
      Serial.println("Rain/Showers");
      // some cloud, lotsa rain, no snow, no lightning, no wind!
      animConfig(timeofday, 30, 250, 0, 0, 0);
      break;

    case 13: // snow flurries
    case 14: // light snow showers
      Serial.println("flurries");
      // some cloud, no rain, some snow, no lightning, no wind!
      animConfig(timeofday, 30, 0, 100, 0, 0);
      break;
      
    case 15: // blowing snow
      Serial.println("blowing snow");
      // some cloud, no rain, snow, no lightning, lotsa wind!
      animConfig(timeofday, 30, 0, 150, 0, 200);
      break;

    case 16: // snow
    case 42: // scattered snow showers
      Serial.println("snow");
      // some cloud, no rain, snow, no lightning, no wind!
      animConfig(timeofday, 30, 0, 150, 0, 10);
      break;
      
    case 41: // heavy snow
    case 43: // heavy snow
      Serial.println("heavy snow");
      // some cloud, no rain, lotsa snow, no lightning, no wind!
      animConfig(timeofday, 30, 0, 255, 0, 0);
  Serial.println("configd");
      break;

    case 31: // clear (night)
      Serial.println("Clear night");
      break;
    case 32: // sunny
    case 33: // fair (night)
    case 34: // fair (day)

    default:
      break;
  }
/*
17  hail
18  sleet
19  dust
20  foggy
21  haze
22  smoky
23  blustery
24  windy
25  cold
26  cloudy
27  mostly cloudy (night)
28  mostly cloudy (day)
29  partly cloudy (night)
30  partly cloudy (day)
35  mixed rain and hail
36  hot
37  isolated thunderstorms
38  scattered thunderstorms
39  scattered thunderstorms
40  scattered showers
44  partly cloudy
45  thundershowers
46  snow showers
47  isolated thundershowers
3200  not available
*/
}
