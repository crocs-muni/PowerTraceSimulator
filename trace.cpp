#include "trace.h"


Trace::Trace(){
}

Trace::Trace(string path){
    if (readFile(path) == 1){
        cerr << "Problem during opening file" << endl;
    }
}

Trace::Trace(int size, int value){
    for(int i=0; i<size; i++)
        this->values.push_back(value);
}

int Trace::setValue(int position, int value){
    this->values.at(position) = value;
    return 0;
}

int Trace::getValue(int position){
    return this->values.at(position);
}

Trace Trace::operator+(Trace anotherOne){
    Trace temp;
    for(int i=0; i<this->getSize(); i++){
        temp.addValue(this->getValue(i)+anotherOne.getValue(i));
    }
    return temp;
}

Trace Trace::operator-(Trace anotherOne){
    Trace temp;
    for(int i=0; i<this->getSize(); i++){
        temp.addValue(this->getValue(i)-anotherOne.getValue(i));
    }
    return temp;
}

int Trace::addTo(Trace second){
    for(int i=0; i<this->getSize(); i++){
        this->setValue(i,this->getValue(i)+second.getValue(i));
    }
    return 0;
}

void Trace::writeValues(int start, int end){
    for(int i=start; i<=end; i++){
        cout << values.at(i) << endl;
    }
}


void Trace::addValue(int value){
    values.push_back(value);
}


int Trace::readFile(string path){
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


int Trace::saveToFile(string path){
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

int Trace::getSize(){
    return values.size();
}


int Trace::cutBottom(){
    int min=values.at(0);
    for(unsigned int i=1; i<values.size(); i++)
        if (values.at(i)<min)
            min=values.at(i);

    for(unsigned int i=0; i<values.size(); i++)
        values.at(i) -= min;

    return 0;
}

int Trace::cutEnd(int size){
    if (size>this->getSize())
        return 1;

    int temp = this->getSize();
    for(int i=0; i<temp-size; i++)
        this->values.pop_back();

    return 0;
}

int Trace::addRandomNoise(int start, int end, int noise){
    for(int i=start; i<=end; i++){
        values.at(i) += rand()%noise;
    }
    return 0;
}


int Trace::addPeak(unsigned char key, unsigned char data, int position, int width, int height){
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

int Trace::moveRight(int value){
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

int Trace::moveLeft(int value){
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

int Trace::moveUp(int value){
    for (int i=0; i<this->getSize(); i++){
        values.at(i) += value;
    }
    return 0;
}

int Trace::moveDown(int value){
    for (int i=0; i<this->getSize(); i++){
        values.at(i) -= value;
    }
    return 0;
}

unsigned char Trace::getData(){
    return this->data;
}

int Trace::setData(unsigned char data){
    this->data = data;
    return 0;
}



int Trace::addOffsets(const int max){
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


int Trace::applyOffsets(Trace offsets){
    Trace temp(this->getSize(),-99999999);

    for(int i=0; i<this->getSize(); i++)
        if(i+offsets.getValue(i) < this->getSize())
            temp.setValue(i+offsets.getValue(i),this->getValue(i));

    for(int i=0; i<this->getSize(); i++)
        this->values.at(i) = temp.getValue(i);

    return 0;
}

