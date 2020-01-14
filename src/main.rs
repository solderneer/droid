// ! Prints "Hello, world!" on the host console using semihosting

#![no_main]
#![no_std]

extern crate panic_halt;

use core::cell::{Cell, RefCell};
use cortex_m::interrupt::{free, Mutex};
use cortex_m_rt::entry;
use stm32f4;
use stm32f4::stm32f410::interrupt;

static ELAPSED_MS: Mutex<Cell<u32>> = Mutex::new(Cell::new(0u32));
static TIMER_TIM5: Mutex<RefCell<Option<stm32f4::stm32f410::TIM5>>> =
    Mutex::new(RefCell::new(None));

#[entry]
fn main() -> ! {
    let cp = cortex_m::Peripherals::take().unwrap();
    let p = stm32f4::stm32f410::Peripherals::take().unwrap();

    // Get GPIO and RCC peripherals to set up AHB clock
    let gpioa = p.GPIOA;
    let rcc = p.RCC;
    let tim5 = p.TIM5;

    // Enabling rcc ahb1 for GPIOA
    rcc.cfgr.write(|w| w.hpre().div1().ppre1().div1());
    rcc.ahb1enr.write(|w| w.gpioaen().bit(true));
    rcc.apb1enr.write(|w| w.tim5en().bit(true));

    // Configure MODER OUTPUT, OTYPE PP, register for PA5
    gpioa.moder.write(|w| w.moder5().bits(0b01));
    gpioa.otyper.write(|w| w.ot5().bit(false));
    gpioa.ospeedr.write(|w| w.ospeedr5().bits(0b01));
    gpioa.pupdr.write(|w| unsafe { w.pupdr5().bits(0b10) });

    // Configuring the TIM5 1 second timer
    tim5.arr.write(|w| unsafe { w.bits(0b101110111000000) });
    tim5.dier.write(|w| w.uie().enabled());
    tim5.cr1.write(|w| w.cen().enabled());

    // Putting timer in a critical section for interrupt access
    free(|cs| *TIMER_TIM5.borrow(cs).borrow_mut() = Some(tim5));

    // Turning the LED on
    gpioa.bsrr.write(|w| w.bs5().set_bit());

    loop {}
}

#[interrupt]
fn TIM6_DAC1() {
    free(|cs| {
        // Clear the update interrupt
        if let Some(ref mut tim5) = *TIMER_TIM5.borrow(cs).borrow_mut() {
            tim5.sr.write(|w| w.uif().clear());
        }

        let cell = ELAPSED_MS.borrow(cs);
        let val = cell.get();
        cell.replace(val + 1);
    });
}
