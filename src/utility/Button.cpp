/*----------------------------------------------------------------------*
 * Arduino Button Library v1.0                                          *
 * Jack Christensen May 2011, published Mar 2012                        *
 *                                                                      *
 * Library for reading momentary contact switches like tactile button   *
 * switches. Intended for use in state machine constructs.              *
 * Use the read() function to read all buttons in the main loop,        *
 * which should execute as fast as possible.                            *
 *                                                                      *
 * This work is licensed under the Creative Commons Attribution-        *
 * ShareAlike 3.0 Unported License. To view a copy of this license,     *
 * visit http://creativecommons.org/licenses/by-sa/3.0/ or send a       *
 * letter to Creative Commons, 171 Second Street, Suite 300,            *
 * San Francisco, California, 94105, USA.                               *
 *----------------------------------------------------------------------*/

#include "Button.h"

/*----------------------------------------------------------------------*
 * Button(pin, puEnable, invert, dbTime) instantiates a button object.  *
 * pin      Is the Arduino pin the button is connected to.              *
 * puEnable Enables the AVR internal pullup resistor if != 0 (can also  *
 *          use true or false).                                         *
 * invert   If invert == 0, interprets a high state as pressed, low as  *
 *          released. If invert != 0, interprets a high state as        *
 *          released, low as pressed  (can also use true or false).     *
 * dbTime   Is the debounce time in milliseconds.                       *
 *                                                                      *
 * (Note that invert cannot be implied from puEnable since an external  *
 *  pullup could be used.)                                              *
 *----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*
/ initialize a Button object and the pin it's connected to.             *
/-----------------------------------------------------------------------*/
void Button::begin() {
    pinMode(m_pin, m_puEnable ? INPUT_PULLUP : INPUT);
    m_state = digitalRead(m_pin);
    if (m_invert) m_state = !m_state;
    m_time       = millis();
    m_lastState  = m_state;
    m_changed    = false;
    m_lastChange = m_time;
}

/*----------------------------------------------------------------------*
/ returns the state of the button, true if pressed, false if released.  *
/ does debouncing, captures and maintains times, previous state, etc.   *
/-----------------------------------------------------------------------*/
bool Button::read() {
    uint32_t ms = millis();
    bool pinVal = digitalRead(m_pin);
    if (m_invert) pinVal = !pinVal;
    if (ms - m_lastChange < m_dbTime) {
        m_changed = false;
    } else {
        m_lastState = m_state;
        m_state     = pinVal;
        m_changed   = (m_state != m_lastState);
        if (m_changed) m_lastChange = ms;
    }
    m_time = ms;
    return m_state;
}

/*----------------------------------------------------------------------*
 * isPressed() and isReleased() check the button state when it was last *
 * read, and return false (0) or true (!=0) accordingly.                *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
bool Button::isPressed() {
    return m_state;
}

bool Button::isReleased() {
    return !m_state;
}

/*----------------------------------------------------------------------*
 * wasPressed() and wasReleased() check the button state to see if it   *
 * changed between the last two reads and return false (0) or           *
 * true (!=0) accordingly.                                              *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
bool Button::wasPressed() {
    return m_state && m_changed;
}

bool Button::wasReleased() {
    return !m_state && m_changed;
}

/*----------------------------------------------------------------------*
 * pressedFor(ms) and releasedFor(ms) check to see if the button is     *
 * pressed (or released), and has been in that state for the specified  *
 * time in milliseconds. Returns false (0) or true (!=0) accordingly.   *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
bool Button::pressedFor(uint32_t ms) {
    return m_state && m_time - m_lastChange >= ms;
}

bool Button::releasedFor(uint32_t ms) {
    return !m_state && m_time - m_lastChange >= ms;
}

/*----------------------------------------------------------------------*
 * lastChange() returns the time the button last changed state,         *
 * in milliseconds.                                                     *
 *----------------------------------------------------------------------*/
uint32_t Button::lastChange() {
    return m_lastChange;
}
