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
        int                     check = 0;

        bool unlockWait(int toPop = 1) { return d_queue.size() >= toPop || flag_EOS; }

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

        void setData(std::deque<T> other)  { for(auto it : other) push(it);}
        std::deque<T> getData() const { return std::deque<T>(d_queue);}
        int size() { return d_queue.size();}

        void clear() {d_queue.clear(); flag_EOS = false;}
        void EOS() {flag_EOS = true; this->d_condition.notify_all();}

        bool isEOS() { return flag_EOS; }
        bool isEmpty() { return d_queue.empty(); }
        bool waitInput() { return !(d_queue.empty() && flag_EOS);}

        int getCheck() {return check;}

        void tellpush() { std::unique_lock<std::mutex> lock(this->d_mutex); check += 1;}

        void push(T value) {
            {
                std::unique_lock<std::mutex> lock(this->d_mutex);
                d_queue.push_front(value);
            }
            this->d_condition.notify_one();
        }

        /**
         * preleva un solo dato
         * @return il dato prelevato o {} nel caso in cui non si sia nulla
         */
        std::optional<T> pop() {
            std::unique_lock<std::mutex> lock(this->d_mutex);
            this->d_condition.wait(lock, [=]{ return this->unlockWait(); });
            if(!isEmpty()) {
                T rc = std::move(this->d_queue.back());
                this->d_queue.pop_back();
                check -= 1;
                return rc;
            } else
                return {};
        }

        /**
         * Preleva, eliminando dalla struttura, il numero indicato di dati
         * @param Nval quantità di dati da prelevare
         *
         * @return Un vettore di contenente i dati che sono stati prelevati è optional perchè se non ci sono dati restituisce {}
         */
        std::optional<std::vector<T>> pop(int Nval) {
            std::unique_lock<std::mutex> lock(this->d_mutex);
            this->d_condition.wait(lock, [=]{ return this->unlockWait(Nval); });
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

            check -= 1;
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
