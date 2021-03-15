#include <stdlib.h>
#include <cassert>
#include "Utility/timer.cpp"
#include "DivideAndConquer.cpp"

//------------------------Utility Function--------------------------------------
template < typename T>
std::ostream& operator << (std::ostream& os, const std::vector<T>& v) {
    os << "[";
    for (T it: v)
        os << it << ", ";
    os << "]";
}
#include <algorithm>
bool isSorted(std::vector<int> v) {
    std::vector<int> sv(v);
    std::sort(sv.begin(), sv.end());

    std::vector<int>::iterator i = sv.begin();
    std::vector<int>::iterator j = v.begin();

    do {
        if(*i != *j)
            return false;
        i++;j++;
    } while(i == sv.end());
    return true;
}

//------------------------Function Test_ 1 -> Fibonacci Number -----------------
std::vector<int> f_divide_T1(int n) {return std::vector<int>{n-1, n-2};}
bool f_baseCaseTest_T1(int n) {return n<=2;}
int f_baseCaseSolution_T1(int n) {return 1;}
int f_conquer_T1(std::vector<int> &vect) {
    int n1 = vect.back(); vect.pop_back();
    int n2 = vect.back(); vect.pop_back();
    return n1+n2;}

//------------------------Function Test_ 2 -> Merge Sort -----------------------
std::vector<std::vector<int>*> f_divide_T2(std::vector<int>* vect) {
    int offset = vect->size()/2;
    return std::vector<std::vector<int>*>{new std::vector<int>(vect->begin(), vect->begin()+offset), new std::vector<int>(vect->begin()+offset, vect->end())};
}
bool f_baseCaseTest_T2(std::vector<int>* vect) {
    return vect->size() <= 2;
}
std::vector<int>* f_baseCaseSolution_T2(std::vector<int>* vect){
    #define valueIn (*vect)
    if(vect->size() == 1)
        return vect;
    if(valueIn[0] > valueIn[1]) {
        int temp = valueIn[0];
        valueIn[0] = valueIn[1];
        valueIn[1] = temp;
    }
    return vect;
}
std::vector<int>* f_conquer_T2(std::vector<std::vector<int>*> &vect){
    std::vector<int>* result = new std::vector<int>();

    std::vector<int>* vect1 = vect.back(); vect.pop_back();
    std::vector<int>* vect2 = vect.back(); vect.pop_back();

    int len = vect1->size() + vect2->size();
    std::vector<int>::iterator i = vect1->begin();
    std::vector<int>::iterator j = vect2->begin();

    for(int k = len; k > 0 ; k--)
        if(*i < *j) {
            result->push_back(*i);
            if(++i == vect1->end()) {//accoda il secondo vettore ha finito il primo
                result->insert(result->end(), j, vect2->end());
                break;
            }
        } else {
            result->push_back(*j);
            if(++j == vect2->end()) {//accoda il primo vettore ha finito il secondo
                result->insert(result->end(), i, vect1->end());
                break;
            }
        }

    return result;
}

//------------------------Function Test_ 3 -> Quick Sort -----------------------
// std::vector<std::vector<int>*> f_divide_T2(std::vector<int>* vect) {}
// bool f_baseCaseTest_T2(std::vector<int>* vect) {}
// std::vector<int>* f_baseCaseSolution_T2(std::vector<int>* vect){}
// std::vector<int>* f_conquer_T2(std::vector<std::vector<int>*> &vect){}

template<typename inpT, typename outT>
void take_mesurement(DivideAndConquer<inpT, outT> test, inpT toCompute, int numOfThread, outT expectedResult, bool verbose = true) {
    outT result = take_mesurement(test, toCompute, numOfThread, verbose);
    assert(result == expectedResult);
}
template<typename inpT, typename outT>
outT take_mesurement(DivideAndConquer<inpT, outT> test, inpT toCompute, int numOfThread, bool verbose = true) {
    test.setnumOfThread(numOfThread);
    tic();
    outT result = test.compute(toCompute);
    std::string time_ = toc(false);
    if(verbose)
        std::cout << "Divide&Conquer has been run with the value " << toCompute << " end the result is: " << result << ".\n\t";
    std::cout << time_ << " with " << numOfThread << " thread." << std::endl;
    return result;
}

int main(int argc, char * argv[]) {
    srand (time(NULL));
    int type, nThread, inputOfTest;
    if(argc == 2) {//controllo args
        type = std::stoi(argv[1]);
        nThread = 1;
        inputOfTest = 0;
    } else {
        if(argc == 3) {
            type = std::stoi(argv[1]);
            nThread = std::stoi(argv[2]);
            inputOfTest = 0;
        } else {
            if(argc == 4) {
                type = std::stoi(argv[1]);
                nThread = std::stoi(argv[2]);
                inputOfTest = std::stoi(argv[3]);
            } else {
                std::cout << "Parameters: TypeTest, [numOfThread], [inputOfTest]" << std::endl;
                std::cout << "\tTypeTest: 1 = Fibonacci o 2 = Merge o 3 = Quick," << std::endl;
                std::cout << "\tnumOfThread: numero di thread da utilizzare durante l'esecuzione," << std::endl;
                std::cout << "\tinputOfTest: Dato in input per il relativo test (n° Fibonacci, N di Elementi del vettore)" << std::endl;
                return 1;
            }
        }
    }

    // if(type == 1) {// Fibonacci number
    //     DivideAndConquer<int, int> test1(f_divide_T1, f_baseCaseTest_T1, f_baseCaseSolution_T1, f_conquer_T1);
    //     if(inputOfTest == 0)
    //         take_mesurement(test1, rand() % 20, nThread);
    //     else
    //         take_mesurement(test1, inputOfTest, nThread);
    // }

    if(type == 2) {//Merge Sort
        DivideAndConquer<std::vector<int>*, std::vector<int>*> test2(f_divide_T2, f_baseCaseTest_T2, f_baseCaseSolution_T2, f_conquer_T2);
        std::vector<int>* vectTest = new std::vector<int>(inputOfTest == 0?1000000:inputOfTest);
        for(int i = vectTest->size(); i!=0; i--)
            (*vectTest)[i] = rand() % 1000000;
        std::vector<int>* result = take_mesurement(test2, vectTest, nThread, false);
        std::cout << isSorted(*result) << (vectTest->size()==result->size()) << true << std::endl;
    }

    // Take Mesurement
    // take_mesurement(test1, 4, 1, 3);
    // take_mesurement(test1, 6, 2, 8);
    // take_mesurement(test1, 8, 1, 21);
    // take_mesurement(test1, 10, 2, 55);
    // take_mesurement(test1, 12, 6, 144);
    //
    // take_mesurement(test2, vectTest, 1);
    // take_mesurement(test2, vectTest, 2);
    // take_mesurement(test2, vectTest, 4);
    // take_mesurement(test2, vectTest, 8);
    // take_mesurement(test2, vectTest, 16);
    // take_mesurement(test2, vectTest, 32);
    // take_mesurement(test2, vectTest, 64);
    // take_mesurement(test2, vectTest, 128);
}
