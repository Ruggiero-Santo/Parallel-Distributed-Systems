#ifndef sanDivideAndConquer
    #include "Utility/Farm.cpp"
    #include "Utility/LoopbackFarm.cpp"
    #include "Utility/Queue.cpp"
    #include <functional>
    #include <deque>
    #define sanDivideAndConquer

template< typename inType, typename outType> class DivideAndConquer {
private:

    Queue<inType>* inpQueue = new Queue<inType>();
    Queue<outType>* outQueue = new Queue<outType>();

    const std::function<std::vector<inType>(inType)>& f_divide;
    const std::function<bool(inType)>& f_baseCaseTest;
    const std::function<outType(inType)>& f_baseCaseSolution;
    const std::function<outType(std::vector<outType>&)>& f_conquer;

    int numOfThread;

    /**
     * Funzione di preparazione per la Farm. Non fa altro che dividere il
     *  problema fino ad avere tanti subProblems quanti i thread dichiarati.
     *
     * @param  problem Problema principale da suddividere.
     * @return         Queue contenente i vari subProblems da sottoporre alla Farm.
     */
    Queue<inType> divideForFarm(inType problem) {
        std::deque<inType> subProblems;
        subProblems.push_back(problem);
        while (subProblems.size() < numOfThread) {//eseguo il divide sul problema per creare subProblems per la Farm fino ad averne almeno uno per nodo della Farm
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
        return Queue(subProblems);
    }

    /**
     * Funzione di acquisizione delle soluzioni prodotte dalla Farm. Non fa altro
     *  che applicare il conquer sulle subSolution per ottenere la soluzione finale.
     * @param  subSolutions Queue contenente i risultati provenenti dai subProblems
     * @return              Soluzione del problema originale.
     */
    outType conquerFromFarm(Queue<outType> subSolutions) {
        //TODO try to create a loopbackFarm class for this type of computation
        std::deque<outType> temp = subSolutions.getData();
        std::vector<outType> _subSolutions(temp.begin(), temp.end());
        while(_subSolutions.size() != 1) //eseguo il conquer su le varie soluzioni che mi sono arrivate dalla Farm fino ad avere una soluzione
            _subSolutions.push_back(f_conquer(_subSolutions));
        return _subSolutions.front();
    }

    static std::optional<outType> conquerFromFarmL(Queue<outType>* subSolutions, const std::function<outType(std::vector<outType>&)>& f_conquer) {
        auto a = subSolutions->pop(2);
        if(a.has_value()) {
            std::vector<outType> temp  = a.value();
            if(temp.size() == 1)
                return temp[0];
            return f_conquer(temp);
        } else
            return {};
    }


public:
    DivideAndConquer(
        const std::function<std::vector<inType>(inType)>& f_divide = nullptr,
        const std::function<bool(inType)>& f_baseCaseTest = nullptr,
        const std::function<outType(inType)>& f_baseCaseSolution = nullptr,
        const std::function<outType(std::vector<outType>&)>& f_conquer = nullptr,
        int numOfThread = 1):
            f_divide(f_divide),
            f_baseCaseTest(f_baseCaseTest),
            f_baseCaseSolution(f_baseCaseSolution),
            f_conquer(f_conquer),
            numOfThread(numOfThread) {
                if (numOfThread < 1)
                    this->numOfThread = 1;
            }

    void setDivedeFunc(const std::function<std::vector<inType>(inType)>& f) {f_divide = f;}
    void setBaseCaseTestFunc(const std::function<bool(inType)>& f) {f_baseCaseTest = f;}
    void setBaseCaseSolutionFunc(const std::function<outType(inType)>& f) {f_baseCaseSolution = f;}
    void setConquerFunc(const std::function<outType(std::vector<outType>&)>& f) {f_conquer = f;}
    void setnumOfThread(int numOfThread) {this->numOfThread = numOfThread;}

    outType compute(inType problem) {
        if (f_divide == nullptr) throw std::invalid_argument( "You must set Divide function." );
        if (f_baseCaseTest == nullptr) throw std::invalid_argument( "You must set Test for base Problem function." );
        if (f_baseCaseSolution == nullptr) throw std::invalid_argument( "You must set Solution for base Problem function." );
        if (f_conquer == nullptr) throw std::invalid_argument( "You must set Conquer function." );
        inpQueue->clear();
        outQueue->clear();

        inpQueue = new Queue(divideForFarm(problem));
        inpQueue->EOS();
        Farm<outType, inType> farm(solve, numOfThread, inpQueue, outQueue, f_divide, f_baseCaseTest, f_baseCaseSolution, f_conquer);
        farm.wait_end();

        LoopbackFarm<outType, outType> LFarm(conquerFromFarmL, 1, outQueue, inpQueue, outQueue, f_conquer);
        LFarm.wait_end();
        return inpQueue->pop().value();
    }

    /**
     * CLassico Solve di un generico DivideAndConquer sequenziale.
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
