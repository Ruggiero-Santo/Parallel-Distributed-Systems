#ifndef sanFarm
    #ifndef sanQueue
        #include "Queue.cpp"
    #endif
    #include <thread>
    #include <vector>
    #include <tuple>
    #define sanFarm

 template<typename inType, typename outType> class Farm {
    private:
        std::vector<std::thread*> vec_thr;
        Queue<inType>* inputQueue;
        Queue<outType>* outputQueue;
        bool takeElement;

        //Funzione wrapper per una farm classica la funzione richiede il valore in input
        template<typename FunctionE, typename TupleE>
        void runUntil_T(FunctionE&& funcE, TupleE tupleE) {
            do {
                auto a = inputQueue->pop();
                if(a.has_value())
                    outputQueue->push(std::apply(funcE, std::tuple_cat(std::make_tuple(a.value()), tupleE)));
                else if(!inputQueue->waitInput()) break;
            } while(inputQueue->waitInput());
        }
        //Funzione wrapper per una farm classica la funzione non richiede il valore in input ma provvede da se
        template<typename FunctionE, typename TupleE>
        void runUntil_F(FunctionE&& funcE, TupleE tupleE) {
            do {
                auto a = std::apply(funcE, tupleE);
                if(a.has_value())
                    outputQueue->push(a.value());
                else if(!inputQueue->waitInput()) break;
            } while(inputQueue->waitInput());
        }
        //Funzione wrapper per una Loopbackfarm classica la funzione di esecuzione e di controllo richiede il valore in input
        template<typename FunctionE, typename TupleE, typename FunctionC, typename TupleC>
        void runUntil_T(FunctionE&& funcE, TupleE tupleE,  FunctionC&& funcC, TupleC tupleC) {
            do {
                auto a = inputQueue->pop();
                if(a.has_value()) {
                    auto b = std::apply(funcE, std::tuple_cat(std::make_tuple(a.value()), tupleE));
                    inputQueue->tellpush();
                    if(std::apply(funcC, std::tuple_cat(std::make_tuple(b), tupleC)))
                        inputQueue->push(b);
                    else
                        outputQueue->push(b);
                } else if(!inputQueue->waitInput()) break;
            } while(inputQueue->waitInput());
        }
        //Funzione wrapper per una Loopbackfarm classica la funzione di esecuzione e di controllo non richiede il valore in input ma provvede da se
        template<typename FunctionE, typename TupleE, typename FunctionC, typename TupleC>
        void runUntil_F(FunctionE&& funcE, TupleE tupleE,  FunctionC&& funcC, TupleC tupleC) {
            do {
                auto a = std::apply(funcE, tupleE);
                if(a.has_value()) {
                    inputQueue->tellpush();
                    if(std::apply(funcC, tupleC))
                        inputQueue->push(a.value());
                    else
                        outputQueue->push(a.value());
                } else if(!inputQueue->waitInput()) break;
            } while(inputQueue->waitInput());
        }

    public:
        /**
         * Classica farm il cui comportamento è definito dai seguenti parametri. I threa continueranno la loro esecuzione fino a quando non si aspetteranno altri dati.
         * @param funcE                 outType(inType, ArgsE...) funzione eseguita dal thread
         * @param argsE                 parametri fissi che si hanno bisogno per la funzione
         * @param numOfThread           numero di Thread che si vogliono creare
         * @param takeElementFormInputQ la sua presenza indica il fatto che la fuzione vorrà cme parametro un inType preso della Queue
         * @param queueInp              Queue contenenete di dati da elaborare
         * @param queueOut              Queue contenenete di dati che sono stati elaborati
         */
        template<typename FunctionE, typename... ArgsE>
        Farm(FunctionE&& funcE, std::tuple<ArgsE...> argsE, int numOfThread, bool takeElementFormInputQ,
                    Queue<inType>* queueInp= nullptr, Queue<outType>* queueOut = nullptr) {
            inputQueue = queueInp == nullptr? new Queue<inType>(): queueInp;
            outputQueue = queueOut == nullptr? new Queue<outType>(): queueOut;

            for(; numOfThread != 0; numOfThread--)
                vec_thr.push_back(new std::thread(&Farm::runUntil_T<FunctionE, std::tuple<ArgsE...>>, this, std::ref(funcE), argsE));
        }
        /**
         * Classica farm il cui comportamento è definito dai seguenti parametri. I thread continueranno la loro esecuzione fino a quando non si aspetteranno altri dati.
         * L'assenza di questo parameto (takeElementFormInputQ) indica che la funzione prenderà i dati da se
         * @param funcE                 std::optional<outType>(ArgsE...) funzione eseguita dal thread
         * @param argsE                 parametri fissi che si hanno bisogno per la funzione
         * @param numOfThread           numero di Thread che si vogliono creare
         * @param queueInp              Queue contenenete di dati da elaborare
         * @param queueOut              Queue contenenete di dati che sono stati elaborati
         */
        template<typename FunctionE, typename... ArgsE>
        Farm(FunctionE&& funcE, std::tuple<ArgsE...> argsE, int numOfThread,
                    Queue<inType>* queueInp= nullptr, Queue<outType>* queueOut = nullptr) {
            inputQueue = queueInp == nullptr? new Queue<inType>(): queueInp;
            outputQueue = queueOut == nullptr? new Queue<outType>(): queueOut;

            for(; numOfThread != 0; numOfThread--)
                vec_thr.push_back(new std::thread(&Farm::runUntil_F<FunctionE, std::tuple<ArgsE...>>, this, std::ref(funcE), argsE));
        }
        /**
         * LoopBackfarm il cui comportamento è definito dai seguenti parametri. I thread continueranno la loro esecuzione fino a quando non si aspetteranno altri dati, e inseriscono il valore calcolato nella queue di input o di output come incato dalla funzione di controllo
         *
         * @param funcE                 std::optional<outType>(inType,ArgsE...) funzione eseguita dal thread
         * @param argsE                 parametri fissi che si hanno bisogno per la funzione
         * @param funcC                 bool(outType, ArgsC...) funzione eseguita dal thread per il controllo per verificare in quale Queue deve essere inserito il valore dell'esecuzione
         * @param argsC                 parametri fissi che si hanno bisogno per la funzione di controllo
         * @param numOfThread           numero di Thread che si vogliono creare
         * @param queueInp              Queue contenenete di dati da elaborare
         * @param queueOut              Queue contenenete di dati che sono stati elaborati
         */
        template<typename FunctionE, typename... ArgsE, typename FunctionC, typename... ArgsC>
        Farm(FunctionE&& funcE, std::tuple<ArgsE...> argsE, FunctionC&& funcC, std::tuple<ArgsC...> argsC, int numOfThread, bool takeElementFormInputQ,
                    Queue<inType>* queueInp= nullptr, Queue<outType>* queueOut = nullptr) {
            inputQueue = queueInp == nullptr? new Queue<inType>(): queueInp;
            outputQueue = queueOut == nullptr? new Queue<outType>(): queueOut;

            for(; numOfThread != 0; numOfThread--)
                vec_thr.push_back(new std::thread(&Farm::runUntil_T<FunctionE, std::tuple<ArgsE...>, FunctionC, std::tuple<ArgsC...>>, this, std::ref(funcE), argsE, std::ref(funcC), argsC));
        }
        /**
         * LoopBackfarm il cui comportamento è definito dai seguenti parametri. I thread continueranno la loro esecuzione fino a quando non si aspetteranno altri dati, e inseriscono il valore calcolato nella queue di input o di output come incato dalla funzione di controllo
         * L'assenza di questo parameto (takeElementFormInputQ) indica che la funzione prenderà i dati da se
         * @param funcE                 std::optional<outType>(ArgsE...) funzione eseguita dal thread
         * @param argsE                 parametri fissi che si hanno bisogno per la funzione
         * @param funcC                 bool(ArgsC...) funzione eseguita dal thread per il controllo per verificare in quale Queue deve essere inserito il valore dell'esecuzione
         * @param argsC                 parametri fissi che si hanno bisogno per la funzione di controllo
         * @param numOfThread           numero di Thread che si vogliono creare
         * @param queueInp              Queue contenenete di dati da elaborare
         * @param queueOut              Queue contenenete di dati che sono stati elaborati
         */
        template<typename FunctionE, typename... ArgsE, typename FunctionC, typename... ArgsC>
        Farm(FunctionE&& funcE, std::tuple<ArgsE...> argsE, FunctionC&& funcC, std::tuple<ArgsC...> argsC, int numOfThread,
                    Queue<inType>* queueInp= nullptr, Queue<outType>* queueOut = nullptr) {
            inputQueue = queueInp == nullptr? new Queue<inType>(): queueInp;
            outputQueue = queueOut == nullptr? new Queue<outType>(): queueOut;

            for(; numOfThread != 0; numOfThread--)
                vec_thr.push_back(new std::thread(&Farm::runUntil_F<FunctionE, std::tuple<ArgsE...>, FunctionC, std::tuple<ArgsC...>>, this, std::ref(funcE), argsE, std::ref(funcC), argsC));
        }
        ~Farm() {
            this->wait_end();
        }

        template <typename... Args>
        void start_process(inType value, Args... args) {
            start_process(value);
            start_process(args...);
        }
        void start_process(inType value) {
            inputQueue->push(value);
        }

        Queue<outType> getOuput() const {
            return *outputQueue;
        }

        void wait_end() {
            inputQueue->EOS();
            for(auto it : vec_thr)
                if(it->joinable()) it->join();
            outputQueue->EOS();//propaga EOS
        }
};
#endif
