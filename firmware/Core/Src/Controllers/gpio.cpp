/*
 * gpio.cpp
 *
 *  Created on: Feb 22, 2024
 *      Author: Piotr Lesicki
 */

#include <gpio.hpp>
#include <algorithm>

//	┏┓  •  ┳
//	┃┓┏┓┓┏┓┃┏┓
//	┗┛┣┛┗┗┛┻┛┗
//	  ┛

GpioIn::GpioIn(GPIO_TypeDef *gpio, uint16_t pin) : gpio(gpio), pin(pin), inv(0) { }

GpioIn::GpioIn(GPIO_TypeDef *gpio, uint16_t pin, bool inv) : gpio(gpio), pin(pin), inv(inv ? pin : 0) { }

bool GpioIn::read()
{
	return bool((gpio->IDR & pin) ^ inv);
}

//	┏┓  •  ┏┓
//	┃┓┏┓┓┏┓┃┃┓┏╋
//	┗┛┣┛┗┗┛┗┛┗┻┗
//	  ┛

GpioOut::GpioOut(GPIO_TypeDef *gpio, uint16_t pin) : gpio(gpio), pin(pin), inv(0) { }

GpioOut::GpioOut(GPIO_TypeDef *gpio, uint16_t pin, bool inv) : gpio(gpio), pin(pin), inv(inv ? pin : 0) { }

void GpioOut::on()
{
	gpio->ODR = (gpio->ODR & ~inv) | (pin ^ inv);
}

void GpioOut::off()
{
	gpio->ODR = (gpio->ODR & ~(pin ^ inv)) | inv;
}

void GpioOut::toggle()
{
	gpio->ODR = gpio->ODR ^ pin;
}

