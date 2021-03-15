#ifndef sanLoopbackFarm
    #ifndef sanQueue
        #include "Queue.cpp"
    #endif
    #include <thread>
    #include <vector>
    #define sanLoopbackFarm

    //TODO farmi passare le tuple inceve che gli args cosi si possono memorizzare
        // non ho problemi di parameti con valori di default e
        // posso inserire vari parametri per ogni funzione che mi passano
 template<typename inType, typename outType> class LoopbackFarm {
    private:
        std::vector<std::thread*> vec_thr;
        Queue<inType>* inputQueue;
        Queue<outType>* outputQueue;

        template<typename Function, typename... Args>
        void runUntil(Function&& func, Args... args) {
            do {
                auto a = func(args...);
                if(a.has_value())
                    inputQueue->push(a.value());
            } while(inputQueue->size() > 1);
            auto a = inputQueue->pop();
            if(a.has_value())
                outputQueue->push(a.value());
            outputQueue->EOS();//propaga EOS
        }
    public:
        template<typename Function, typename... Args>
        LoopbackFarm(Function&& funcExe, int numOfThread, Args... args): LoopbackFarm(funcExe, numOfThread, new Queue<inType>(), new Queue<outType>(), args...) {}

        template<typename Function, typename... Args>
        LoopbackFarm(Function&& funcExe, int numOfThread, Queue<inType>* queueInp, Args... args): LoopbackFarm(funcExe, numOfThread, queueInp, new Queue<outType>(), args...) {}

        template<typename Function, typename... Args>
        LoopbackFarm(Function&& func, int numOfThread, Queue<inType>* queueInp, Queue<outType>* queueOut, Args... args):inputQueue(queueInp), outputQueue(queueOut) {
            for(; numOfThread != 0; numOfThread--)
                vec_thr.push_back(new std::thread(&LoopbackFarm::runUntil<Function, Args...>, this, std::ref(func), args...));
        }
        ~LoopbackFarm() {
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
        }
};
#endif
