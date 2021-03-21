#include <stdlib.h>
#include <cassert>
#include <string>
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
template < typename T>
std::ostream& operator << (std::ostream& os, const std::vector<T>* v) {
    os << "[";
    for (T it: *v)
        os << it << ", ";
    os << "]";
}

template<typename inpT, typename outT>
void take_mesurement(DivideAndConquer<inpT, outT> test, inpT toCompute, int designType, std::vector<int> numOfThread, outT expectedResult, bool verbose = true) {
    outT result = take_mesurement(test, toCompute, designType, numOfThread, verbose);
    assert(result == expectedResult);
}
template<typename inpT, typename outT>
void take_mesurement(DivideAndConquer<inpT, outT> test, inpT toCompute, int designType, int numOfThread, outT expectedResult, bool verbose = true) {
    test.setnumOfThread(numOfThread);
    take_mesurement(test, toCompute, designType, test.getnumOfThread(), expectedResult, verbose);
}
template<typename inpT, typename outT>
outT take_mesurement(DivideAndConquer<inpT, outT> test, inpT toCompute, int designType, int numOfThread, bool verbose = true) {
    test.setnumOfThread(numOfThread);
    return take_mesurement(test, toCompute, designType, test.getnumOfThread(), verbose);
}
template<typename inpT, typename outT>
outT take_mesurement(DivideAndConquer<inpT, outT> test, inpT toCompute, int designType, std::vector<int> numOfThread, bool verbose = true) {
    test.setnumOfThread(numOfThread);
    tic();
    outT result = test.compute(toCompute, designType);
    std::string time_ = toc(false);
    if(verbose)
        std::cout << "Divide&Conquer has been run with the value " << toCompute << " end the result is: " << result << ".\n\t";
    std::cout << time_ << " with " << test.getnumOfThread() << " thread." << std::endl;
    return result;
}

//------------------------Function Test Fibonacci Number -----------------------
std::vector<int> f_divide_F(int n) {return std::vector<int>{n-1, n-2};}
bool f_baseCaseTest_F(int n) {return n<=2;}
int f_baseCaseSolution_F(int n) {return 1;}
int f_conquer_F(std::vector<int> &vect) {
    int n1 = vect.back(); vect.pop_back();
    int n2 = vect.back(); vect.pop_back();
    return n1+n2;}

//------------------------Function Test Merge Sort -----------------------------
std::vector<std::vector<int>*> f_divide_M(std::vector<int>* vect) {
    int offset = vect->size()/2;
    return std::vector<std::vector<int>*>{new std::vector<int>(vect->begin(), vect->begin()+offset), new std::vector<int>(vect->begin()+offset, vect->end())};
}
bool f_baseCaseTest_M(std::vector<int>* vect) {
    return vect->size() <= 2;
}
std::vector<int>* f_baseCaseSolution_M(std::vector<int>* vect){
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
std::vector<int>* f_conquer_M(std::vector<std::vector<int>*> &vect){
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

//------------------------Function Test Quick Sort -----------------------------
// std::vector<std::vector<int>*> f_divide_Q(std::vector<int>* vect) {}
// bool f_baseCaseTest_Q(std::vector<int>* vect) {}
// std::vector<int>* f_baseCaseSolution_Q(std::vector<int>* vect){}
// std::vector<int>* f_conquer_Q(std::vector<std::vector<int>*> &vect){}

int main(int argc, char * argv[]) {
    srand (time(NULL));
    std::string type;
    int designType, inputOfTest;
    std::vector<int> nThread(3, 0);
    if(argc < 4 || argc > 9) {
        std::cout << "Parameters: TypeTest, designType, inputOfTest, [numOfThread]" << std::endl;
        std::cout << "\tTypeTest: F = Fibonacci o M = Merge con la possibilita di aggiungere T = Test(es. MT)" << std::endl;
        std::cout << "\tDesignType:\n\t\t 0 = Sequenziale \n\t\t 1 = DivideSeq->FarmSolve->CombineSeq\n\t\t 2 = Pipeline(FarmDivide,FarmBaseSolution,FarmCombine)," << std::endl;
        std::cout << "\tinputOfTest: Dato in input per il relativo test (n° Fibonacci, N di Elementi del vettore)" << std::endl;
        std::cout << "\tnumOfThread: numero di thread da utilizzare durante l'esecuzione o array contenente i thread per ogni stato della pipeline esattamente uguale a questo [ n1 n2 n3 ]," << std::endl;
        return 1;
    } else {
        type = argv[1];
        designType = std::stoi(argv[2]);
        inputOfTest = std::stoi(argv[3]);
        std::string check;
        if(argc == 4)
            check = "1";
        else
            check = argv[4];
        int i = 0;

        if(check.compare("[") == 0) {
            do {
                check = argv[5+i];
                if(check.compare("]") != 0)
                    nThread[i] = std::stoi(check);
                else break;
                if(++i > 3)
                    break;
            } while(true);
        } else
            nThread[0] = std::stoi(check);
    }

    if(type.compare("F") == 0 || type.compare("FT") == 0) {// Fibonacci number
        DivideAndConquer<int, int> test1(f_divide_F, f_baseCaseTest_F, f_baseCaseSolution_F, f_conquer_F);
        inputOfTest = inputOfTest == 0?rand() % 20:inputOfTest;
        if(type.compare("F") == 0) {
            if(designType != 0)
                take_mesurement(test1, inputOfTest, designType, nThread);
            else {
                tic();
                int result = DivideAndConquer<int, int>::solve(inputOfTest, f_divide_F, f_baseCaseTest_F, f_baseCaseSolution_F, f_conquer_F);
                std::string time_ = toc(false);
                std::cout << time_ << " with " << nThread << " thread." << std::endl;
            }
            return 0;
        }
        //tutti i casi di test
        if(designType == 1) {
            take_mesurement(test1, inputOfTest, designType, 1, false);
            take_mesurement(test1, inputOfTest, designType, 2, false);
        }
        take_mesurement(test1, inputOfTest, designType, 3, false);
        take_mesurement(test1, inputOfTest, designType, 4, false);
        take_mesurement(test1, inputOfTest, designType, 5, false);
        take_mesurement(test1, inputOfTest, designType, 6, false);
        take_mesurement(test1, inputOfTest, designType, 7, false);
        take_mesurement(test1, inputOfTest, designType, 8, false);
        take_mesurement(test1, inputOfTest, designType, 9, false);
        take_mesurement(test1, inputOfTest, designType, 10, false);
        take_mesurement(test1, inputOfTest, designType, 20, false);
        take_mesurement(test1, inputOfTest, designType, 30, false);
        take_mesurement(test1, inputOfTest, designType, 40, false);
        take_mesurement(test1, inputOfTest, designType, 50, false);
        take_mesurement(test1, inputOfTest, designType, 100, false);
        take_mesurement(test1, inputOfTest, designType, 150, false);
        take_mesurement(test1, inputOfTest, designType, 200, false);
        take_mesurement(test1, inputOfTest, designType, 250, false);

        // take_mesurement(test1, inputOfTest, designType, 2, false);
        // take_mesurement(test1, inputOfTest, designType, 4, false);
        // take_mesurement(test1, inputOfTest, designType, 8, false);
        // take_mesurement(test1, inputOfTest, designType, 16, false);
        // take_mesurement(test1, inputOfTest, designType, 32, false);
        // take_mesurement(test1, inputOfTest, designType, 64, false);
        // take_mesurement(test1, inputOfTest, designType, 128, false);
        // take_mesurement(test1, inputOfTest, designType, 256, false);
    }

    if(type.compare("M") == 0 || type.compare("MT") == 0) {//Merge Sort
        DivideAndConquer<std::vector<int>*, std::vector<int>*> test2(f_divide_M, f_baseCaseTest_M, f_baseCaseSolution_M, f_conquer_M);

        std::vector<int>* vectTest = new std::vector<int>(inputOfTest == 0?1000000:inputOfTest);
        for(int i = vectTest->size(); i!=0; i--) (*vectTest)[i] = rand() % 100000;

        if(type.compare("M") == 0) {
            if(designType != 0)
                std::vector<int>* result = take_mesurement(test2, vectTest, designType, nThread, false);
            else {
                tic();
                std::vector<int>* result = DivideAndConquer<std::vector<int>*, std::vector<int>*>::solve(vectTest, f_divide_M, f_baseCaseTest_M, f_baseCaseSolution_M, f_conquer_M);
                std::string time_ = toc(false);
                std::cout << time_ << " with " << nThread << " thread." << std::endl;
            }
            return 0;
        }
        //tutti i casi di test
        if(designType == 1) {
            take_mesurement(test2, vectTest, designType, 1, false);
            take_mesurement(test2, vectTest, designType, 2, false);
        }
        take_mesurement(test2, vectTest, designType, 3, false);
        take_mesurement(test2, vectTest, designType, 4, false);
        take_mesurement(test2, vectTest, designType, 5, false);
        take_mesurement(test2, vectTest, designType, 6, false);
        take_mesurement(test2, vectTest, designType, 7, false);
        take_mesurement(test2, vectTest, designType, 8, false);
        take_mesurement(test2, vectTest, designType, 9, false);
        take_mesurement(test2, vectTest, designType, 10, false);
        take_mesurement(test2, vectTest, designType, 20, false);
        take_mesurement(test2, vectTest, designType, 30, false);
        take_mesurement(test2, vectTest, designType, 40, false);
        take_mesurement(test2, vectTest, designType, 50, false);
        take_mesurement(test2, vectTest, designType, 100, false);
        take_mesurement(test2, vectTest, designType, 150, false);
        take_mesurement(test2, vectTest, designType, 200, false);
        take_mesurement(test2, vectTest, designType, 250, false);

        // take_mesurement(test2, vectTest, designType, 2, false);
        // take_mesurement(test2, vectTest, designType, 4, false);
        // take_mesurement(test2, vectTest, designType, 8, false);
        // take_mesurement(test2, vectTest, designType, 16, false);
        // take_mesurement(test2, vectTest, designType, 32, false);
        // take_mesurement(test2, vectTest, designType, 64, false);
        // take_mesurement(test2, vectTest, designType, 128, false);
        // take_mesurement(test2, vectTest, designType, 256, false);

    }
}
