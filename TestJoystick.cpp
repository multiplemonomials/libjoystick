/*
 * TestJoystick.cpp
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

#include <libjoystick/Joystick.h>
#include <libjoystick/JoystickException.h>

#include <unistd.h>

void onButtonChange(std::shared_ptr<joy::ButtonChangeEvent> changeEvent)
{
	std::cout << "TestJoystick: Button " << changeEvent->getButtonNumber() << " is now " << std::boolalpha << changeEvent->isPressed() << std::endl;
}
void onAxisChange(std::shared_ptr<joy::AxisChangeEvent> changeEvent)
{
	std::cout << "TestJoystick: Axis " << changeEvent->getAxisNumber() << " is now " << changeEvent->getValue() << std::endl;
}

int main()
{
	try
	{
		joy::Joystick joystick(std::string("/dev/input/js2"));

		std::cout << "Binding to joystick " << joystick.getName() << std::endl;

		joystick.setButtonCallback(std::make_shared<joy::Joystick::ButtonCallback>(&onButtonChange));
		joystick.setAxisCallback(std::make_shared<joy::Joystick::AxisCallback>(&onAxisChange));

		std::cin.get();

		return 0;
	}
	catch(joy::JoystickException & error)
	{
		std::cerr << "Error in unit test: " << error.what() << std::endl;
	}
}
