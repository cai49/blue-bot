void shut_device_off(void);

void configure_stepper(void);
void configure_steppers(void);

void configure_limit_switches_pinmode(void);
void read_limit_switches_state(void);

bool _scan_for_zero(int axis);

float _get_position_meters(int axis);
float _get_pulses(float travel);

bool set_position(double x, double y, double z);
void set_position_now(double x, double y, double z);

void update_steppers(void);

void report_done(void);
