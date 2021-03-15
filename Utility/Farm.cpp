#ifndef sanFarm
    #ifndef sanQueue
        #include "Queue.cpp"
    #endif
    #include <thread>
    #include <vector>
    #define sanFarm

//TODO override EOS function in queue in modo tale da lanciare EOS solo quando tutti gli stage hanno finito
template<typename inType, typename outType> class Farm {
    private:
        std::vector<std::thread*> vec_thr;
        Queue<inType>* inputQueue;
        Queue<outType>* outputQueue;

        template<typename Function, typename... Args>
        void runUntil(Function&& func, Args... args) {
            do {
                auto a = inputQueue->pop();
                if(a.has_value())
                    outputQueue->push(func(a.value(), args...));
                else break;
            } while(inputQueue->waitInput());
            outputQueue->EOS();//propaga EOS
        }

    public:
        template<typename Function, typename... Args>
        Farm(Function&& func, int numOfThread, Args... args): Farm(func, numOfThread, new Queue<inType>(), new Queue<outType>(), args...) {}

        template<typename Function, typename... Args>
        Farm(Function&& func, int numOfThread, Queue<inType>* queueInp, Args... args): Farm(func, numOfThread, queueInp, new Queue<outType>(), args...) {}

        template<typename Function, typename... Args>
        Farm(Function&& func, int numOfThread, Queue<inType>* queueInp, Queue<outType>* queueOut, Args... args):inputQueue(queueInp), outputQueue(queueOut) {
            for(; numOfThread != 0; numOfThread--)
                vec_thr.push_back(new std::thread(&Farm::runUntil<Function, Args...>, this, std::ref(func), args...));
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
        }
};
#endif
