void shut_device_off(void);
float get_position_meters(int axis);

void configure_stepper(void);
void configure_steppers(void);

void configure_limit_switches_pinmode(void);
void read_limit_switches_state(int *ls_states);

void scan_for_zero(int axis);