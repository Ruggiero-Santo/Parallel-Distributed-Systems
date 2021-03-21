#ifndef sanDivideAndConquer
    #include "Utility/Farm.cpp"
    #include "Utility/Queue.cpp"
    #include <functional>
    #include <deque>
    #include <tuple>
    #define sanDivideAndConquer

template< typename inType, typename outType> class DivideAndConquer {
private:

    Queue<inType>* inpQueue = new Queue<inType>();
    Queue<inType>* midIQueue = new Queue<inType>();
    Queue<outType>* midOQueue = new Queue<outType>();
    Queue<outType>* outQueue = new Queue<outType>();

    const std::function<std::vector<inType>(inType)> f_divide;
    const std::function<bool(inType)> f_baseCaseTest;
    const std::function<outType(inType)> f_baseCaseSolution;
    const std::function<outType(std::vector<outType>&)> f_conquer;

    std::vector<int> numOfThread;

    /**
     * Funzione di preparazione per la Farm. Non fa altro che dividere il
     *  problema fino ad avere tanti subProblems quanti i thread dichiarati.
     *
     * @param  problem Problema principale da suddividere.
     * @return         Queue contenente i vari subProblems da sottoporre alla Farm.
     */
    std::deque<inType> divideForFarm(inType problem) {
        std::deque<inType> subProblems;
        subProblems.push_back(problem);
        while (subProblems.size() < numOfThread[0]) {//eseguo il divide sul problema per creare subProblems per la Farm fino ad averne almeno uno per nodo della Farm
            inType temp = subProblems.front(); subProblems.pop_front();
            if (f_baseCaseTest(temp)) {//verifico se sto cercado di dividere una Problema Base
                int i;
                for(i = subProblems.size(); i != 0; i--) {// verifico se i subProblems avuti precendentemente non sono base
                    inType it = subProblems.front();
                    if(f_baseCaseTest(it)) {
                        subProblems.pop_front();
                        subProblems.push_back(it);
                    } else break;//se ho trovato un Problema non Base esco dalla verifica e continuo a dividere in subProblems
                }
                subProblems.push_front(temp);// se sono tutti Base esco ed invio i subProblems alla Farm
                if(i == 0) break;
            } else
                for (inType it: f_divide(temp))
                    subProblems.push_back(it);
        }
        return subProblems;
    }
    /**
     * Funzione di acquisizione delle soluzioni prodotte dalla Farm. Non fa altro
     *  che applicare il conquer sulle subSolution per ottenere la soluzione finale.
     * @param  subSolutions Queue contenente i risultati provenenti dai subProblems
     * @return              Soluzione del problema originale.
     */
    outType conquerFromFarm(Queue<outType> subSolutions) {
        std::deque<outType> temp = subSolutions.getData();
        std::vector<outType> _subSolutions(temp.begin(), temp.end());
        while(_subSolutions.size() != 1) //eseguo il conquer su le varie soluzioni che mi sono arrivate dalla Farm fino ad avere una soluzione
            _subSolutions.push_back(f_conquer(_subSolutions));
        return _subSolutions.front();
    }

    static inType f_FarmDivide_E(inType inp, const std::function<std::vector<inType>(inType)>& f_divide, const std::function<bool(inType)>& f_test, Queue<inType>* Iqueue, Queue<inType>* Oqueue) {
        auto in = f_divide(inp);
        auto i = in.begin()++;
        while(++i != in.end()) {
            if(f_test(*i))
            Oqueue->push(*i);
            else
            Iqueue->push(*i);
        }
        return in[0];
    }
    static bool f_FarmDivide_C(inType inp, const std::function<bool(inType)>& f_test) {
        return !f_test(inp);
    }

    static std::optional<outType> f_FarmConquer_E(Queue<outType>* subSolutions, const std::function<outType(std::vector<outType>&)>& f_conquer) {
        auto a = subSolutions->pop(2);
        if(a.has_value()) {
            std::vector<outType> temp  = a.value();
            if(temp.size() == 1) {
                if(!subSolutions->waitInput())
                    return temp[0];
                else {
                    subSolutions->push(temp[0]);
                    subSolutions->tellpush();
                    return {};
                }
            }
            return f_conquer(temp);
        } else
            return {};
    }
    static bool f_FarmConquer_C(Queue<outType>* queue) {
        return (queue->getCheck() == 0 && queue->waitInput() == false)? false: true;
    }

public:
    DivideAndConquer(
        const std::function<std::vector<inType>(inType)>& f_divide = nullptr,
        const std::function<bool(inType)>& f_baseCaseTest = nullptr,
        const std::function<outType(inType)>& f_baseCaseSolution = nullptr,
        const std::function<outType(std::vector<outType>&)>& f_conquer = nullptr,
        std::vector<int> numOfThread = {1, 0, 0}):
            f_divide(f_divide),
            f_baseCaseTest(f_baseCaseTest),
            f_baseCaseSolution(f_baseCaseSolution),
            f_conquer(f_conquer) {setnumOfThread(numOfThread);}

    void setDivedeFunc(const std::function<std::vector<inType>(inType)> f) {f_divide = f;}
    void setBaseCaseTestFunc(const std::function<bool(inType)>& f) {f_baseCaseTest = f;}
    void setBaseCaseSolutionFunc(const std::function<outType(inType)>& f) {f_baseCaseSolution = f;}
    void setConquerFunc(const std::function<outType(std::vector<outType>&)>& f) {f_conquer = f;}
    void setnumOfThread(std::vector<int> numThread) {numThread.resize(3, 0);numOfThread.swap(numThread);}
    void setnumOfThread(int numThread) {numOfThread.resize(3, 0); numOfThread[0] = numThread;}

    std::vector<int> getnumOfThread()const { return numOfThread;}

    outType compute(inType problem, int structureType = 0) {
        if (f_divide == nullptr) throw std::invalid_argument( "You must set Divide function." );
        if (f_baseCaseTest == nullptr) throw std::invalid_argument( "You must set Test for base Problem function." );
        if (f_baseCaseSolution == nullptr) throw std::invalid_argument( "You must set Solution for base Problem function." );
        if (f_conquer == nullptr) throw std::invalid_argument( "You must set Conquer function." );
        inpQueue->clear();
        midIQueue->clear();
        midOQueue->clear();
        outQueue->clear();
        if(structureType == 1) {
            inpQueue = new Queue(divideForFarm(problem));
            Farm<outType, inType> Sfarm(solve, std::make_tuple(f_divide, f_baseCaseTest, f_baseCaseSolution, f_conquer), numOfThread[0], true, inpQueue, outQueue);
            Sfarm.wait_end();
            return conquerFromFarm(*outQueue);
        }

        if(structureType == 2) {
            if(numOfThread[1] == 0 && numOfThread[2] == 0) {
                if(numOfThread[0] < 3)
                    throw std::invalid_argument( "You must set 3 mimunun thread." );
                int temp = numOfThread[0];
                numOfThread[0] = temp/3;
                numOfThread[1] = temp/3;
                numOfThread[2] = temp/3;
                if(temp%3 == 1)
                    numOfThread[2]++;
                else if(temp%3 == 2) {
                    numOfThread[0]++;
                    numOfThread[2]++;
                }
            }

            inpQueue->push(problem);

            Farm<inType, inType> Dfarm(f_FarmDivide_E, std::make_tuple(f_divide, f_baseCaseTest, inpQueue, midIQueue), f_FarmDivide_C, std::make_tuple(f_baseCaseTest), numOfThread[0], true, inpQueue, midIQueue);
            Farm<outType, inType> farm(f_baseCaseSolution, std::make_tuple(), numOfThread[1], true, midIQueue, midOQueue);
            Farm<outType, outType> CFarm(f_FarmConquer_E, std::make_tuple(midOQueue, f_conquer), f_FarmConquer_C, std::make_tuple(midOQueue), numOfThread[2], midOQueue, outQueue);
            Dfarm.wait_end();
            farm.wait_end();
            CFarm.wait_end();

            if(outQueue->size() >= 2)//avviene solo in caso in cui size = 2(messo >= per completezza), qesto di ha perche spesso i due thread avendo in controllo asincrono creadono entrambe che che non c'è altro da elamborare e insieriscono il risultato nell'output il problema però nonsussiste perchè il combine finale deve essere fatto necessariamente da un solo thred quindi non avrò "perdite" di tempo
                return conquerFromFarm(*outQueue);
            else
                return outQueue->pop().value();
        }
    }

    /**
     * Classico Solve di un generico DivideAndConquer sequenziale.
     *  Questo è il codice eseguito da un singolo thread della Farm.
     *
     * @param  problem Problema da risolvere con il DivideAndConquer
     * @return         Soluzione del Problema
     */
    static outType solve(inType problem,
        const std::function<std::vector<inType>(inType)>& f_divide,
        const std::function<bool(inType)>& f_baseCaseTest,
        const std::function<outType(inType)>& f_baseCaseSolution,
        const std::function<outType(std::vector<outType>&)>& f_conquer) {
        if (f_baseCaseTest(problem))
            return f_baseCaseSolution(problem);
        else {
            std::vector<inType> subProblems = f_divide(problem);
            std::vector<outType> subSolutions;
            for (inType it : subProblems)
                subSolutions.push_back(solve(it, f_divide, f_baseCaseTest, f_baseCaseSolution, f_conquer));
            while(subSolutions.size() != 1)
                subSolutions.push_back(f_conquer(subSolutions));
            return subSolutions.front();
       }
    }
};

#endif
