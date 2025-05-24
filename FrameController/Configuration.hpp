/// Stepper Motors
  #define STP_STEP 0
  #define STP_DIR 1

  // { STEP, DIR }
  const int stp0_PINS[2] = { 7, 8 };
  const int stp1_PINS[2] = { 10, 11 };
  const int stp2_PINS[2] = { 13, 14 };

  const int STP_MAX_SPEED = 800;
  const int STP_MAX_ACCEL = 1600;

  const int PPR = 800;       // pulse/rev
  const int SCREW_PITCH = 8; // mm/rev
  const int ZERO_SPEED = -PPR / 10; // Should take approx 10 seconds

  const int SCAN_POLLING_UPDATE_RATE = 900; // us

  const int STP_N = 3;

/// Limit Switches
  const int LS_N = 3;
  const int LS_PINS[LS_N] = { 20, 19, 18 }; // X, Y, Z Axis
  const bool LS_ACTIVE = true;
  
  int ls_states[3] = { false, false, false };

/// Servo Motor
  const int SERVO_PIN = 2;

  unsigned long currentMillis;
  unsigned long servoMillis;

  const unsigned long servo_refresh_rate = 10;