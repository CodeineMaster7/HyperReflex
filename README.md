# HyperReflex

Console Setup:
The SetConsoleColor function changes the console text color, and PrintBanner clears the console and displays a banner with author information.

Configuration Loading:
The Config structure holds parameters for the script, including the scanning zone, target color (RGB), color tolerance, and delays for CapsLock and Alt keys. The LoadConfig function reads these settings from the config.txt file.

GDI+ Initialization:
The InitGDIPlus function initializes the GDI+ library, which is necessary for handling graphics operations and screen capturing.

Screen Capture and Analysis:

* The CaptureScreenRegion function captures an image of a specified rectangular area of the screen.

* The SearchForColorInRegion function analyzes the captured image to search for the target color within the specified tolerance. The IsColorMatch function is used to compare the colors.

Action Simulation:
When the target color is detected in the specified screen area:

* The script waits for a defined delay (depending on whether the CapsLock or Alt key is pressed).

* It simulates a mouse click using the ClickMouse function.

* The reaction time is calculated and displayed.

Script Restart:
The RestartScript function restarts the script to refresh its state after a click event.

Main Loop:
The main loop continuously checks if either the Alt or CapsLock key is pressed. If one of these keys is active and the target color is found in the specified area, the script simulates a mouse click and then restarts itself. Pressing the End key exits the script.

In summary, the script monitors a specific region of the screen for a target color. When the color is detected and the appropriate keys are pressed, it performs a mouse click after a configured delay, then restarts to continue monitoring.
