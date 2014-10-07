#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <math.h>
#include <iomanip>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

const unsigned char KEY = 0x51;

using namespace std;

/**
 * Return sum of 1 in the char n
 * @brief bitCount
 * @param n
 */
static unsigned bitCount (unsigned char n){
    unsigned count = 0;
    while (n){
        ++count;
        n &= (n - 1);
    }
    return count;
}

/**
 * Represents curve which contains values and used data.
 * @brief The curve class
 */
class curve{
    vector<int> values;
    unsigned char data;

public:

    curve(){
    }

    curve(string path){
        if (readFile(path) == 1){
            cerr << "Problem during opening file" << endl;
        }
    }

    curve(int size, int value){
        for(int i=0; i<size; i++)
            this->values.push_back(value);
    }

    /**
     * Set value of specific point of curve
     * @brief setValue
     * @param position
     * @param value
     * @return
     */
    int setValue(int position, int value){
        this->values.at(position) = value;
        return 0;
    }

    int getValue(int position){
        return this->values.at(position);
    }

    curve operator+(curve anotherOne){
        curve temp;
        for(int i=0; i<this->getSize(); i++){
            temp.addValue(this->getValue(i)+anotherOne.getValue(i));
        }
        return temp;
    }

    curve operator-(curve anotherOne){
        curve temp;
        for(int i=0; i<this->getSize(); i++){
            temp.addValue(this->getValue(i)-anotherOne.getValue(i));
        }
        return temp;
    }

    int addTo(curve second){
        for(int i=0; i<this->getSize(); i++){
            this->setValue(i,this->getValue(i)+second.getValue(i));
        }
        return 0;
    }

    /**
     * Writes all values of curve to std. output
     * @brief writeValues
     * @param start
     * @param end
     */
    void writeValues(int start, int end){
        for(int i=start; i<=end; i++){
            cout << values.at(i) << endl;
        }
    }

    /**
     * Add value at the end of curve
     * @brief addValue
     * @param value
     */
    void addValue(int value){
        values.push_back(value);
    }

    /**
     * Load values of curve from specific file, EACH VALUE ON EACH LINE!
     * @brief readFile
     * @param path
     * @return
     */
    int readFile(string path){
        ifstream file(path);
        if (!file) {
            return 1;
        }
        string line;
        while(getline(file, line)){
            values.push_back(stoi(line));
        }
        file.close();
        return 0;

    }

    /**
     * Save values of curve to the file, 1 VALUE ON 1 LINE
     * @brief saveToFile
     * @param path
     * @return
     */
    int saveToFile(string path){
        ofstream file(path);
        if (!file){
            cerr << "Could not open file!" << endl;
            return 1;
        }

        for(unsigned int i=0; i<values.size(); i++){
            file << values.at(i) << endl;
        }
        file.close();
        return 0;
    }

    int getSize(){
        return values.size();
    }

    /**
     * Find the lowest value and subtract this value from all values. The lowes value is 0
     * @brief cutBottom
     * @return
     */
    int cutBottom(){
        int min=values.at(0);
        for(unsigned int i=1; i<values.size(); i++)
            if (values.at(i)<min)
                min=values.at(i);

        for(unsigned int i=0; i<values.size(); i++)
            values.at(i) -= min;

        return 0;
    }

    int cutEnd(int size){
        if (size>this->getSize())
            return 1;

        int temp = this->getSize();
        for(int i=0; i<temp-size; i++)
            this->values.pop_back();

        return 0;
    }

    /**
     * Add random value to each VALUE of the curve in specific range
     * @brief addRandomNoise
     * @param start
     * @param end
     * @param noise
     * @return
     */
    int addRandomNoise(int start, int end, int noise){
        for(int i=start; i<=end; i++){
            values.at(i) += rand()%noise;
        }
        return 0;
    }

    /**
     * Add peak to the curve, using Hamming weight
     * @brief addPeak
     * @param key
     * @param data
     * @param position
     * @param width
     * @param height
     * @param noise
     * @return
     */
    int addPeak(unsigned char key, unsigned char data, int position, int width, int height){
        double peak = (double)bitCount(data^key)/8;
        values.at(position) += height*peak;
        for(int i=1; i<=width/2; i++){
            double exp = (2.5/(width/2))*i;
            values.at(position+i) += peak*height*pow(2.7,-((exp*exp)/2));
            values.at(position-i) += peak*height*pow(2.7,-((exp*exp)/2));
        }
        // this->addRandomNoise(position-width/2,position+width/2,noise);
        return 0;
    }

    int moveRight(int value){
        int size = this->getSize();
        for(int i=0; i<value; i++){
            this->addValue(values.at(size-value+i));
        }
        for(int i=size-1; i>value; i--){
            values.at(i)=values.at(i-value);
        }
        for(int i=value; i>=0; i--){
            this->values.at(i)= 2*this->values.at(i+1)-this->values.at(i+2);
        }
        return 0;
    }

    int moveLeft(int value){
        for(int i=0; i<value; i++){
            this->values.emplace(this->values.begin(),0);
        }
        for(int i=value; i<this->getSize(); i++){
            values.at(i-value)=values.at(i);
        }

        for(int i=getSize()-value; i<getSize(); i++){
            this->values.at(i)= (2*this->values.at(i-1))-this->values.at(i-2);
        }
        return 0;
    }

    int moveUp(int value){
        for (int i=0; i<this->getSize(); i++){
            values.at(i) += value;
        }
        return 0;
    }

    int moveDown(int value){
        for (int i=0; i<this->getSize(); i++){
            values.at(i) -= value;
        }
        return 0;
    }

    unsigned char getData(){
        return this->data;
    }

    int setData(unsigned char data){
        this->data = data;
        return 0;
    }



    int addOffsets(const int max){
        vector<bool> control;
        for(unsigned int i=0; i<this->values.size(); i++){
            control.push_back(0);
        }

        int offset = 0;

        for(int i=0; i<max; i++){
            offset = rand()%max+1;
            if(control.at(i+offset) == 0){
                control.at(i+offset) = 1;
                this->setValue(i,offset);
            } else {
                offset = -i;
                while(control.at(i+offset) == 1){
                    offset++;
                }
                control.at(i+offset) = 1;
                this->setValue(i,offset);
            }
        }

        for(unsigned int i=max; i<this->values.size(); i++){
            if(control.at(i-max) == 0){
                control.at(i-max) = 1;
                this->setValue(i,-max);
            } else {
                offset = rand()%max+1;
                (rand()%2 == 0) ? offset=offset : offset=-offset;

                while((int)i+offset >= this->getSize())
                    offset--;

                if(control.at(i+offset) == 0){
                    control.at(i+offset) = 1;
                    this->setValue(i,offset);
                } else {
                    offset=-max;
                    while(control.at(i+offset) == 1)
                        offset++;
                    control.at(i+offset) = 1;
                    this->setValue(i,offset);
                }
            }
        }

        return 0;
    }


    int applyOffsets(curve offsets){
        curve temp(this->getSize(),-99999999);

        for(int i=0; i<this->getSize(); i++)
            if(i+offsets.getValue(i) < this->getSize())
                temp.setValue(i+offsets.getValue(i),this->getValue(i));

        for(int i=0; i<this->getSize(); i++)
            this->values.at(i) = temp.getValue(i);

        return 0;
    }

};

/**
 * From specific curve creates number of files, each curve contains peak in specific place and range, key is set, data are random generated
 * @brief createCurvesFile
 * @param myCurve
 * @param number
 * @param path
 * @param key
 * @param position
 * @param width
 * @param height
 * @param noise
 * @return
 */
int createCurvesFile(curve myCurve, int number, string path, unsigned char key, int position, int width, int height, int noise){
    if ((position+width/2) > myCurve.getSize()) {return 1;}
    ofstream file(path+"_record.dat");
    for(int i=0; i<number; i++){
        curve temp = myCurve;
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
 * From specific curve creates Vector of curves, each curve contains peak in specific place and range, key is set, data are random generated
 * @brief createCurvesVector
 * @param myCurve
 * @param number
 * @param curves
 * @param key
 * @param position
 * @param width
 * @param height
 * @param noise
 * @return
 */
int createCurvesVector(curve myCurve, int number, vector <curve> * curves, unsigned char key, int position, int width, int height, int noise, int offs){
    if ((position+width/2) > myCurve.getSize()) {return 1;}
    curve offsets(myCurve.getSize(),0);
    offsets.addOffsets(offs);
    //offsets.saveToFile("D:\\save.txt");

    for(int i=0; i<number; i++){
        curve temp = myCurve;
        unsigned char data = rand()%256;
        temp.addPeak(key, data, position, width, height);
        temp.addRandomNoise(0,temp.getSize()-1,noise);
        temp.setData(data);
        temp.applyOffsets(offsets);
        curves->push_back(temp);
    }
    return 0;
}


/**
 * Export Vector of curves to the Files
 * @brief saveCurvesFile
 * @param curves
 * @param path
 * @return
 */
int saveCurvesFile(vector <curve> * curves, string path){
    for(unsigned int i=0; i<curves->size(); i++){
        curves->at(i).saveToFile(path+to_string(i)+".dat");
    }
    return 0;
}


int dpaKey(vector <curve> * curves, unsigned int key, curve *diff){

    int sumLow = 0;
    int sumHigh = 0;
    curve low(curves->at(0).getSize(),0);
    curve high(curves->at(0).getSize(),0);

    for(unsigned int i=0; i<curves->size(); i++){
        if(bitCount(curves->at(i).getData()^key) < 4){
            low.addTo(curves->at(i));
            sumLow++;
        } else {
            high.addTo(curves->at(i));
            sumHigh++;
        }
    }

    for(int i=0; i<low.getSize(); i++)
        low.setValue(i,low.getValue(i)/sumLow);

    for(int i=0; i<high.getSize(); i++)
        high.setValue(i,high.getValue(i)/sumHigh);

    diff->setData(key);
    *diff = high-low;
    return 0;
}

int dpa(vector <curve> * curves, vector <curve> * diffCurves){
    for(unsigned int i=0; i<256; i++){
        curve temp;
        dpaKey(curves, i, &temp);
        diffCurves->push_back(temp);
    }
    return 0;
}

/**
 * Find maximum value from Vector of curves, return number of curve which contains highes value
 * @brief findMax
 * @param curves
 * @param position
 * @return
 */
int fitnessBasic(vector <curve> * curves, int *position){
    int temp= -99999; //nahrad int min value
    for(unsigned int i=0; i<curves->size(); i++){
        for(int j=0; j<curves->at(i).getSize(); j++){
            if(curves->at(i).getValue(j) > temp){
                temp=curves->at(i).getValue(j);
                *position = i;
            }
        }
    }
    return 0;
}


int fitnessBetter(vector <curve> * curves, int *positionV, int *positionW){
    int radius = 2;
    int maxValue = -9999999;
    int maxWide = -999999;
    for(unsigned int i=0; i<curves->size(); i++){
        for(int j=radius; j<curves->at(i).getSize()-radius; j++){
            int temp = 0;
            for(int k=j-radius; k<=j+radius; k++)
                temp += curves->at(i).getValue(k);
            temp = temp/(2*radius+1);
            if(temp>maxValue){
                maxValue=temp;
                *positionV=i;
            }
            //curves->at(i).setValue(j,temp);
        }
    }

    radius = 5;
    for(unsigned int i=0; i<curves->size(); i++){
        for(int j=radius; j<curves->at(i).getSize()-radius; j++){
            int temp = 0;
            for(int k=j-radius; k<=j+radius; k++)
                temp += curves->at(i).getValue(k);
            if(temp>maxWide){
                maxWide=temp;
                *positionW=i;
            }
            // curves->at(i).setValue(j,temp);
        }
    }

    //fitnessBasic(curves,position);
    return maxWide;
}

int shake(vector <curve> * curves, const int noise){
    if ((noise == 0)||(noise== 1)) return 0;

    int temp = curves->at(0).getSize();
    for(unsigned int i=0; i<curves->size(); i++){
        curves->at(i).moveRight(rand()%noise);
        curves->at(i).cutEnd(temp);
    }
    return 0;
}


void test(){
    srand (time(NULL));
    curve skuska("D:\\dataout4.dat");
    skuska.cutBottom();

    int noise = 10;
    int sum = 100;
    int shakeValue = 0; //to effect must be higher than 1
    int offsets = 5;

    ofstream file("D:\\offsets\\shake0_off5.txt");

    file << "Used base curve: " << "D:\\dataout4.dat" << endl;
    file << "Number of generated curves with peak: 500" << endl;
    file << "Used key: 0x" << hex << uppercase << (int)KEY << endl;
    file << "Position where peak is added: 100" << endl;
    file << "Max height of peak 100" << endl;
    file << "Width of peak: 10" << endl;
    file << "Used max random offsets value (random move for each value): " << offsets << endl;
    file << "Used shake value(random move to right): " << shakeValue << endl << endl;
    file << "BEGIN TEST" << endl;
    file << "==========" << endl << endl << endl;

    for (int j=0; j<7; j++){
        int okv = 0;
        int okw = 0;
        int together = 0;


        for (int i=0; i<sum; i++){
            vector <curve> array;
            createCurvesVector(skuska,500,&array,KEY,100,10,100,noise,offsets);

            vector <curve> result;
            shake(&array, shakeValue);
            dpa(&array,&result);

            int positionV;
            int positionW;

            fitnessBetter(&result,&positionV, &positionW);
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
    curve skuska("D:\\dataout4.dat");

   // curve sk(skuska.getSize(),0);
  //  sk.addOffsets(20);
    skuska.cutEnd(40);
   skuska.cutBottom();

   // skuska.addOffsets(5);

     //  skuska.addPeak(0xFF,0x51,100,10,10000);
    skuska.saveToFile("D:\\before.txt");
    skuska.addPeak(KEY,KEY^0xFF,25,10,50);
    skuska.saveToFile("D:\\peak.txt");
    curve temp(40,0);
    temp.addOffsets(5);
    skuska.applyOffsets(temp);
    skuska.saveToFile("D:\\after.txt");

   // skuska.addRandomNoise(20,30,200);
   // skuska.saveToFile("D:\\after.txt");
 //   skuska.applyOffsets(sk);
  //  skuska.saveToFile("D:\\after.txt");
  //  sk.saveToFile("D:\\offsets.txt");

}

int main()
{
    //test();
     testSec();
    return 0;
}

