/*
 * Joystick.cpp
 *
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

#include "Joystick.h"
#include "JoystickException.h"

#include <libeurointroduction/Util/DefineZeroedStruct.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

namespace joy
{

Joystick::Joystick(std::string path)
:_joystick_fd(open(path.c_str(), O_RDONLY))
{
	if(_joystick_fd == -1)
	{
		throw JoystickException("Error " + std::string(strerror(errno)) + " opening file \"" + path + "\"");
	}

	//read number of buttons and axes out of driver
	char axes = 0;
	char buttons = 0;
	if((ioctl(_joystick_fd, JSIOCGAXES, &axes) == -1) || (ioctl(_joystick_fd, JSIOCGBUTTONS, &buttons) == -1))
	{
		throw JoystickException("Error " + std::string(strerror(errno)) + " reading properties from joystick \"" + path + "\"");
	}

	_buttonValues = std::vector<bool>(buttons);
	_axisValues = std::vector<int16_t>(axes);

	_thread = boost::thread(&Joystick::run, boost::ref(*this));
}

std::string Joystick::getName()
{
	char nameBuffer[129];

	ioctl(_joystick_fd, JSIOCGNAME(sizeof(nameBuffer) - 1),  &nameBuffer);

	nameBuffer[sizeof(nameBuffer) - 1] = '\n';

	return std::string(nameBuffer);
}

void Joystick::run()
{
	while(true)
	{
		try
		{
			boost::this_thread::interruption_point();

			DEFINE_ZEROED_STRUCT(js_event, event);

			if(read(_joystick_fd, &event, sizeof(event)) == -1)
			{
				std::cerr << "libjoystick: Error reading from joystick: " << strerror(errno);
			}

			if((event.type & ~JS_EVENT_INIT) == JS_EVENT_BUTTON)
			{
				auto buttonChangeEvent = std::make_shared<ButtonChangeEvent>(event.number, !(event.value));

				//update class variables
				boost::unique_lock<boost::mutex> lock;
				_buttonValues[buttonChangeEvent->getButtonNumber()] = buttonChangeEvent->isPressed();

				if(_buttonCallback)
				{
					(*_buttonCallback)(buttonChangeEvent);
				}
			}
			else if((event.type & ~JS_EVENT_INIT) == JS_EVENT_AXIS)
			{
				auto axisChangeEvent = std::make_shared<AxisChangeEvent>(event.number, event.value);

				//update class variables
				boost::unique_lock<boost::mutex> lock;
			    _axisValues[axisChangeEvent->getAxisNumber()] = axisChangeEvent->getValue();

				if(_axisCallback)
				{
					(*_axisCallback)(axisChangeEvent);
				}
			}
			else
			{
				//we can't throw an exception, there's no one there to hear us scream.
				std::cerr << "libjoystick: error in internal thread: unknown change event type " << (event.type & ~JS_EVENT_INIT);
			}
		}
		catch(boost::thread_interrupted & interrupt)
		{
			return;
		}
	}
}

Joystick::~Joystick()
{
	_thread.interrupt();

	if(close(_joystick_fd) == -1)
	{
		throw JoystickException("Could not close file descriptor: error " + std::string(strerror(errno)));
	}
}

void Joystick::setButtonCallback(std::shared_ptr<joy::Joystick::ButtonCallback> newCallback)
{
	_buttonCallback = newCallback;
}

void Joystick::setAxisCallback(std::shared_ptr<joy::Joystick::AxisCallback> newCallback)
{
	_axisCallback = newCallback;
}

void Joystick::clearButtonCallback()
{
	_buttonCallback = decltype(_buttonCallback){};
}

void Joystick::clearAxisCallback()
{
	_axisCallback = decltype(_axisCallback){};
}

bool Joystick::getButtonValue(unsigned int buttonNumber)
{
	if(buttonNumber >=  _buttonValues.size())
	{
		throw JoystickException("Attempt to access button number " + std::to_string(buttonNumber) +
				" of joystick with only " + std::to_string(_buttonValues.size()) + "buttons");
	}

	return _buttonValues[buttonNumber];
}

int16_t Joystick::getAxisValue(unsigned int axisNumber)
{
	if(axisNumber >=  _axisValues.size())
	{
		throw JoystickException("Attempt to access axis number " + std::to_string(axisNumber) +
				" of joystick with only " + std::to_string(_axisValues.size()) + "axes");
	}

	return _axisValues[axisNumber];
}

}
