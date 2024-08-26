#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME "Fall detection"
#define BLYNK_AUTH_TOKEN ""



#include <PulseSensorPlayground.h>     
const int PulseWire = 36;      
int Threshold = 2000;          
                               
PulseSensorPlayground pulseSensor;

Adafruit_MPU6050 mpu;

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "";
char pass[] = "";

BlynkTimer timer;
void sendSensor()
{
  static unsigned long lastFallTime = 0;
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  int myBPM = pulseSensor.getBeatsPerMinute();

  // Calculate jerk
  float currentAccX = a.acceleration.x;
  float currentAccY = a.acceleration.y;
  float currentAccZ = a.acceleration.z;

  static float prevAccX = 0;
  static float prevAccY = 0;
  static float prevAccZ = 0;

  float jerkX = currentAccX - prevAccX;
  float jerkY = currentAccY - prevAccY;
  float jerkZ = currentAccZ - prevAccZ;

  float fall_meter = 2*sqrt(jerkX*jerkX) + 2*sqrt(jerkY*jerkY) + 5*sqrt(jerkZ*jerkZ);

  prevAccX = currentAccX;
  prevAccY = currentAccY;
  prevAccZ = currentAccZ;

  /* Print out the values */
  Serial.print("fall_meter:");
  Serial.print(fall_meter);
  Serial.print(",");
  Serial.print("BPM:");
  Serial.print(myBPM);
  Serial.print(",");
  Serial.print("AccelX:");
  Serial.print(a.acceleration.x);
  Serial.print(",");
  Serial.print("AccelY:");
  Serial.print(a.acceleration.y);
  Serial.print(",");
  Serial.print("AccelZ:");
  Serial.print(a.acceleration.z);
  Serial.print(", ");
  Serial.print("GyroX:");
  Serial.println(g.gyro.x);

  

  // You can send any value at any time.
  // Please don't send more than 10 values per second.
  Blynk.virtualWrite(V0, a.acceleration.x);
  Blynk.virtualWrite(V1, a.acceleration.y);
  Blynk.virtualWrite(V2, a.acceleration.z);
  Blynk.virtualWrite(V3, fall_meter);
  Blynk.virtualWrite(V4, myBPM);
}
void setup()
{   
  
   Serial.begin(115200);
    while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");
  if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.  
  }
  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  //setupt motion detection
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true); // Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);
  pulseSensor.analogInput(PulseWire);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);
  Serial.println("");
  delay(50);
 
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(100L, sendSensor);
 
  }

void loop()
{
  Blynk.run();
  timer.run();
 }