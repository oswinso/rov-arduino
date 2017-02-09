const int NEUTRAL_THRUST = 1500;
const int MAXIMUM_THRUST = 1700;
const int MINIMUM_THRUST = 1230;
const int DEADBAND_THRUST = 25;

void setup() {
  // put your setup code here, to run once:

}

void loop() {

}

// Changes response curve so lower thrust values are more accurate
float processResponseCurve(float value) {
  if(value>0) {
    return 100*value*value;
  } else {
    return -100*value*value
  }
}

void processJoystickValues(int[] motorVectors) {
  // IDK IF I NEED TO SCALE REMINDER!!!!
  float x = processResponseCurve(map(motorVectors[0], -128, 127, -1, 1));
  float y = processResponseCurve(map(motorVectors[1], -128, 127, -1, 1));
  float z = processResponseCurve(map(motorVectors[2], -128, 127, -1, 1));

  float leftM = mapSomethingToSomething(x+y);
  float rightM = mapSomethingToSomething(x-y);
  float verticalM = mapSomethingToSomething(z);

  return [leftM, rightM, verticalM];
}

float mapSomethingToSomething(float value) {
  if(value == 0) {
    return NEUTRAL_THRUST;
  }
  // Account for deadband in thruster
  if(value > 0) {
    return map(value, 0, 100, NEUTRAL_THRUST+DEADBAND_THRUST, MAXIMUM_THRUST);
  } else {
    return map(value, -100, 0, MINIMUM_THRUST, NEUTRAL_THRUST-DEADBAND_THRUST);
  }
}

float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// INCOMING COMMUNICATIONS PROTOCOL
// {
//   x:   BYTE (-128-127)
//   y:   BYTE (-128-127)
//   z:   BYTE (-128-127)
//   arm: BYTE (-128-127)
// }

// OUTGOING PROTOCOL
// {
//    HEADING:      2 BYTES (-32769-32768)
//    DEPTH:        2 BYTES (-32769-32768)
//    MOTOR VALUES: 5* 1 BYTE
// }

// [x, y, z]
// forward-backward is x, left-right is y, up-down is z.


// X: +MAX, Y: 0
// LEFT: +MAX, RIGHT: +MAX
//
// X: 0 Y: +MAX
// LEFT: -MAX, RIGHT: +MAX
//
// X: +MAX, Y: +MAX
// LEFT: x, RIGHT:
