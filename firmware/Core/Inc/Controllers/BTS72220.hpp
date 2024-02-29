/*
 * BTS72220-4ESA.hpp
 *
 *  Created on: Feb 22, 2024
 *      Author: Piotr Lesicki
 */

#ifndef INC_CONTROLLERS_BTS72220_HPP_
#define INC_CONTROLLERS_BTS72220_HPP_

#ifdef __cplusplus

#include <main.h>
#include <gpio.hpp>
#include <array>

class Bts72220
{
	public:
		enum struct Channel : uint8_t
		{
			_1,
			_2,
			_3,
			_4
		};

		auto cmdInit();
		auto cmdSetChState(Channel channel);
		auto cmdSetIMuxCh(Channel channel);
		float current(uint16_t raw_adc);
	private:
};

#endif //__cplusplus

#endif /* INC_CONTROLLERS_BTS72220_HPP_ */
