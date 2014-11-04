#ifndef TRACE_H
#define TRACE_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <math.h>
#include <iomanip>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <other.cpp>


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

    Trace(string path);

    Trace(int size, int value);

    Trace(const Trace &second);

    /**
     * Set value of specific point of Trace
     * @brief setValue
     * @param position
     * @param value
     * @return
     */
    int setValue(int position, int value);

    int getValue(int position);

    vector<int> getValues();

    Trace operator+(Trace anotherOne);

    Trace operator-(Trace anotherOne);

    int addTo(Trace second);

    int fromTo(Trace second);
    /**
     * Writes all values of Trace to std. output
     * @brief writeValues
     * @param start
     * @param end
     */
    void writeValues(int start, int end);

    /**
     * Add value at the end of Trace
     * @brief addValue
     * @param value
     */
    void addValue(int value);

    /**
     * Load values of Trace from specific file, EACH VALUE ON EACH LINE!
     * @brief readFile
     * @param path
     * @return
     */
    int readFile(string path);

    /**
     * Save values of Trace to the file, 1 VALUE ON 1 LINE
     * @brief saveToFile
     * @param path
     * @return
     */
    int saveToFile(string path);

    int getSize();

    /**
     * Find the lowest value and subtract this value from all values. The lowes value is 0
     * @brief cutBottom
     * @return
     */
    int cutBottom();

    int cutToSize(int size);

    int cutEnd(int number);

    int cutFront(int number);

    /**
     * Add random value to each VALUE of the Trace in specific range
     * @brief addRandomNoise
     * @param start
     * @param end
     * @param noise
     * @return
     */
    int addRandomNoise(int start, int end, int noise);

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
    int addPeak(unsigned char key, unsigned char data, int position, int width, int height);

    int moveRight(int value, int param);

    int moveLeft(int value, int param);

    int moveUp(int value);

    int moveDown(int value);

    unsigned char getData();

    int setData(unsigned char data);

    int addOffsets(const int max);

    int applyOffsets(Trace offsets);
};

#endif // TRACE_H
