# Create

A proof of concept "port" of Create to Minecraft Bedrock. IMPORTANT: This is not intended as a complete port, the original codebase is over 200K LOC and I dont got time for all that, but more of a demo of anything being possible with native modding on Bedrock given enough effort. This project is created as close to a 1 to 1 of the original codebase as possible (only adjusting for small platform differences, and differences between java and c++).

## Issues

1. Currently there are issues with rotation propagation not updating, specifically when you place a shaft next to a motor for the first time, it resets the motors speed to 0 on the server, and therefore fails to propagate speed to the shaft. I'm unsure what the difference is between my port and the original java right now. Changing the speed of the motor will cause it to update and work correctly.

2. Very minor difference is the mouse cursor currently doesnt snap to the current position in the motor slider, so when opening the UI, the cursor will be at a random location.

## Bugs found in the original codebase

1. KineticScrollValueBehaviour::formatSettings results in ticks in sliders using this formatter with the number 1 at the start