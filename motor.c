/**
 * @file   gpio_mymodule.c
 * @author Frank Quirarte/Alejandra Victoria
 * @date   10 sep 2019
 * @brief  A kernel module for controlling a GPIO LED/button pair. The
 * device mounts an LED and Pyrosensor via sysfs /sys/class/gpio/gpio17
 * and gpio27 respectively. */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>                 // for the GPIO functions
#include <linux/interrupt.h>            // for the IRQ code

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Frank_Ale");
MODULE_DESCRIPTION("A DC Motor Controller driver for the RPi");
MODULE_VERSION("0.1");

static int left_events = 0;
static int right_events = 0;
module_param(left_events, int, S_IRUGO);
MODULE_PARM_DESC(left_events, "Counter for left events");
module_param(right_events, int, S_IRUGO);  
MODULE_PARM_DESC(right_events, "Counter for right events");

static unsigned int gpioOut1 = 5;       // pin 29 (GPIO5)
static unsigned int gpioOut2 = 6;    // pin 31 (GPIO6)
static unsigned int left_irq_number;          // share IRQ num within file
static unsigned int right_irq_number;          // share IRQ num within file


// prototype for the custom IRQ handler function, function below
static irq_handler_t  left_motor_irq_handler(unsigned int irq, void
                                    *dev_id, struct pt_regs *regs);
                                    
// prototype for the custom IRQ handler function, function below
static irq_handler_t  right_motor_irq_handler(unsigned int irq, void
                                    *dev_id, struct pt_regs *regs);


/** @brief The LKM initialization function */
static int __init motor_gpio_init(void){
   int result = 0;
   printk(KERN_INFO "Motor_Controller: Initializing the Motor_Controller LKM\n");
   if (!gpio_is_valid(gpioOut1)){
      printk(KERN_INFO "Motor_Controller: invalid LED Output1\n");
      return -ENODEV;
   }
   if (!gpio_is_valid(gpioOut2)){
      printk(KERN_INFO "Motor_Controller: invalid LED Output2\n");
      return -ENODEV;
   }
   gpio_request(gpioOut1, "sysfs");          // request Output1 GPIO
   gpio_request(gpioOut2, "sysfs");          // request Output2 GPIO
   gpio_direction_output(gpioOut1, true);   // set in output mode and on
   gpio_direction_output(gpioOut2, true);   // set in output mode and on
   gpio_export(gpioOut1, false);             // appears in /sys/class/gpio
   gpio_export(gpioOut2, false); 
			               // false prevents direction change
                                       
   left_irq_number = gpio_to_irq(gpioOut1);     // map GPIO to IRQ number
   right_irq_number = gpio_to_irq(gpioOut2);     // map GPIO to IRQ number
   
   printk(KERN_INFO "Motor_Controller: left event mapped to IRQ: %d\n", left_irq_number);
   printk(KERN_INFO "Motor_Controller: right event mapped to IRQ: %d\n", right_irq_number);
   
   // This next call requests an interrupt line
   result = request_irq(left_irq_number,         // interrupt number requested
            (irq_handler_t) left_motor_irq_handler, // handler function
            IRQF_TRIGGER_RISING,  // on rising edge (press, not release)
            "left_motor_handler",  // used in /proc/interrupts
            NULL);                // *dev_id for shared interrupt lines

  // This next call requests an interrupt line
   result = request_irq(right_irq_number,         // interrupt number requested
            (irq_handler_t) right_motor_irq_handler, // handler function
            IRQF_TRIGGER_RISING,  // on rising edge (press, not release)
            "right_motor_handler",  // used in /proc/interrupts
            NULL);                // *dev_id for shared interrupt lines
                        
   printk(KERN_INFO "Motor_Controller: Right IRQ request result is: %d\n", result);
  
   return 0;
}

/** @brief The LKM cleanup function  */
static void __exit motor_gpio_exit(void){
   printk(KERN_INFO "Motor_Controller: button value is currently: %d\n",
          gpio_get_value(gpioOut1));
   //printk(KERN_INFO "GPIO_TEST: pressed %d times\n", numberPresses);
   gpio_set_value(gpioOut1, 0);    // turn out1 off
   gpio_set_value(gpioOut2, 0);    // turn out2 off
   gpio_unexport(gpioOut1);        // unexport the out1 GPIO
   gpio_unexport(gpioOut2);        // unexport the out2 GPIO
   gpio_free(gpioOut1);            // free the out1 GPIO
   gpio_free(gpioOut2);         // free the out2 GPIO
   printk(KERN_INFO "Motor_Controller: Goodbye from the LKM!\n");
}

static irq_handler_t  left_motor_irq_handler(unsigned int irq, void
                                    *dev_id, struct pt_regs *regs){
  left_events++;    
  printk(KERN_INFO "Motor_Controller: Left Interrupt: %d\n", left_events);
  return (irq_handler_t) IRQ_HANDLED; // announce IRQ handled      
}

static irq_handler_t  right_motor_irq_handler(unsigned int irq, void
                                    *dev_id, struct pt_regs *regs){
   right_events++;
   printk(KERN_INFO "Motor_Controller: Right Interrupt: %d\n", right_events);
   return (irq_handler_t) IRQ_HANDLED; // announce IRQ handled                            
}

module_init(motor_gpio_init);
module_exit(motor_gpio_exit);
