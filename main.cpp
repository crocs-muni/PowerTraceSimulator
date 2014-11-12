#include <trace.h>

const unsigned char KEY = 0x51;

/**
 * From specific Trace creates number of files, each Trace contains peak in specific place and range, key is set, data are random generated
 * @brief createTracesFile
 * @param myTrace
 * @param number
 * @param path
 * @param key
 * @param position
 * @param width
 * @param height
 * @param noise
 * @return
 */
int createTracesFile(Trace myTrace, const int& number, const string& path, const unsigned char& key, const int& position, const int& width, const int& height, const int& noise){
    if (((position+width/2) > myTrace.getSize()) || (position-width/2<0))
        return RE;

    ofstream file(path+"_record.dat");
    int control = OK;

    for(int i=0; i<number; i++){
        Trace temp = myTrace;
        unsigned char data = rand()%256;
        control = temp.addPeak(key, data, position, width, height);
        control = temp.addRandomNoise(0,temp.getSize()-1,noise);
        control = temp.saveToFile(path+to_string(i)+".dat");
        file << "0x" << hex << setw(2) << uppercase << (int)data << " = " << path+to_string(i)+".dat" << endl;
    }
    file.close();
    return control;
}

/**
 * From specific Trace creates Vector of Traces, each Trace contains peak in specific place and range, key is set, data are random generated
 * @brief createTracesVector
 * @param myTrace
 * @param number
 * @param Traces
 * @param key
 * @param position
 * @param width
 * @param height
 * @param noise
 * @return
 */
int createTracesVector(const Trace& myTrace, const int& number, vector <Trace>& traces, const unsigned char& key, const int& position, const int& width, const int& height, const int& noise, const int& offs){
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
        control = temp.setData(data);
        control = temp.applyOffsets(offsets);
        traces.push_back(temp);
    }
    return control;
}


/**
 * Export Vector of Traces to the Files
 * @brief saveTracesFile
 * @param Traces
 * @param path
 * @return
 */
int saveTracesFile(const vector <Trace> &Traces, const string& path){
    int control = OK;
    for(unsigned int i=0; i<Traces.size(); i++)
        control = Traces.at(i).saveToFile(path+to_string(i)+".dat");
    return control;
}


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

int dpa(const vector <Trace> &Traces, vector <Trace> &diffTraces){
    for(unsigned int i=0; i<256; i++){
        Trace temp;
        dpaKey(Traces, i, temp);
        diffTraces.push_back(temp);
    }
    return OK;
}

/**
 * Find maximum value from Vector of Traces, return number of Trace which contains highes value
 * @brief findMax
 * @param Traces
 * @param position
 * @return
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


int fitness(const vector <Trace> &Traces, int& position, const int& radius){
    /*  int radius = 2;
    int maxValue = INT_MIN;
    int maxWide = INT_MIN;
    for(unsigned int i=0; i<Traces.size(); i++){
        for(int j=radius; j<Traces.at(i).getSize()-radius; j++){
            int temp = 0;
            for(int k=j-radius; k<=j+radius; k++)
                temp += Traces.at(i).getValue(k);
            temp = temp/(2*radius+1);
            if(temp>maxValue){
                maxValue=temp;
                positionV=i;
            }
        }
    }
*/
    int fitnessValue = INT_MIN;
    for(unsigned int i=0; i<Traces.size(); i++){
        for(int j=radius; j<Traces.at(i).getSize()-radius; j++){
            int temp = 0;
            for(int k=j-radius; k<=j+radius; k++)
                temp += Traces.at(i).getValue(k);
            if(temp>fitnessValue){
                fitnessValue=temp;
                position=i;
            }
        }
    }
    return fitnessValue;
}

int moveTraces(vector <Trace> &Traces, Trace &offsets){
    for(unsigned int i=0; i<Traces.size(); i++)
        (offsets.getValue(i)>0) ? Traces.at(i).moveRight(offsets.getValue(i),1) : Traces.at(i).moveLeft(-offsets.getValue(i),1);
    return OK;
}


int shake(vector <Trace> &Traces, const int noise, Trace &offsets){
    if (noise == 0) return OK;

    for(unsigned int i=0; i<Traces.size(); i++){
        int move = rand()%noise+1;
        if (rand()%2 == 0) {
            Traces.at(i).moveRight(move,1);
            offsets.addValue(move);
        } else {
            Traces.at(i).moveLeft(move,1);
            offsets.addValue(-move);
        }

        Traces.at(i).cutFront(noise);
        Traces.at(i).cutEnd(noise);
    }
    return OK;
}


void test(){
    srand (time(NULL));
    Trace skuska("data.dat");
    // skuska.cutBottom();

    int control = OK;
    int noise = 50;
    int sum = 100;
    int shakeValue = 5; //to effect must be higher than 1
    int offsets = 0;
    int radius = 5;
    int numOfTraces = 1000;

    ofstream file("D:\\shake0_off0.txt");

    file << "Used base Trace: " << "D:\\dataout4.dat" << endl;
    file << "Number of generated Traces with peak: " << numOfTraces << endl;
    file << "Used key: 0x" << hex << uppercase << (int)KEY << endl;
    file << "Position where peak is added: 100" << endl;
    file << "Max height of peak 100" << endl;
    file << "Width of peak: 10" << endl;
    file << "Used max random offsets value (random move for each value): " << offsets << endl;
    file << "Used shake value(random move to right): " << shakeValue << endl << endl;
    file << "BEGIN TEST" << endl;
    file << "==========" << endl << endl << endl;

    for (int j=0; j<5; j++){
        int ok = 0;
        for (int i=0; i<sum; i++){
            vector <Trace> array;
            control = createTracesVector(skuska,numOfTraces,array,KEY,100,10,100,noise,offsets);
            Trace offs;
            vector <Trace> result;
            control = shake(array, shakeValue, offs);
            control = dpa(array,result);
            int position;
            fitness(result, position, radius);
            if(position == (int)KEY)
                ok++;
        }

        file << "Number of tryes: " << dec << sum << endl;
        file << "Used noise: "  << noise << endl;
        file << "Success: " << (float)ok/sum*100 << "%" << endl;
        file << "---------------------------------" << endl << endl;

        noise += 20;
        cout << j+1 << ". done." << endl;
    }
    file.close();
    (control == OK) ? (cout << "Done without problems!" << endl) : (cout << "Done with ERRORS!" << endl);
}

void testSec(){
    srand (time(NULL));
    Trace skuska("D:\\lab\\PowerTraceSimulator\\data.dat");
    int radius = 5;
    vector <Trace> array;
    createTracesVector(skuska,500,array,KEY,100,10,150,0,0);
    Trace offsets;
    vector <Trace> result;
    shake(array, 5, offsets);

    /* offsets.saveToFile("D:\\offs.txt");
    offsets.inverseValues();
    offsets.saveToFile("D:\\offsIN.txt");
    moveTraces(array, offsets);
   */

    Trace random(offsets.getSize(),0);
    random.addRandomNoise(0,random.getSize()-1,5);
    //  moveTraces(array, random);

    dpa(array,result);
    int position;
    cout << "Fitness Value is: " << fitness(result, position, radius) << endl;
    result.at(KEY).saveToFile("D:\\dpa.txt");
    (position == (int)KEY) ? cout << "Key found" << endl : cout << "Key NOT found" << endl;


    //Trace sk(skuska.getSize(),0);
    //sk.addOffsets(20);
    //skuska.cutToSize(40);
    //skuska.cutBottom();
    //skuska.addOffsets(5);
    //skuska.addPeak(0xFF,0x51,100,10,10000);
    //skuska.saveToFile("D:\\before.txt");
    //skuska.addPeak(KEY,KEY^0xFF,25,10,50);
    //skuska.moveLeft(10);

    //skuska.moveLeft(5,1);
    //skuska.saveToFile("D:\\left.txt");
    //skuska.moveLeft(10,1);
    //skuska.saveToFile("D:\\right.txt");
    //Trace temp(40,0);
    //temp.addOffsets(5);
    //skuska.applyOffsets(temp);
    //skuska.saveToFile("D:\\after.txt");

    //skuska.addRandomNoise(20,30,200);
    //skuska.saveToFile("D:\\after.txt");
    //skuska.applyOffsets(sk);
    //skuska.saveToFile("D:\\after.txt");
    //sk.saveToFile("D:\\offsets.txt");
}

void testT(){
    Trace skuska(50,10);
    Trace dalsia(50,20);

    (skuska-dalsia).writeValues(0,49);


}

int main(){
   // test();
        clock_t elapsed = -clock();
        test();
        elapsed += clock();
        cout << "Without exceptions: " << elapsed * 1000 / CLOCKS_PER_SEC << " ms" << endl;

    //  testSec();

    //testT();

    return OK;
}
