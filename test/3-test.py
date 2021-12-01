'''
def send(name,address):
    return 'package is sent to %s, located in %s' %(name, address)
print(send('winnie','shanghai'))
'''
def subselect(*collect):
    print(collect)
    print(type(collect))

def packing(**aa):
    print(type(aa))
    print(aa)

subselect(['ppe-test-1','dddd'])
packing(a=1,b=2,sub[11,22])
