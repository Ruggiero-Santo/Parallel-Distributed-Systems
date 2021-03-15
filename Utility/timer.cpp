#ifndef sanTimer
    #include <chrono>
    #include <string>
    #include <iostream>
    #define sanTimer

std::chrono::high_resolution_clock::time_point start;

void tic() {
    start = std::chrono::high_resolution_clock::now();
}

std::string toc(bool verbose = true) {
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    unsigned long long  nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
    std::string time_ = "";
        int temp = (int) (nsec / 604800000000000.0);nsec %= 604800000000000;
        if (temp != 0) time_ += std::to_string(temp) + "week ";
        temp = (int) (nsec / 86400000000000.0); nsec %= 86400000000000;
        if (temp != 0) time_ += std::to_string(temp) + "day ";
        temp = (int) (nsec / 3600000000000.0); nsec %= 3600000000000;
        if (temp != 0) time_ += std::to_string(temp) + "h ";
        temp = (int) (nsec / 60000000000.0); nsec %= 60000000000;
        if (temp != 0) time_ += std::to_string(temp) + "min ";
        temp = (int) (nsec / 1000000000.0); nsec %= 1000000000;
        if (temp != 0) time_ += std::to_string(temp) + "s ";
        temp = (int) (nsec / 1000000.0); nsec %= 1000000;
        if (temp != 0) time_ += std::to_string(temp) + "ms ";
        temp = (int) (nsec / 1000.0); nsec %= 1000;
        if (temp != 0) time_ += std::to_string(temp) + "us ";
        if (nsec != 0) time_ += std::to_string(nsec) + "ns";
    time_ = "Elapsed time: " + time_;
    if(verbose)
        std::cout << time_ << std::endl;
    return time_;
}

void active_delay(int msecs) {
    auto _start = std::chrono::high_resolution_clock::now();
    auto end = false;
    while( !end ) {
        auto elapsed = std::chrono::high_resolution_clock::now() - _start;
        auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        if ( msec > msecs )
            end = true;
    }
    return;
}
#endif
