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
    unsigned char data;
    vector<int> values;

public:

    Trace();

    Trace(const string& path);

    Trace(const int& size, const int& value);

    Trace(const Trace &second);

    /**
     * Set value of specific point of Trace
     * @brief setValue
     * @param position
     * @param value
     * @return
     */
    int setValue(const unsigned int& position, const int &value);

    int getValue(const int& position) const;

    vector<int> getValues() const;

    Trace operator+(const Trace& second);

    Trace operator-(const Trace& second);

    int addition(const Trace& second);

    int subtraction(const Trace& second);

    int inverseValues();

    /**
     * Writes all values of Trace to std. output
     * @brief writeValues
     * @param start
     * @param end
     */
    void writeValues(const int& start, const int& end) const;

    void writeValues() const;

    /**
     * Add value at the end of Trace
     * @brief addValue
     * @param value
     */
    void addValue(const int& value);

    /**
     * Load values of Trace from specific file, EACH VALUE ON EACH LINE!
     * @brief readFile
     * @param path
     * @return
     */
    int readFile(const string& path);

    /**
     * Save values of Trace to the file, 1 VALUE ON 1 LINE
     * @brief saveToFile
     * @param path
     * @return
     */
    int saveToFile(const string &path) const;

    int getSize() const;

    /**
     * Find the lowest value and subtract this value from all values. The lowes value is 0
     * @brief cutBottom
     * @return
     */
    int cutBottom();

    int cutToSize(const int& size);

    int cutEnd(const int& number);

    int cutFront(const int& number);

    /**
     * Add random value to each VALUE of the Trace in specific range
     * @brief addRandomNoise
     * @param start
     * @param end
     * @param noise
     * @return
     */
    int addRandomNoise(const int& start, const int& end, const int& noise);

    /**
    * Add peak to the Trace, using Hamming weight
    * @brief addPeak
    * @param key
    * @param data
    * @param position
    * @param width
    * @param height
    * @param noise
    * @return
    */
    int addPeak(const unsigned char key, const unsigned char data, const int position, const int width, const int height);

    int moveRight(const int value, const int param);

    int moveLeft(const int value, const int param);

    int moveUp(const int value);

    int moveDown(const int value);

    unsigned char getData() const;

    int setData(const unsigned char data);

    int addOffsets(const int max);

    int applyOffsets(const Trace& offsets);
};

#endif // TRACE_H
