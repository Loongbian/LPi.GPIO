# install python LPi.GPIO module
sudo python3 setup.py install --record files.txt 

# remove LPi.GPIO
cat files.txt | sudo xargs rm -rf 
