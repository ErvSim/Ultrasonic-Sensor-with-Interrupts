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

We can see this a visual picture of how this should work shown below


<img src="https://github.com/user-attachments/assets/c2b981e1-c299-4c9b-b159-678402b21e6c" height="50%" width="80%" alt="Disk Sanitization Steps"/>

To convert this duration into distance in inches, the pulse duration is divided by 148.

How we do this in code is simple. First, we created a user function called trigger_pulse(uint pin) which:

- Sets pin 16 to HIGH.

- Waits for 10 µs.

- Sets pin 16 back to LOW.

- Waits 20 µs (for stability before starting with the Echo pin).

To enable the interrupt, we use the following line: gpio_set_irq_enabled_with_callback(17, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, gpio_callback_function); 

Here’s how this function works:

- Pin 17 (Echo pin) is set to trigger the interrupt.

- The event is set to trigger on either a rising edge or a falling edge.

- The interrupt is enabled.

- When an interrupt occurs, a pointer to the callback function gpio_callback_function is called to handle the event. We use a pointer rather then directly calling it because we do not want it to go into the function immediately. we just give it the address and once the interrupt is triggered, it calls then fucntion
  
---------

<img src="https://github.com/user-attachments/assets/a959d82b-45c3-40ce-827f-681a232407f0" height="50%" width="100%" alt="Disk Sanitization Steps"/>

The gpio_callback_function is a user-defined function, but the arguments must be uint and uint32_t because the Pico SDK is hardcoded to pass the pin number and event type when an interrupt occurs.

Even though we are only using pin 17, it’s good practice to check if (gpio == 17) in case multiple pins use interrupts in future projects.

When a rising edge is detected, we store the current time in start_time, and when a falling edge occurs, we store the time in end_time. These values are captured using get_absolute_time().

Since interrupts work asynchronously, we use global variables to allow both the main loop and the interrupt to access these values. They are also marked as volatile to prevent the compiler from caching them, ensuring that the most up-to-date values are used.

The required variables are:

- start_time → Stores time when Echo goes HIGH.

- end_time → Stores time when Echo goes LOW.

- pulse_us → Holds pulse duration (end - start).

- interrupt_flag → Indicates when a measurement is ready.

<img src="https://github.com/user-attachments/assets/1c782ec6-8cbe-49d2-8fa4-d737f5a10755" height="50%" width="50%" alt="Disk Sanitization Steps"/>

After calculating pulse_us in the interrupt, the flag is set to true to notify the main loop. The main loop waits for this flag, calculates the distance by dividing pulse_us by 148 to get inches, resets the flag to false, and adds a 200 ms delay to prevent CPU overload.

------

An example will be shown where the ultrasonic sensor (USS) is placed approximately 11 inches away from the monitor. Since the sensor is being held by hand, there may be slight errors due to hand movement.

The sensor is generally accurate, but during testing, it was observed that it struggles to read distances accurately from flat surfaces, likely because of poor reflection from the TX. This issue is more noticeable when measuring against convex surfaces, where the reflected signal may not return directly to the RX, causing inconsistent results.

<img src="https://github.com/user-attachments/assets/46b8176c-c39a-4bb6-bd0c-31e9beaae13c" height="50%" width="30%" alt="Disk Sanitization Steps"/>

<img src="https://github.com/user-attachments/assets/bf90565d-c055-4e88-b519-7537c29cfc3c" height="50%" width="80%" alt="Disk Sanitization Steps"/>



