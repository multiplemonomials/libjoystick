/*
 * Joystick.h
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
 *
 * Fairly thin c++ wrapper around the Linux Joystick API.
 *
 * NOTE: for documentation about the c api, see https://www.kernel.org/doc/Documentation/input/joystick-api.txt
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <string>
#include <memory>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <sys/types.h>
#include <linux/joystick.h>

#include "JoystickChangeEvent.h"

namespace joy
{

/*
 * This class represents one game controller attached to a linux system
 * Since unix has the motto, "everything is a file*", joysticks are read like files... sort of.
 * Joystick-Files work in blocking mode, so this class has an internal thread whick reads in data.
 *
 * You can register a callback for the update of any button or axis with setButtonCallback(std::shared_ptr<ButtonCallback>)
 * and setAxisCallback(std::shared_ptr<AxisCallback>), respectively.  The ChangeEvent passed to these classes will contain
 * the number and new value of the thing that changed.
 *
 * Or, if you don't like callbacks, you can read data out of the main class with getButtonValue and getAxisValue.  Please note, however,
 * that while the joystick driver will send an init event for every button and axis when someone opens the file (i.e.
 * the user doesn't have to move a joystick before its value can be known), the joystick thread needs to be run for
 * long enough to populate the internal cache.  So, give things some time before you read out the data.
 *
 * USAGE: No-Callbacks
 *
 * void foo()
 * {
 *    joy::Joystick joystick("/dev/bar");
 *
 * 	  //stuff
 *
 * 	  bool launchMissiles = joystick.getButtonValue(0);
 *
 *	  if(launchMissiles)
 *	  {
 * 	  	 float longitude = ((double)joystick.getAxisValue(0) / 32768)*60;
 * 	  }
 * }
 *
 *  * USAGE: Callbacks
 *
 * static joy::Joystick joystick("/dev/bar");
 *
 * void launchMissiles(std::shared_ptr<joy::ButtonChangeEvent> event)
 * {
 *    if(event.getButtonNumber() == 0 && event.isPressed() == true)
 *    {
 *        float longitude = ((double)joystick.getAxisValue(0) / 32768)*60;
 *    }
 * }
 *
 * void foo()
 * {
 *    joystick.setButtonCallback(std::make_shared<joy::Joystick::ButtonCallback>(&launchMissiles));
 *
 *    //stuff
 * }
 */
class Joystick
{
public:
	typedef std::function<void(std::shared_ptr<ButtonChangeEvent>)> ButtonCallback;

	typedef std::function<void(std::shared_ptr<AxisChangeEvent>)> AxisCallback;

private:

	int _joystick_fd;

	std::vector<bool> _buttonValues;

	std::vector<int16_t> _axisValues;

	std::shared_ptr<ButtonCallback> _buttonCallback;

	std::shared_ptr<AxisCallback> _axisCallback;

	boost::mutex _mutex;

	boost::thread _thread;

	//internal thread function
	void run();

public:
	//construct Joystick from path to device
	//e.g. Joystick("/dev/joy0");
	explicit Joystick(std::string path);

	//reads the name string of the joystick.
	std::string getName();

	//returns true if specified button is pressed (0 indexed), or throws
	//a JoystickException if the value is out of range.
	bool getButtonValue(unsigned int buttonNumber);

	//returns the specified axis's value (0 indexed), or throws
	//a JoystickException if the value is out of range.
	int16_t getAxisValue(unsigned int buttonNumber);

	//Set the function to call when a button changes
	//If called with a null shared ptr, it sets the callback to nothing
	void setButtonCallback(std::shared_ptr<joy::Joystick::ButtonCallback> newCallback);

	//Set the function to call when an axis's value changes
	//If called with a null shared ptr, it sets the callback to nothing
	void setAxisCallback(std::shared_ptr<joy::Joystick::AxisCallback> newCallback);

	//Clear button callback.
	//Same as calling setButtonCallback(std::shared_ptr<joy::Joystick::ButtonCallback>{})
	void clearButtonCallback();

	//Clear axis callback.
	//Same as calling setAxisCallback(std::shared_ptr<joy::Joystick::AxisCallback>{})
	void clearAxisCallback();

	virtual ~Joystick();
};

}
#endif /* JOYSTICK_H_ */
