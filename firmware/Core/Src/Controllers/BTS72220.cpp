/*
 * BTS72220.cpp
 *
 *  Created on: Feb 27, 2024
 *      Author: Piotr Lesicki
 */

#include <Controllers/BTS72220.hpp>
#include <string.h>
#include <concepts>

//  ┏┓    ┓  ┳       ┏
//  ┃ ┏┳┓┏┫  ┃┏┓╋┏┓┏┓╋┏┓┏┏┓
//  ┗┛┛┗┗┗┻  ┻┛┗┗┗ ┛ ┛┗┻┗┗
//

namespace Cmd
{
	enum struct ReadAddr : uint8_t
	{
		OUT		= 0b0000'0000,
		RCS		= 0b0000'1000,
		SRC 	= 0b0000'1001,
		OCR		= 0b0000'0100,
		RCD 	= 0b0000'1100,
		KRC		= 0b0000'0101,
		PCS		= 0b0000'1101,
		HWCR	= 0b0000'0110,
		ICS		= 0b0000'0111,
		WARN	= 0b0'0000'001,
		DIAG	= 0b0'0000'010,
		ERR		= 0b0'0000'011,
	};

	enum struct WriteAddr : uint8_t
	{

	};

	struct Register
	{
		constexpr void toRaw(uint8_t *buff)
		{
			memcpy(buff, this, 1);
		}

		constexpr void fromRaw(uint8_t *buff)
		{
			memcpy(this, buff, 1);
		}
	};

	struct RCS : public Register
	{
		public:
			static constexpr uint8_t read_addr = uint8_t(ReadAddr::RCS);
			uint8_t restart_counter_value : 3;
		private:
			uint8_t _;
	};

	enum struct SlewRate : uint8_t
	{
		Normal,
		Adjusted
	};

	struct SRC : public Register
	{
		public:
			static constexpr uint8_t read_addr = uint8_t(ReadAddr::SRC);

			SlewRate slew_rate0 : 1;
			SlewRate slew_rate1 : 1;
			SlewRate slew_rate2 : 1;
			SlewRate slew_rate3 : 1;
		private:
			uint8_t _ : 4;
	};

	enum struct OverCurrentProtectionLevel : uint8_t
	{
		HighLevel,
		LowLevel
	};

	struct OCR : public Register
	{
		public:
			static constexpr uint8_t read_addr = uint8_t(ReadAddr::OCR);

			OverCurrentProtectionLevel over_current_level0 : 1;
			OverCurrentProtectionLevel over_current_level1 : 1;
			OverCurrentProtectionLevel over_current_level2 : 1;
			OverCurrentProtectionLevel over_current_level3 : 1;
		private:
			uint8_t _ : 4;
	};

	enum struct RestartStrategy : uint8_t
	{
		Automatic,
		Latch
	};

	struct RCD : public Register
	{
		public:
			static constexpr uint8_t read_addr = uint8_t(ReadAddr::RCD);

			RestartStrategy restart_strategy0 : 1;
			RestartStrategy restart_strategy1 : 1;
			RestartStrategy restart_strategy2 : 1;
			RestartStrategy restart_strategy3 : 1;
		private:
			uint8_t _ : 4;
	};

	enum struct CurrentSensRange : uint8_t
	{
		HighRange,
		LowRange
	};

	struct KRC : public Register
	{
		public:
			static constexpr uint8_t read_addr = uint8_t(ReadAddr::KRC);

			CurrentSensRange krc0 : 1;
			CurrentSensRange krc1 : 1;
			CurrentSensRange krc2 : 1;
			CurrentSensRange krc3 : 1;
		private:
			uint8_t _ : 4;
	};

	enum struct ParallelConfig
	{
		Independent,
		Out03,
		Out12,
		Out03Out12
	};

	struct PCS : public Register
	{
		public:
			static constexpr uint8_t read_addr = uint8_t(ReadAddr::PCS);

			SlewRate slew_rate_channel : 1;
			bool clear_channel : 1;
			ParallelConfig parallel_config : 2;
		private:
			uint8_t _ : 4;
	};

	enum struct InputCombination : uint8_t
	{
		Or,
		And
	};

	struct HWRC : public Register
	{
		public:
			static constexpr uint8_t read_addr = uint8_t(ReadAddr::HWRC);

			bool clear : 1;
			bool reset: 1;
			InputCombination col : 1;
		private:
			uint8_t _ : 5;
	};

//	struct ICS : public Register
//	{
//		public:
//			uint8_t clc : 1;
//			uint8_t rst : 1;
//			uint8_t col : 1;
//		private:
//			uint8_t _ : 5;
//	};

	enum struct Mux : uint8_t
	{
		Pin0,
		Pin1,
		Pin2,
		Pin3,
		DiagEn,
		Verify,
		HighZ,
		Sleep
	};

	struct DCR : public Register
	{
		public:
			static constexpr uint8_t read_addr = uint8_t(ReadAddr::DCR);

			Mux mux : 3;
			bool switch_register : 1;
		public:
			uint8_t _ : 4;
	};

	template < typename T >
	concept ReadRegisterConcept = requires(T x)
	{
	    std::is_base_of_v < Reg, T >;
	    sizeof(T) == 1;
	    { T::read_addr };
	};

	template < typename T >
	concept WriteRegisterConcept = requires(T x)
	{
		std::is_base_of_v < Reg, T >;
		sizeof(T) == 1;
		{ T::write_addr };
	};

	struct WriteOutReg
	{
		public:
			WriteOutReg(uint8_t ch0, uint8_t ch1, uint8_t ch2, uint8_t ch3, uint8_t edo) :
				ch0(ch0), ch1(ch1), ch2(ch2), ch3(ch3), edo(edo), id(0b100) { }
			// write channel state
			uint8_t ch0 : 1;
			uint8_t ch1 : 1;
			uint8_t ch2 : 1;
			uint8_t ch3 : 1;
			uint8_t edo : 1;
		private:
			uint8_t id : 3;
	};

	struct WriteReg
	{
		public:
			template < WriteRegisterConcept Reg >
			WriteReg(Reg reg, WriteAddr waddr) : data(reg.toRaw()), waddr(uint8_t(Reg::write_addr)), id(0b11) { }
			uint8_t data : 4;
			uint8_t waddr : 2;
		private:
			uint8_t id : 2;
	};

	struct ReadReg
	{
		public:
			template < ReadRegisterConcept Reg >
			ReadReg() : raddr(uint8_t(Reg::read_addr)), _(0b000), id(0b0) { }
			uint8_t raddr : 4;
		private:
			uint8_t _ : 3;
			uint8_t id : 1;
	};
}

//  ┳┓  ━┓┏┓┏┓┏┓┏┓
//  ┣┫╋┏ ┃┏┛┏┛┏┛┃┫
//  ┻┛┗┛ ╹┗━┗━┗━┗┛
//

auto Bts72220::cmdInit()
{

}

auto Bts72220::cmdSetChState(Bts72220::Channel channel)
{

}

auto Bts72220::cmdSetIMuxCh(Bts72220::Channel channel)
{

}

float Bts72220::current(uint16_t raw_adc)
{

}
