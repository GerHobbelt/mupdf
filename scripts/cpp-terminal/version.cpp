/*
* cpp-terminal
* C++ library for writing multi-platform terminal applications.
*
* SPDX-FileCopyrightText: 2019-2023 cpp-terminal
*
* SPDX-License-Identifier: MIT
*/

#include "cpp-terminal/version.hpp"

// clang-format off
std::uint16_t Term::Version::major() noexcept
{
	static std::uint16_t ret{1};
	return ret;
}

std::uint16_t Term::Version::minor() noexcept
{
	static std::uint16_t ret{0};
	return ret;
}

std::uint16_t Term::Version::patch() noexcept
{
	static std::uint16_t ret{0};
	return ret;
}

std::string Term::Version::string() noexcept
{
	static const char* ret{"1.0.0"};
	return ret;
}

std::string Term::homepage() noexcept
{
	static const char* ret{"https://github.com/jupyter-xeus/cpp-terminal"};
	return ret;
}
// clang-format on
