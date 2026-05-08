from math import pi, sqrt
try: 
    f  =1e6*float(input("inter frequ as Mhz "))
except:
    f= 55.8e6

# f = 1/2pi sqrt(lc)
c = 43e-12
c=133e-12
l= 1/((2*pi*f)**2*c)
print("loop inductance nH = ", l*1e9)
a=input("press any keys to exit")
