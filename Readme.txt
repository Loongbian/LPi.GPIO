# install python LPi.GPIO module
sudo python3 setup.py install --record files.txt 

# remove LPi.GPIO
cat files.txt | sudo xargs rm -rf 


py_setup_channel()
  |
  mmap_gpio_mem()//gpio->mem ---setup()//c_gpio.c ---mmap()//mmap function
  |                    
  setup_one()---get_gpio_number()//common.c pin_to_gpio_rev1[] ---gpio_function()           
  
