/*
 * gpio.hpp
 *
 *  Created on: Feb 22, 2024
 *      Author: Piotr Lesicki
 */

#ifndef INC_CONTROLLERS_GPIO_HPP_
#define INC_CONTROLLERS_GPIO_HPP_

#ifdef __cplusplus

#include <main.h>
#include <gpio.h>

//	┏┓  •  ┳
//	┃┓┏┓┓┏┓┃┏┓
//	┗┛┣┛┗┗┛┻┛┗
//	  ┛

class GpioIn
{
	public:
		/**
		  * @brief GpioIn Constructor
		  * @param gpio - GPIO_TypeDef gpio's register
		  * @param pin - gpio pin
		  */
		GpioIn(GPIO *gpio, uint16_t pin);
		/**
		  * @brief GpioIn Constructor
		  * @param gpio - GPIO_TypeDef gpio's register
		  * @param pin - gpio pin
		  * @param inv - if true, 0 on the input would be interpreted as 1
		  */
		GpioIn(GPIO_TypeDef *gpio, uint16_t pin, bool inv);

		/**
		 * @brief read channel state
		 * @retval bool - value of the channel if inv was set to
		 * true return negated value
		 */
		bool read();

	private:
		GPIO_TypeDef *gpio;
		uint16_t pin;
		uint16_t inv;
};

//	┏┓  •  ┏┓
//	┃┓┏┓┓┏┓┃┃┓┏╋
//	┗┛┣┛┗┗┛┗┛┗┻┗
//	  ┛

class GpioOut
{
	public:
		/**
		  * @brief GpioOut Constructor
		  * @param gpio - GPIO_TypeDef gpio's register
		  * @param pin - gpio pin
		  */
		GpioOut(GPIO_TypeDef *gpio, uint16_t pin);
		/**
		  * @brief GpioIn Constructor
		  * @param gpio - GPIO_TypeDef gpio's register
		  * @param pin - gpio pin
		  * @param inv - if true, setting the channel ON will output 0 and
		  * setting the channel OFF will output 1
		  */
		GpioOut(GPIO_TypeDef *gpio, uint16_t pin, bool inv);
		/**
		 * @brief set channel logic state to HIGH, LOW if inv was set to true
		 * @retval none
		 */
		void on();
		/**
		 * @brief set channel logic state to LOW, HIGH if inv was set to true
		 * @retval none
		 */
		void off();
		/**
		 * @brief toggle the channel state
		 * @retval none
		 */
		void toggle();

	private:
		GPIO_TypeDef *gpio;
		uint16_t pin;
		uint16_t inv;
};


//	┏┓  •  ┳┳┓•
//	┃┓┏┓┓┏┓┃┃┃┓┏┏
//	┗┛┣┛┗┗┛┛ ┗┗┛┗
//	  ┛

#define PORT(LABEL) 			(LABEL##_GPIO_Port)
#define PIN(LABEL) 				(LABEL##_Pin)

#define GPIO_OUT(LABEL, INV) 	GpioOut(PORT(LABEL), PIN(LABEL), INV)
#define GPIO_IN(LABEL, INV) 	GpioIn(PORT(LABEL), PIN(LABEL), INV)

#endif //__cplusplus

#endif /* INC_CONTROLLERS_GPIO_HPP_ */
