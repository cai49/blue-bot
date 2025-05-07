void shut_device_off(void);

void configure_stepper(void);
void configure_steppers(void);

void configure_limit_switches_pinmode(void);
void read_limit_switches_state(int *ls_states);

bool _scan_for_zero(int axis);

float _get_position_meters(int axis);
long _get_pulses(int travel);

void set_position_now(int x, int y, int z);
bool set_position(int x, int y, int z);

void update_steppers(void);
