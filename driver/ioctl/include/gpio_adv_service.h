// wait_for_edge
int wait_for_edge(int gpio_num, int trigger_mode, int timeout);
//int add_event_callback(int gpio_num, gpio_callback_t cb, int bouncetime);
//int remove_event_callback(int gpio_num, gpio_callback_t cb);
int add_event_detect(int gpio_num, int edge, gpio_callback_t cb, int bouncetime);
//int remove_event_detect(int gpio_num);
//int event_detected(int gpio_num);

