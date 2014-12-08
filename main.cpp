#include <trace.h>

//Set constant KEY
const unsigned char KEY = 0x51;

/**
 * @brief createTracesFile - From specific Trace creates number of files, each Trace contains peak at specific place and range, key is set, data are random.
 *                           With the curves files is generated record file that contains information about individual keys which were used to generating peaks
 * @param myTrace - base Trace; each trace in Vector is based on this trace and peak
 * @param number - number of generated traces (recommended: 500 and more)
 * @param path - path where traces (files) will be stored - OPTIONAL: create new directory
 * @param key - KEY used to generate peaks
 * @param position - position where peak is added (width/2 =< position < size of Trace-width/2)
 * @param width - width of peak (Gaussian distribution is used)
 * @param height - maximum height of peak (maximum is when KEY^DATA = 0xFF, minimum when KEY^DATA = 0x00
 * @param noise - maximum value of noise which is distributed to all traces
 * @return - information about the success of operation
 */
int createTracesFile(Trace myTrace, const int& number, const string& path, const unsigned char& key, const int& position, const int& width, const int& height, const int& noise){
    if (((position+width/2) > myTrace.getSize()) || (position-width/2<0))
        return RE;

    ofstream file(path+"_record.dat"); //name of output trace (file)
    int control = OK;

    for(int i=0; i<number; i++){
        Trace temp = myTrace;
        unsigned char data = rand()%256;
        control = temp.addPeak(key, data, position, width, height);
        control = temp.addRandomNoise(0,temp.getSize()-1,noise);
        control = temp.saveToFile(path+"_"+to_string(i)+".dat");
        file << "0x" << hex << setw(2) << uppercase << (int)data << " = " << path+to_string(i)+".dat" << endl;
    }
    file.close();
    return control;
}

/**
 * @brief generateTraces - Create a Vector which contains Traces. Each Trace is unique and contains one peak with specific width and height placed at specific position.
 * @param myTrace - base Trace; each trace in Vector is based on this trace and peak
 * @param number - number of generated traces (recommended: 500 and more)
 * @param traces - vector where are generated traces saved
 * @param key - KEY used to generate peaks
 * @param position - position where peak is added (width/2 =< position < size of Trace-width/2)
 * @param width - width of peak (Gaussian distribution is used)
 * @param height - maximum height of peak (maximum is when KEY^DATA = 0xFF, minimum when KEY^DATA = 0x00
 * @param noise - maximum value of noise which is distributed to all traces
 * @return - information about the success of operation
 */
int generateTraces(const Trace& myTrace, const int& number, vector <Trace>& traces, const unsigned char& key, const int& position, const int& width, const int& height, const int& noise){
    if (((position+width/2) > myTrace.getSize()) || (position-width/2<0))
        return RE;

    int control = OK;
    for(int i=0; i<number; i++){
        Trace temp = myTrace;
        unsigned char data = rand()%256;
        control = temp.addPeak(key, data, position, width, height);
        control = temp.addRandomNoise(0,temp.getSize()-1,noise);
        temp.setData(data);
        traces.push_back(temp);
    }
    return control;
}

/**
 * @brief generateTraces - Same behavior as the method above + add random micro offsets (described in the wiki)
 * @param myTrace - base Trace; each trace in Vector is based on this trace and peak
 * @param number - number of generated traces (recommended: 500 and more)
 * @param traces - vector where are generated traces saved
 * @param key - KEY used to generate peaks
 * @param position - position where peak is added (width/2 =< position < size of Trace-width/2)
 * @param width - width of peak (Gaussian distribution is used)
 * @param height - maximum height of peak (maximum is when KEY^DATA = 0xFF, minimum when KEY^DATA = 0x00
 * @param noise - maximum value of noise which is distributed to all traces
 * @param offs - maximum value of one micro offset
 * @return - information about the success of operation
 */
int generateTraces(const Trace& myTrace, const int& number, vector <Trace>& traces, const unsigned char& key, const int& position, const int& width, const int& height, const int& noise, const int& offs){
    if (((position+width/2) > myTrace.getSize()) || (position-width/2<0))
        return RE;

    int control = OK;

    Trace offsets(myTrace.getSize(),0);
    offsets.addOffsets(offs);

    for(int i=0; i<number; i++){
        Trace temp = myTrace;
        unsigned char data = rand()%256;
        control = temp.addPeak(key, data, position, width, height);
        control = temp.addRandomNoise(0,temp.getSize()-1,noise);
        temp.setData(data);
        control = temp.applyOffsets(offsets);
        traces.push_back(temp);
    }
    return control;
}

/**
 * @brief saveTracesFile - Export Vector of Traces to files
 * @param Traces - Traces to be exported
 * @param path - path to folder + name of exported file
 * @return information about the success of operation
 */
int saveTracesFile(const vector <Trace> &Traces, const string& path){
    int control = OK;
    for(unsigned int i=0; i<Traces.size(); i++)
        control = Traces.at(i).saveToFile(path+"_"+to_string(i)+".dat");
    return control;
}

/**
 * @brief dpaKey - Perform DPA (differential power analysis) at Vector of Traces with specific estimated KEY
 * @param Traces - input Vector of Traces
 * @param key - estimated KEY
 * @param diff - output differential Trace
 * @return information about the success of operation
 */
int dpaKey(const vector <Trace> &Traces, const unsigned int& key, Trace &diff){

    int sumLow = 0;
    int sumHigh = 0;
    Trace low(Traces.at(0).getSize(),0);
    Trace high(Traces.at(0).getSize(),0);

    for(unsigned int i=0; i<Traces.size(); i++){
        if(bitCount(Traces.at(i).getData()^key) < 4){
            low.addition(Traces.at(i));
            sumLow++;
        } else {
            high.addition(Traces.at(i));
            sumHigh++;
        }
    }

    for(int i=0; i<low.getSize(); i++)
        low.setValue(i,low.getValue(i)/sumLow);

    for(int i=0; i<high.getSize(); i++)
        high.setValue(i,high.getValue(i)/sumHigh);

    high.subtraction(low);
    diff = high;
    diff.setData(key);
    return OK;
}

/**
 * @brief dpa - Perform DPA (differential power analysis) at Vector of Traces with all KEYS (0x00 - 0xFF)
 * @param Traces - input Vector of Traces
 * @param diffTraces - vector of output differential Traces (as parameter use empty vector <Trace>)
 * @return information about the success of operation
 */
int dpa(const vector <Trace> &Traces, vector <Trace> &diffTraces){
    for(unsigned int i=0; i<256; i++){
        Trace temp;
        dpaKey(Traces, i, temp);
        diffTraces.push_back(temp);
    }
    return OK;
}

/**
 * @brief findKey - basic KEY finder function, which finds higher value in all output differential Traces
 * @param Traces - vector of output differential Traces (should be 256 Traces)
 * @param key - found KEY
 * @return information about the success of operation
 */
int findKey(const vector <Trace>& Traces, int& key){
    int temp= INT_MIN;
    for(unsigned int i=0; i<Traces.size(); i++)
        for(int j=0; j<Traces.at(i).getSize(); j++)
            if(Traces.at(i).getValue(j) > temp){
                temp=Traces.at(i).getValue(j);
                key = i;
            }
    return OK;
}

/**
 * @brief fitness - better KEY finder function, which optimize all output differential Traces
 * @param Traces - vector of output differential Traces (should be 256 Traces)
 * @param key - found KEY
 * @param radius - a level of optimization (OPTIONAL: 1-5)
 * @return - fitnessValue (quality of result - for more advanced users)
 */
int fitness(const vector <Trace> &Traces, int& key, const int& radius){
    int fitnessValue = INT_MIN;
    for(unsigned int i=0; i<Traces.size(); i++){
        for(int j=radius; j<Traces.at(i).getSize()-radius; j++){
            int temp = 0;
            for(int k=j-radius; k<=j+radius; k++)
                temp += Traces.at(i).getValue(k);
            if(temp>fitnessValue){
                fitnessValue=temp;
                key=i;
            }
        }
    }
    return fitnessValue;
}


/**
 * @brief shake - Random move all Traces to the right or left.
 * @param Traces - Traces to be moved
 * @param noise - maximum value of moving
 * @return information about the success of operation
 */
int shake(vector <Trace> &Traces, const int noise){
    if (noise == 0) return OK;
    if (noise > (Traces.at(0).getSize()/4)) return RE;

    int control = OK;

    for(unsigned int i=0; i<Traces.size(); i++){
        int move = rand()%noise+1;
        if (rand()%2 == 0)
            control = Traces.at(i).moveRight(move,1);
        else
            control = Traces.at(i).moveLeft(move,1);

        control = Traces.at(i).cutFront(noise);
        control = Traces.at(i).cutEnd(noise);
    }
    return control;
}

/**
 * @brief shake - Random move all Traces to the right or left. And store offsets to Trace.
 * @param Traces - Traces to be moved
 * @param noise - maximum value of moving
 * @param offsets - Trace where offsets will be stored (as parameter use empty Trace)
 * @return information about the success of operation
 */
int shake(vector <Trace> &Traces, const int noise, Trace &offsets){
    if (noise == 0) return OK;
    if (noise > (Traces.at(0).getSize()/4)) return RE;
    int control = OK;

    for(unsigned int i=0; i<Traces.size(); i++){
        int move = rand()%noise+1;
        if (rand()%2 == 0) {
            control = Traces.at(i).moveRight(move,1);
            offsets.addValue(move);
        } else {
            control = Traces.at(i).moveLeft(move,1);
            offsets.addValue(-move);
        }

        control = Traces.at(i).cutFront(noise);
        control = Traces.at(i).cutEnd(noise);
    }
    return control;
}

/**
 * @brief moveTraces - Move all Traces to the right or left. Offset values are in offsets Trace. The inversion method to Shake method.
 * @param Traces - Traces to be moved (shifted)
 * @param offsets - Trace contains values of moving
 * @return information about the success of operation
 */
int moveTraces(vector <Trace> &Traces, Trace &offsets){
    for(unsigned int i=0; i<Traces.size(); i++)
        (offsets.getValue(i)>0) ? Traces.at(i).moveRight(offsets.getValue(i),1) : Traces.at(i).moveLeft(-offsets.getValue(i),1);
    return OK;
}


int main(){
    srand (time(NULL));  // For different values of random each time you run program.

    //YOUR CODE
    //You can set constant KEY at the beginning of this file.

    return OK;
}
