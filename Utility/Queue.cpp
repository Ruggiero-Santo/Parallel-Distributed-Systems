#ifndef sanQueue
    #include <deque>
    #include <mutex>
    #include <vector>
    #include <optional>
    #include <iostream>
    #include <condition_variable>
    #define sanQueue

template <typename T> class Queue {
    private:
        std::mutex              d_mutex;
        std::condition_variable d_condition;
        std::deque<T>           d_queue;
        bool                    flag_EOS = false;

        bool unlockWait() { return !d_queue.empty() || flag_EOS; }

    public:
        Queue(){}
        Queue(std::deque<T> other){
            this->d_queue = std::deque<T>(other);
        }
        Queue(const Queue& other)  {
            this->d_queue = std::deque<T>(other.d_queue);
            this->flag_EOS = other.flag_EOS;
        }

        Queue& operator=(const Queue& other) {
            this->d_queue = std::deque(other.d_queue);
            this->flag_EOS = other.flag_EOS;
            return *this;
        }

        std::deque<T> getData() const { return std::deque<T>(d_queue);}
        int size() { return d_queue.size(); }

        void clear() {d_queue.clear(); flag_EOS = false;}
        void EOS() {flag_EOS = true; this->d_condition.notify_all();}

        bool isEOS() { return flag_EOS; }
        bool isEmpty() { return d_queue.empty(); }
        bool waitInput() { return !(d_queue.empty() && flag_EOS);}

        void push(T value) {
            {
                std::unique_lock<std::mutex> lock(this->d_mutex);
                d_queue.push_front(value);
            }
            this->d_condition.notify_one();
        }

        std::optional<T> pop() {
            std::unique_lock<std::mutex> lock(this->d_mutex);
            this->d_condition.wait(lock, [=]{ return this->unlockWait(); });
            if(!isEmpty()) {
                T rc = std::move(this->d_queue.back());
                this->d_queue.pop_back();
                return rc;
            } else
                return {};
        }

        std::optional<std::vector<T>> pop(int Nval) {
            std::unique_lock<std::mutex> lock(this->d_mutex);
            this->d_condition.wait(lock, [=]{ return this->unlockWait(); });
            std::vector<T> Vrc;
            for(; Nval != 0 ;--Nval) {
                if(!isEmpty()) {
                    T rc = std::move(this->d_queue.back());
                    this->d_queue.pop_back();
                    Vrc.push_back(rc);
                } else
                    break;
            }
            if(Vrc.size() == 0)
                return {};
            return Vrc;
        }

        void print() {
            std::cout << "{";
            for (T it: d_queue)
                std::cout << it << ", ";
            std::cout << "}" << std::endl;
        }
};
#endif
