#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <math.h>
#include <iomanip>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <trace.h>
#include <limits.h>

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
int createTracesFile(Trace myTrace, int number, string path, unsigned char key, int position, int width, int height, int noise){
    if ((position+width/2) > myTrace.getSize()) {return 1;}
    ofstream file(path+"_record.dat");
    for(int i=0; i<number; i++){
        Trace temp = myTrace;
        unsigned char data = rand()%256;
        temp.addPeak(key, data, position, width, height);
        temp.addRandomNoise(0,temp.getSize()-1,noise);
        temp.saveToFile(path+to_string(i)+".dat");
        file << "0x" << hex << setw(2) << uppercase << (int)data << " = " << path+to_string(i)+".dat" << endl;
    }
    file.close();
    return 0;
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
int createTracesVector(Trace myTrace, int number, vector <Trace> * Traces, unsigned char key, int position, int width, int height, int noise, int offs){
    if ((position+width/2) > myTrace.getSize()) {return 1;}
    Trace offsets(myTrace.getSize(),0);
    offsets.addOffsets(offs);
    //offsets.saveToFile("D:\\save.txt");

    for(int i=0; i<number; i++){
        Trace temp = myTrace;
        unsigned char data = rand()%256;
        temp.addPeak(key, data, position, width, height);
        temp.addRandomNoise(0,temp.getSize()-1,noise);
        temp.setData(data);
       // temp.applyOffsets(offsets);
        Traces->push_back(temp);
    }
    return 0;
}


/**
 * Export Vector of Traces to the Files
 * @brief saveTracesFile
 * @param Traces
 * @param path
 * @return
 */
int saveTracesFile(vector <Trace> * Traces, string path){
    for(unsigned int i=0; i<Traces->size(); i++){
        Traces->at(i).saveToFile(path+to_string(i)+".dat");
    }
    return 0;
}


int dpaKey(vector <Trace> * Traces, unsigned int key, Trace& diff){

    int sumLow = 0;
    int sumHigh = 0;
    Trace low(Traces->at(0).getSize(),0);
    Trace high(Traces->at(0).getSize(),0);

    for(unsigned int i=0; i<Traces->size(); i++){
        if(bitCount(Traces->at(i).getData()^key) < 4){
            low.addTo(Traces->at(i));
            sumLow++;
        } else {
            high.addTo(Traces->at(i));
            sumHigh++;
        }
    }

    for(int i=0; i<low.getSize(); i++)
        low.setValue(i,low.getValue(i)/sumLow);

    for(int i=0; i<high.getSize(); i++)
        high.setValue(i,high.getValue(i)/sumHigh);

    high.fromTo(low);
    diff = high;
    diff.setData(key);
        return 0;
}

int dpa(vector <Trace> * Traces, vector <Trace> * diffTraces){
    for(unsigned int i=0; i<256; i++){
        Trace temp;
        dpaKey(Traces, i, temp);
        diffTraces->push_back(temp);
    }
    return 0;
}

/**
 * Find maximum value from Vector of Traces, return number of Trace which contains highes value
 * @brief findMax
 * @param Traces
 * @param position
 * @return
 */
int fitnessBasic(vector <Trace> * Traces, int *position){
    int temp= INT_MIN;
    for(unsigned int i=0; i<Traces->size(); i++){
        for(int j=0; j<Traces->at(i).getSize(); j++){
            if(Traces->at(i).getValue(j) > temp){
                temp=Traces->at(i).getValue(j);
                *position = i;
            }
        }
    }
    return 0;
}


int fitness(vector <Trace> &Traces, int &positionV, int &positionW){
    int radius = 2;
    int maxValue = -9999999;
    int maxWide = -999999;
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

    radius = 5;
    for(unsigned int i=0; i<Traces.size(); i++){
        for(int j=radius; j<Traces.at(i).getSize()-radius; j++){
            int temp = 0;
            for(int k=j-radius; k<=j+radius; k++)
                temp += Traces.at(i).getValue(k);
            if(temp>maxWide){
                maxWide=temp;
                positionW=i;
            }            
        }
    }
    return maxWide;
}

int shake(vector <Trace> * Traces, const int noise){
    if ((noise == 0)||(noise== 1)) return 0;

    int temp = Traces->at(0).getSize();
    for(unsigned int i=0; i<Traces->size(); i++){
        int move = rand()%noise;
        (rand()%2 == 0) ? Traces->at(i).moveRight(move,1) : Traces->at(i).moveLeft(move,1);
        Traces->at(i).cutToSize(temp);
    }
    return 0;
}


void test(){
    srand (time(NULL));
    Trace skuska("D:\\dataout4.dat");
    skuska.cutBottom();

    int noise = 300;
    int sum = 100;
    int shakeValue = 0; //to effect must be higher than 1
    int offsets = 0;

    ofstream file("D:\\offsets\\shake0_off0.txt");

    file << "Used base Trace: " << "D:\\dataout4.dat" << endl;
    file << "Number of generated Traces with peak: 500" << endl;
    file << "Used key: 0x" << hex << uppercase << (int)KEY << endl;
    file << "Position where peak is added: 100" << endl;
    file << "Max height of peak 100" << endl;
    file << "Width of peak: 10" << endl;
    file << "Used max random offsets value (random move for each value): " << offsets << endl;
    file << "Used shake value(random move to right): " << shakeValue << endl << endl;
    file << "BEGIN TEST" << endl;
    file << "==========" << endl << endl << endl;

    for (int j=0; j<3; j++){
        int okv = 0;
        int okw = 0;
        int together = 0;


        for (int i=0; i<sum; i++){
            vector <Trace> array;
            createTracesVector(skuska,500,&array,KEY,100,10,100,noise,offsets);

            vector <Trace> result;
            shake(&array, shakeValue);
            dpa(&array,&result);

            int positionV;
            int positionW;

            fitness(result, positionV, positionW);
            //  cout << "Searched key by Better is: 0x" << hex << uppercase << position <<endl;
            if(positionV == (int)KEY)
                okv++;
            if(positionW == (int)KEY)
                okw++;

            if((positionV == (int)KEY) && (positionW == (int)KEY))
                together++;
        }


        file << "Number of tryes: " << dec << sum << endl;
        file << "Used noise: "  << noise << endl;
        file << "MaxValue: " << (float)okv/sum*100 << "%" << endl;
        file << "MaxWidth: " << (float)okw/sum*100 << "%" << endl;
        file << "MaxVal+Width: " << (float)together/sum*100 << "%" <<  endl;
        file << "---------------------------------" << endl << endl;

        noise += 15;
        cout << j+1 << ". done." << endl;
    }
    file.close();
    cout << "Done!" << endl;
}

void testSec(){
    srand (time(NULL));
    Trace skuska("D:\\dataout4.dat");


   // Trace sk(skuska.getSize(),0);
  //  sk.addOffsets(20);
    skuska.cutToSize(40);
    skuska.cutBottom();

   // skuska.addOffsets(5);
   //  skuska.addPeak(0xFF,0x51,100,10,10000);
    skuska.saveToFile("D:\\before.txt");
    //skuska.addPeak(KEY,KEY^0xFF,25,10,50);
 //   skuska.moveLeft(10);

 //   skuska.saveToFile("D:\\left.txt");
    skuska.moveLeft(5,1);
    skuska.saveToFile("D:\\middle.txt");
    skuska.cutEnd(10);
    skuska.cutFront(10);
    cout<<skuska.getSize();

    //skuska.moveLeft(10,1);
    skuska.saveToFile("D:\\right.txt");
    //Trace temp(40,0);
    //temp.addOffsets(5);
    //skuska.applyOffsets(temp);
    //skuska.saveToFile("D:\\after.txt");

   // skuska.addRandomNoise(20,30,200);
   // skuska.saveToFile("D:\\after.txt");
 //   skuska.applyOffsets(sk);
  //  skuska.saveToFile("D:\\after.txt");
  //  sk.saveToFile("D:\\offsets.txt");

}

int main(){
    //test();
     testSec();
    return 0;
}
