#include "trace.h"

Trace::Trace(){
}

Trace::Trace(const Trace& second):data(second.data),values(second.values){
}

Trace::Trace(const string &path){
    if (readFile(path) == E){
        cerr << "Problem during opening file" << endl;
    }
}

Trace::Trace(const int &size, const int &value){
    for(int i=0; i<size; i++)
        this->values.push_back(value);
}

Trace Trace::operator+(const Trace& second){
    Trace temp(*this);

    for(int i=0; (i<second.getSize()) && (i<this->getSize()); i++)
        temp.values.at(i) += second.getValue(i);
    return temp;
}

Trace Trace::operator-(const Trace& second){
    Trace temp(*this);

    for(int i=0; (i<second.getSize()) && (i<this->getSize()); i++)
        temp.values.at(i) -= second.getValue(i);
    return temp;
}

vector<int> Trace::getValues() const{
    return values;
}

int Trace::getValue(const int &position) const{
    return this->values.at(position);
}

int Trace::setValue(const unsigned int &position, const int &value){
    if (position >= values.size())
        return RE;
    this->values.at(position) = value;
    return OK;
}

int Trace::addition(const Trace &second){
    for(int i=0; (i<second.getSize()) && (i<this->getSize()); i++)
        this->values.at(i) += second.values.at(i);
    return OK;
}

int Trace::subtraction(const Trace &second){
    for(int i=0; (i<second.getSize()) && (i<this->getSize()); i++)
        this->values.at(i) -= second.values.at(i);
    return OK;
}

void Trace::writeValues(const int &start, const int &end) const{
    for(int i=start; i<=end; i++)
        cout << values.at(i) << endl;
}

void Trace::writeValues() const{
    for (auto &element : values)
        cout << element << endl;
}

void Trace::addValue(const int &value){
    values.push_back(value);
}

int Trace::readFile(const string &path){
    ifstream file(path);
    if (!file) {
        return E;
    }
    string line;
    while(getline(file, line)){
        values.push_back(stoi(line));
    }
    file.close();
    return OK;
}

int Trace::saveToFile(const string& path) const{
    ofstream file(path);
    if (!file){
        cerr << "Could not open file!" << endl;
        return E;
    }

    for (auto &element : values)
        file << element << endl;

    file.close();
    return OK;
}

int Trace::getSize() const{
    return values.size();
}

int Trace::cutBottom(){
    int min=values.at(0);
    for(unsigned int i=1; i<values.size(); i++)
        if (values.at(i)<min)
            min=values.at(i);

    for (auto &element : values)
        element -= min;

    return OK;
}

int Trace::cutToSize(const int &size){
    if (size>=this->getSize())
        return RE;

    int temp = this->getSize();
    for(int i=0; i<temp-size; i++)
        this->values.pop_back();
    return OK;
}

int Trace::cutEnd(const int &number){
    values.erase(values.end()-number,values.end());
    return OK;
}

int Trace::cutFront(const int &number){
    values.erase(values.begin(),values.begin()+number);
    return OK;
}

int Trace::addRandomNoise(const int &start, const int &end, const int &noise){
    if ((start>end) || (start<0) || (start>=this->getSize()) || (end>=this->getSize())) return RE;
    if (noise==0) return OK;

    for(int i=start; i<=end; i++)
        (rand()%2 == 0) ? values.at(i)+=rand()%(noise+1) : values.at(i)-=rand()%(noise+1);
    return OK;
}

int Trace::inverseValues(){
    for (auto &element : values)
        element = -element;
    return OK;
}

int Trace::addPeak(const unsigned char key, const unsigned char data, const int position, const int width, const int height){
    if ((position+width/2 >= this->getSize()) || (position-width/2 < 0)) return RE;

    double peak = (double)bitCount(data^key)/8;
    values.at(position) += height*peak;
    for(int i=1; i<=width/2; i++){
        double exp = (2.5/(width/2))*i;
        values.at(position+i) += peak*height*pow(2.7,-((exp*exp)/2));
        values.at(position-i) += peak*height*pow(2.7,-((exp*exp)/2));
    }
    return OK;
}

int Trace::moveRight(const int value, const int param){
    if ((param != 0) || (param != 1)) return WP;
    if ((value < 0) || (value>=this->getSize())) return RE;

    if(param == 0){
        int size = this->getSize();
        for(int i=0; i<value; i++)
            this->addValue(values.at(size-value+i));

        for(int i=size-1; i>value; i--)
            values.at(i)=values.at(i-value);

        for(int i=value; i>=0; i--)
            this->values.at(i)= 2*this->values.at(i+1)-this->values.at(i+2);
    }

    if(param == 1){
        for(int i=this->values.size()-value-1; i>=0; i--)
            values.at(i+value)=values.at(i);
    }
    return OK;
}

int Trace::moveLeft(const int value, const int param){
    if ((param != 0) || (param != 1)) return WP;
    if ((value < 0) || (value>=this->getSize())) return RE;

    if (param==0){
        for(int i=0; i<value; i++)
            this->values.emplace(this->values.begin(),0);

        for(int i=value; i<this->getSize(); i++)
            values.at(i-value)=values.at(i);


        for(int i=getSize()-value; i<getSize(); i++)
            this->values.at(i)= (2*this->values.at(i-1))-this->values.at(i-2);

    }

    if (param==1){
        for(unsigned int i=0; i<this->values.size()-value; i++)
            values.at(i)=values.at(i+value);
    }
    return OK;
}

int Trace::moveUp(const int value){
    for (auto &element : values)
        element += value;
    return OK;
}

int Trace::moveDown(const int value){
    for (auto &element : values)
        element -= value;

    return OK;
}

unsigned char Trace::getData() const{
    return this->data;
}

int Trace::setData(const unsigned char data){
    this->data = data;
    return OK;
}

int Trace::addOffsets(const int max){
    vector<bool> control(this->getSize(),0);
    int offset = 0;

    for(int i=0; i<max; i++){
        offset = rand()%max+1;
        if(control.at(i+offset) == 0){
            control.at(i+offset) = 1;
            this->setValue(i,offset);
        } else {
            offset = -i;
            while(control.at(i+offset) == 1)
                offset++;
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
    return OK;
}

int Trace::applyOffsets(const Trace &offsets){
    if (this->getSize() < offsets.getSize()) return RE;

    Trace temp(this->getSize(),INT_MIN);

    for(int i=0; i<this->getSize(); i++)
        if(i+offsets.getValue(i) < this->getSize())
            temp.setValue(i+offsets.getValue(i),this->getValue(i));

    for(int i=0; i<this->getSize(); i++)
        this->values.at(i) = temp.getValue(i);
    return OK;
}
