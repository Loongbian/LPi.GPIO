#define MODE_UNKNOWN -1
#define BOARD        10
#define BCM          11 //#define LS 11

int gpio_mode;
const int (*pin_to_gpio_rev)[61];
const int (*pin_to_gpio)[61];
int gpio_direction[74];

int module_setup;
int get_gpio_number(int channel, unsigned int *gpio);

