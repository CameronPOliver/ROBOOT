### Test code for initiating capacitor charge and discharge sequences by with tactile buttons.

### In this version there are 2 buttons and a potentiometer:
#### Charge Button:  
Should act as a flip-flop (when pressed, the charging pin output signal will turn on and remain on until pressed again to turn off and remain off, and so on).
#### Discharge Button:  
Should initiate a 2 pin discharge sequence (when pressed, the coil 1 discharge signal pin should turn on for a set amount of time, then turn off, followed by a set delay period, then coil 2 should turn on for a set amount of time).
#### Potentiometer:  
Can be used to vary the discharge times and delays (currently the discharge delay and both discharge times are all equal and modified equally by the potentiometer.  In a future version, multiple potentiometers will be used to adjust these independently.  In the final version, the timing should all be predetermined and hard coded, so the potentiometers will be removed)
