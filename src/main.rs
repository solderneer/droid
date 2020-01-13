// ! Prints "Hello, world!" on the host console using semihosting

#![no_main]
#![no_std]

extern crate panic_halt;

use cortex_m_rt::entry;
use stm32f4;

#[entry]
fn main() -> ! {
    // hprintln!("Hello, world!").unwrap();

    let cp = cortex_m::Peripherals::take().unwrap();
    let p = stm32f4::stm32f410::Peripherals::take().unwrap();

    // Get GPIO and RCC peripherals to set up AHB clock
    let gpioa = p.GPIOA;
    let rcc = p.RCC;

    // Enabling rcc ahb1 for GPIOA
    rcc.ahb1enr.write(|w| w.gpioaen().bit(true));

    // Configure MODER OUTPUT, OTYPE PP, register for PA5
    gpioa.moder.write(|w| w.moder5().bits(0b01));
    gpioa.otyper.write(|w| w.ot5().bit(false));
    gpioa.ospeedr.write(|w| w.ospeedr5().bits(0b01));
    gpioa.pupdr.write(|w| unsafe { w.pupdr5().bits(0b10) });

    // Turning the LED on
    gpioa.bsrr.write(|w| w.bs5().set_bit());

    loop {}
}
