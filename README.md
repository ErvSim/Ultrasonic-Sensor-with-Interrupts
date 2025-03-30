This project uses an HC-SR04 ultrasonic sensor with a Raspberry Pi Pico to measure distance by calculating the duration of an ultrasonic pulse that travels to an object and reflects back. For this project, interrupts were used.

Per the datasheet:

- VCC is connected to 5V, which means it should be connected to physical pin 40 (VBUS).

- Ground is connected to GND.

- Trigger is connected to GPIO 16.

- Echo is connected to GPIO 17.

Now that the physical wiring is complete, we can proceed with the code.

As usual:

stdio_init_all() is called to initialize the standard I/O.

A small delay is added to allow the USB to activate.

A user-defined function was created to initialize the GPIO pins, keeping the main loop cleaner.

Once the GPIOs are initialized, we are ready to move on to capturing pulse durations and calculating distance.

<img src="https://github.com/user-attachments/assets/5760a91b-6b32-41b7-bb69-5b9f742e68c4" height="50%" width="60%" alt="Disk Sanitization Steps"/>


<img src="https://github.com/user-attachments/assets/db46bcb7-247b-4d4c-82ab-eeb8d76e0158" height="50%" width="80%" alt="Disk Sanitization Steps"/>

-----

Now that everything is initialized, we can start making the ultrasonic sensor work.

Per the datasheet:

- The first step is to send a 10 µs pulse to the Trigger pin to activate the sensor.

- After sending the pulse, the sensor emits an 8-cycle burst at 40 kHz from the TX pin.

- If the sound wave hits an object and reflects back, the RX pin detects the signal, and the Echo pin goes HIGH.

- The Echo pin stays HIGH for the duration that the signal travels to the object and returns.

- The time duration between the moment Echo goes HIGH and when it goes LOW again is captured. This duration is calculated as end_time - start_time.

To convert this duration into distance in inches, the pulse duration is divided by 148.

How we do this in code is simple. First, we created a user function 
