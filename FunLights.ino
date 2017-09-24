// NeoPixelFunRandomChange
// This example will randomly select a number pixels and then
// start an animation to blend them from their current color to
// randomly selected a color
// modified to make lights transition to red as bedtime approaches.


#include <NtpClientLib.h>
#include <ESP8266WiFi.h>          
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>          
#include <ESP8266WebServer.h> 
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>


#define colorSaturation 128
#define DST_TIMEZONE_OFFSET -4    // Day Light Saving Time offset 
#define  ST_TIMEZONE_OFFSET  -5    // Standard Time offset 


//US Eastern Time Zone (New York, Boston)


const char* ssid = "ssid";
const char* password = "password";
const uint16_t PixelCount = 150; // make sure to set this to the number of pixels in your strip
const uint8_t PixelPin = 2;  // make sure to set this to the correct pin, ignored for Esp8266
const int bedtime = 22;
const int waketime = 7;


RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

HslColor hslRed(red);
HslColor hslGreen(green);
HslColor hslBlue(blue);
HslColor hslWhite(white);
HslColor hslBlack(black);


//NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
// For Esp8266, the Pin is omitted and it uses GPIO3 due to DMA hardware use.  
// There are other Esp8266 alternative methods that provide more pin options, but also have
// other side effects.
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount);
//
// NeoEsp8266Uart800KbpsMethod uses GPI02 instead

NeoPixelAnimator animations(PixelCount); // NeoPixel animation management object

// what is stored for state is specific to the need, in this case, the colors.
// Basically what ever you need inside the animation update function
struct MyAnimationState
{
    RgbColor StartingColor;
    RgbColor EndingColor;
};

// one entry per pixel to match the animation timing manager
MyAnimationState animationState[PixelCount];

void SetRandomSeed()
{
    uint32_t seed;

    // random works best with a seed that can use 31 bits
    // analogRead on a unconnected pin tends toward less than four bits
    seed = analogRead(0);
    delay(1);

    for (int shifts = 3; shifts < 31; shifts += 3)
    {
        seed ^= analogRead(0) << shifts;
        delay(1);
    }

    // Serial.println(seed);
    randomSeed(seed);
}

// simple blend function
void BlendAnimUpdate(const AnimationParam& param)
{
    // this gets called for each animation on every time step
    // progress will start at 0.0 and end at 1.0
    // we use the blend function on the RgbColor to mix
    // color based on the progress given to us in the animation
    RgbColor updatedColor = RgbColor::LinearBlend(
        animationState[param.index].StartingColor,
        animationState[param.index].EndingColor,
        param.progress);
    // apply the color to the strip
    strip.SetPixelColor(param.index, updatedColor);
}

void PickRandom(float luminance)
{
    // pick random count of pixels to animate
    uint16_t count = random(PixelCount/2,PixelCount);
    while (count > 0)
    {
        // pick a random pixel
        uint16_t pixel = random(PixelCount);

        // pick random time and random color
        // we use HslColor object as it allows us to easily pick a color
        // with the same saturation and luminance 
        uint16_t time = random(1000, 5000);
        animationState[pixel].StartingColor = strip.GetPixelColor(pixel);
       // animationState[pixel].EndingColor = HslColor(random(360) / 360.0f, 1.0f, luminance);
       animationState[pixel].EndingColor = HslColor(random(360) / 360.0f, random(75,100)/100.0f, random(20,70)/360.0f);
      //  animationState[pixel].EndingColor = HslColor(40/240.0f, (random(0,80)/100.0f), random(40,360)/360.0f);
        animations.StartAnimation(pixel, time, BlendAnimUpdate);

        count--;
    }
}

void FlashRed(float luminance)
{
    // pick random count of pixels to animate
float numred = ((minute()/60.0))*((PixelCount-40.0)/2.0);
//numred = (int) (minute()/60.0)*((PixelCount-40)/2);
Serial.print("minute=");Serial.println(minute());
Serial.println((minute()/60.0));
Serial.println((PixelCount-40)/2.0);
Serial.print("numred=");Serial.println(numred);
Serial.print("bedtime=");Serial.println(bedtime);
if (hour()>=bedtime+1 || hour()<=waketime) {
  Serial.println("bedtime+1 routine");
  uint16_t count = random(PixelCount);

    while (count > 0)
    {
        // pick a random pixel
        uint16_t pixel = random(PixelCount);

        // pick random time and random color
        // we use HslColor object as it allows us to easily pick a color
        // with the same saturation and luminance 
        uint16_t time = random(1000, 5000);
        animationState[pixel].StartingColor = strip.GetPixelColor(pixel);
        animationState[pixel].EndingColor = HslColor(0.0f, 1.0f, random(5,30)/360.0f);
        animations.StartAnimation(pixel, time, BlendAnimUpdate);

        count--;
    }}
else{

uint16_t count = random(PixelCount);

    while (count > 0)
    {
        // pick a random pixel
        uint16_t pixel = random(PixelCount);

        // pick random time and random color
        // we use HslColor object as it allows us to easily pick a color
        // with the same saturation and luminance 
        uint16_t time = random(1000, 5000);
        animationState[pixel].StartingColor = strip.GetPixelColor(pixel);
        if (pixel<(20+numred) || pixel>(PixelCount-numred-20))    {
        animationState[pixel].EndingColor = HslColor(0.0f, 1.0f, random(0,30)/360.0f);
        }
        else
        {
          animationState[pixel].EndingColor = HslColor(random(360) / 360.0f, 1.0f, random(10,40)/360.0f);
        }
        animations.StartAnimation(pixel, time, BlendAnimUpdate);

        count--;
    }}
 }


void setup()
{
    Serial.begin(115200);
      // Connect to WiFi network
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
  
  // Start the server
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
 
   // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
 //  ArduinoOTA.setPassword((const char *)"1234");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
    NTP.begin("pool.ntp.org", -4, false); // get time from NTP server pool.  ntpServerName, int timeOffset, bool daylight
    NTP.setInterval(63);

    NTP.onNTPSyncEvent([](NTPSyncEvent_t error) {
        if (error) {
            Serial.print("Time Sync error: ");
            if (error == noResponse)
                Serial.println("NTP server not reachable");
                
            else if (error == invalidAddress)
                Serial.println("Invalid NTP server address");
        }
        else {
            Serial.print("Got NTP time: ");
            Serial.println(NTP.getTimeDateString(NTP.getLastNTPSync()));
        }
    });
   
        Serial.println(NTP.getTimeDateString()); Serial.print(" ");
        Serial.println(hour());
        Serial.println(NTP.isSummerTime() ? "Summer Time. " : "Winter Time. ");
        Serial.print("WiFi is ");
        Serial.print(WiFi.isConnected() ? "connected" : "not connected"); Serial.print(". ");
        Serial.print("Uptime: ");
        Serial.print(NTP.getUptimeString()); Serial.print(" since ");
        Serial.println(NTP.getTimeDateString(NTP.getFirstSync()).c_str());   

    strip.Begin();
    strip.Show();

    SetRandomSeed();
}


void loop()
{
  ArduinoOTA.handle();
   static int i = 0;
    static int last = 0;

    if ((millis() - last) > 5100) {
        //Serial.println(millis() - last);
        last = millis();
        Serial.print(i); Serial.print(" ");
        Serial.print(NTP.getTimeDateString()); Serial.print(". ");
        Serial.print("Uptime: ");
        Serial.print(NTP.getUptimeString()); Serial.print(" since ");
        Serial.println(NTP.getTimeDateString(NTP.getFirstSync()).c_str());
        Serial.print(hour());Serial.println("h");

        i++;
    }

    if (animations.IsAnimating())
    {
        // the normal loop just needs these two to run the active animations
        animations.UpdateAnimations();
        delay(50);
        
        strip.Show();
    }
    else
    {
        // no animations runnning, start some 
        //
        if (hour() >= bedtime || hour()<=waketime)    {
   FlashRed(0.2f); 

   }
    else
    {
    
        PickRandom(0.2f); // 0.0 = black, 0.25 is normal, 0.5 is bright
    }}
}


