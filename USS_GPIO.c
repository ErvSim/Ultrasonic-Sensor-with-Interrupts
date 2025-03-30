#include <stdio.h>
#include "pico/stdlib.h"

volatile absolute_time_t end_time;
volatile absolute_time_t start_time;
volatile uint32_t pulse_us;
volatile bool interrupt_flag = false;

void config_gpio(uint pin,uint direction);
void trigger_pulse(uint pin);
void gpio_callback_function(uint pin, uint32_t event);

int main()
{
    stdio_init_all();
    sleep_ms(2000);
    config_gpio(16,GPIO_OUT);   //initlize pin 16 for Trigger
    config_gpio(17,GPIO_IN);    //initlize pin 17 for Echo

    gpio_set_irq_enabled_with_callback(17,GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, gpio_callback_function);   //interrupt for pin 17

    while (true)
    {
        trigger_pulse(16); //call trigger

        while(!interrupt_flag)
        {
            tight_loop_contents();  //pretend main is doing something
        }
        float distance_in_inches = pulse_us/ 148.0;     //convert us to inches
        printf("The distance is %.2f inches\n", distance_in_inches);
        interrupt_flag = false;     //reset flag for next interupt
        sleep_ms(200);   //delay to not overload cpu
        
    }
}

void config_gpio(uint pin,uint direction) //User function to take care of gpio initilization
{
    gpio_init(pin);
    gpio_set_dir(pin,direction);
}

void trigger_pulse(uint pin)     // Trigger function 
{
    gpio_put(pin,1);    
    sleep_us(10);       //allow 10us per datasheet
    gpio_put(pin,0);
    sleep_us(20);       //stability right before Echoing 
}
void gpio_callback_function(uint pin, uint32_t event)
{
    if(pin == 17)   //not needed for this project but good habit to get into since we might have more than 1 pin using interrupts
    {   
        if(event & GPIO_IRQ_EDGE_RISE)      // check rising edge
        {
            start_time = get_absolute_time();   //store starting time
        }
        if(event & GPIO_IRQ_EDGE_FALL)      //check for falling edge
        {
            end_time = get_absolute_time(); // store ending time
            pulse_us = absolute_time_diff_us(start_time, end_time);    // end - start in us
            interrupt_flag = true;      //make flag true to get out of while loop.
        }
    }
    
}