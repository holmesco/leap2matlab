install the leap sdk into the directory containing the leap2matlab files (just download them and put them in the same directory).
you may need to move Leap.dll, libmex.dll and libmx.dll into this folder directly. it cant see them sometimes for some reason.
use dependancy walker for other dependancy related problems.

run build to set up the leap2matlab mex file

in matlab call leap2matlab('connect') to connect to the leap motion sensor

then call leap2matlab('getframe') every time you want a frame.

the controller will disconnect when matlab closes or if clear is called.

i think there is some delay between connecting and when the first valid frame is available so make sure to pause like in the 
sample m file.



sincerely,


holmesco
