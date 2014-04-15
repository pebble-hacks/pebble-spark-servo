// WIRELESS SERVO EXAMPLE CODE
// BDub 4/7/2014
//
// Connect RED wire to VIN (~5V)
// Connect ORANGE, YELLOW, or WHITE wire to A0 (servo signal)
// Connect BLACK or BROWN wire to GND (0V)
// Adjust these connections for your particular servo
// if you have a wiring diagram for it.
//
//
// COMPLEMENTARY API CALL
// POST /v1/devices/{DEVICE_ID}/{FUNCTION}
//
// # EXAMPLE REQUEST
// curl https://api.spark.io/v1/devices/0123456789abcdef01234567/servo \
//     -d access_token=1234123412341234123412341234123412341234 \
//     -d "args=180"
//----------------------------------------------------------------------------

Servo myservo;  // create servo object to control a servo
                // a maximum of eight servo objects can be created

int pos = 0;    // variable to store the servo position

void setup()
{
  // attaches the servo on the A0 pin to the servo object
  myservo.attach(A0);

  // register the Spark function
  Spark.function("servo", updateServo);
}

void loop()
{
  // do nothing
}

//this function automagically gets called upon a matching POST request
int updateServo(String command)
{
  // convert string to integer
  uint8_t pos = command.toInt();

  // process if integer is 0 - 180
  if(pos <= 180)
  {
    // tell servo to go to position in variable 'pos'
    myservo.write(pos);

    // return an integer success code that can be processed by our app
    return 200;
  }
  else {
    // return an integer error code that can be processed by our app
    return -1;
  }
}