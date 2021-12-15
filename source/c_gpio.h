int setup(void);
void setup_gpio(int gpio, int direction);
void output_gpio(int gpio, int value);
int input_gpio(int gpio);
void cleanup(void);



#define SETUP_OK           0
#define SETUP_DEVMEM_FAIL  1
#define SETUP_MALLOC_FAIL  2
#define SETUP_MMAP_FAIL    3

#define INPUT  1 // is really 0 for control register!
#define OUTPUT 0 // is really 1 for control register!

#define HIGH 1
#define LOW  0


