#ifndef TRACE_H
#define TRACE_H

#include <limits.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <math.h>
#include <iomanip>
#include <other.cpp>
#include <stdexcept>

const unsigned int OK = 0;  //Correct State
const unsigned int E = 1;   //General Error
const unsigned int RE = 2;  //Range Error
const unsigned int WP = 3;  //Wrong Parameter

/**
 * Represents Trace which contains values and used data.
 * @brief The Trace class
 */
class Trace{

protected:
    /**
     * @brief data - value where you can store information about KEY
     */
    unsigned char data;

    /**
     * @brief values - vector of values that represents trace
     */
    vector<int> values;

public:

    /**
     * @brief Trace - default constructor
     */
    Trace();

    /**
     * @brief Trace - create trace from file
     * @param path - path to the file
     */
    Trace(const string& path);

    /**
     * @brief Trace - create trace of specific size, each value of trace is set to specific value
     * @param size - size of new trace
     * @param value - value to be set at each value of trace
     */
    Trace(const int& size, const int& value);

    /**
     * @brief Trace - copy constructor
     * @param second - trace to be copied
     */
    Trace(const Trace &second);

    /**
     * @brief Set value of specific point of Trace
     * @param position
     * @param value
     * @return information about the success of operation
     */
    int setValue(const unsigned int& position, const int &value);

    /**
     * @brief getValue - Return value of Trace at specific position
     * @param position
     * @return specific value
     */
    int getValue(const int& position) const;

    /**
     * @brief operator + Addition of two traces
     * @param second - trace to be added
     * @return - Trace that contain addition of two traces
     */
    Trace operator+(const Trace& second);

    /**
     * @brief operator - Subtraction of two traces
     * @param second - trace to be subtract
     * @return Trace that contain subtraction of two traces
     */
    Trace operator-(const Trace& second);

    /**
     * @brief addition - add Trace to this trace
     * @param second - trace to be added to this trace
     * @return information about the success of operation
     */
    int addition(const Trace& second);

    /**
     * @brief subtraction - subtract Trace from this trace
     * @param second - trace to be subtract from this trace
     * @return information about the success of operation
     */
    int subtraction(const Trace& second);

    /**
     * @brief inverseValues - each value of trace is inverted (value = -value)
     * @return information about the success of operation
     */
    int inverseValues();

    /**
     * @brief writeValues - writes values of Trace in specific range to std. output
     * @param start - first value
     * @param end - last value to be written
     */
    void writeValues(const int& start, const int& end) const;

    /**
     * @brief writeValues - writes all values of Trace to std. output
     */
    void writeValues() const;

    /**
     * @brief addValue - Add value at the end of Trace
     * @param value - value to be added
     */
    void addValue(const int& value);

    /**
     * @brief readFile - Load values of Trace from specific file, format: EACH VALUE AT EACH LINE!
     * @param path - path to the file
     * @return information about the success of operation
     */
    int readFile(const string& path);

    /**
     * Save values of Trace to the file, 1 VALUE AT 1 LINE
     * @brief saveToFile
     * @param path
     * @return
     */
    int saveToFile(const string &path) const;

    /**
     * @brief getSize - return number of values of Trace
     * @return - number of values
     */
    int getSize() const;

    /**
     * @brief cutBottom - Find the lowest value and subtract this value from all values. The lower value is 0
     * @return information about the success of operation
     */
    int cutBottom();

    /**
     * @brief cutToSize - First "size" values are untouched, other are deleted from Trace
     * @param size - new size of Trace
     * @return information about the success of operation
     */
    int cutToSize(const int& size);

    /**
     * @brief cutEnd - detele last "number" values of Trace
     * @param number - number of values to be deleted
     * @return information about the success of operation
     */
    int cutEnd(const int& number);

    /**
     * @brief cutFront - detele first "number" values of Trace
     * @param number - number of values to be deleted
     * @return information about the success of operation
     */
    int cutFront(const int& number);

    /**
     * @brief addRandomNoise - Add or subtract random value to each VALUE of the Trace in specific range
     * @param start
     * @param end
     * @param noise - max random value
     * @return information about the success of operation
     */
    int addRandomNoise(const int& start, const int& end, const int& noise);

    /**
    * @brief addPeak - Add peak to the Trace, with specific width and height placed at specific position. Use Hamming distibution.
    * @param key - key used to generate peak.
    * @param data - data used to generate peak.
    * @param position - position where peak is added (width/2 =< position < size of Trace-width/2).
    * @param width - width of peak (Gaussian distribution is used).
    * @param height - maximum height of peak (maximum is when KEY^DATA = 0xFF, minimum when KEY^DATA = 0x00).
    * @return information about the success of operation
    */
    int addPeak(const unsigned char key, const unsigned char data, const int position, const int width, const int height);

    /**
     * @brief moveRight - shift all values to right
     * @param value - how much values should be shifted
     * @param param - MODE 0 - All values are intact; MODE 1 - boundary values are overwritten
     * @return information about the success of operation
     */
    int moveRight(const int value, const int param);

    /**
     * @brief moveLeft - shift all values to left
     * @param value - how much values should be shifted
     * @param param - MODE 0 - All values are intact; MODE 1 - boundary values are overwritten
     * @return information about the success of operation
     */
    int moveLeft(const int value, const int param);

    /**
     * @brief moveUp - to all values of trace is added "value"
     * @param value
     * @return information about the success of operation
     */
    int moveUp(const int value);

    /**
     * @brief moveDown - subtract "value" from all values of Trace
     * @param value
     * @return information about the success of operation
     */
    int moveDown(const int value);

    /**
     * @brief getData - return attribute data of class Trace
     * @return data
     */
    unsigned char getData() const;

    /**
     * @brief setData - set attribute data of class Trace
     * @param data
     */
    void setData(const unsigned char data);

    /**
     * @brief addOffsets - Method creates vector of micro offsets. This vector is later applied to the main Trace.
     * @param max - specify the maximum offset of single value of main Trace. Problem with displacement out of range is solved.
     * @return information about the success of operation
     */
    int addOffsets(const int max);

    /**
     * @brief applyOffsets - Method replaces values of main Trace. Values of offsets are in the micro offset Trace (vector).
     * @param offsets - vector created using addOffsets(). Problem of displacement out of range is solved during creating micro offset Trace.
     * @return information about the success of operation
     */
    int applyOffsets(const Trace& offsets);
};

#endif // TRACE_H
