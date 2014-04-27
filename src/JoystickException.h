/*
 * Copyright 2014 Jamie Smith
 *
 * This file is part of libjoystick.
 *
 * libjoystick is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libjoystick is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libjoystick.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef JOYSTICKEX_H_
#define JOYSTICKEX_H_

#include <exception>

namespace joy
{

class JoystickException : std::exception
{
	std::string _message;

public:
	JoystickException(std::string message)
	:_message(message)
	{

	}

	std::string const what()
	{
		return "JoystickException: " + _message;
	}

	virtual ~JoystickException() throw()
	{

	}
};

}

#endif /* JOYSTICKEX_H_*/
