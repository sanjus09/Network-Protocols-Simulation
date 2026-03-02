#include <bits/stdc++.h>

using namespace std;

struct Packet {
    int seq_num;
    string data;
    int checksum;
};

struct AckPacket {
    int ack_num;
    int checksum;
};

struct TimerState {
    mutex mtx;
    condition_variable cv;
    bool active = false;
    bool expired = false;
    int generation = 0; // Track timer generations
};

TimerState timer;
int sender_seq = 0;
int receiver_seq = 0;
const int MAX_ATTEMPTS = 20;
const int TIMEOUT_MS = 1000;

int packet_checksum(const Packet& pkt) {
    int sum = pkt.seq_num;
    for (char c : pkt.data) sum += c;
    return sum;
}

int ack_checksum(const AckPacket& ack) {
    return ack.ack_num ; 
}

void timer_thread(int duration, int generation) {
    unique_lock<mutex> lock(timer.mtx);
    if (!timer.active || timer.generation != generation) return;

    auto status = timer.cv.wait_for(lock, chrono::milliseconds(duration));
    if (status == cv_status::timeout && timer.generation == generation) {
        timer.expired = true;
    }
    timer.active = false;
    timer.cv.notify_all();
}

void start_timer() {
    unique_lock<mutex> lock(timer.mtx);
    timer.expired = false;
    timer.active = true;
    timer.generation++; // New generation for each timer
    
    thread t(timer_thread, TIMEOUT_MS, timer.generation);
    t.detach();
}

void stop_timer() {
    unique_lock<mutex> lock(timer.mtx);
    timer.expired = true;
    timer.active = false;
    timer.generation++; // Invalidate old timers
    timer.cv.notify_all();
}

bool transmit(const Packet& sent, Packet& received, double loss, double corrupt) {
    if (rand()/(double)RAND_MAX < loss) return false;
    
    received = sent;
    if (rand()/(double)RAND_MAX < corrupt && !received.data.empty()) {
        received.data[0] = ~received.data[0];
    }
    return true;
}

bool transmit(const AckPacket& sent, AckPacket& received, double loss, double corrupt) {
    if (rand()/(double)RAND_MAX < loss) return false;
    
    received = sent;
    if (rand()/(double)RAND_MAX < corrupt) {
        received.ack_num = 1 - received.ack_num;
    }
    return true;
}

AckPacket receive_packet(const Packet& pkt) {
    AckPacket ack;
    bool valid = packet_checksum(pkt) == pkt.checksum;
    bool expected = pkt.seq_num == receiver_seq;

    if (!valid) {
        cout << "[Receiver] Corrupted! Expected: " << receiver_seq << endl;
        ack.ack_num = 1 - receiver_seq; // Send last valid ACK
    }
    else if (expected) {
        cout << "[Receiver] Valid: \"" << pkt.data << "\" seq=" << pkt.seq_num << endl;
        receiver_seq = 1 - receiver_seq;
        ack.ack_num = pkt.seq_num;
    }
    else {
        cout << "[Receiver] Unexpected seq: " << pkt.seq_num 
             << " (expecting " << receiver_seq << ")" << " *Packet discarded....*" << endl;
        ack.ack_num = 1 - receiver_seq; // Send last valid ACK
    }

    ack.checksum = ack_checksum(ack);
    return ack;
}

void sender(const vector<string>& messages, double loss, double corrupt) {
    for (const auto& msg : messages) {
        int attempts = 0;
        bool success = false;

        while (!success && attempts++ < MAX_ATTEMPTS) {
            Packet pkt{sender_seq, msg, 0};
            pkt.checksum = packet_checksum(pkt);

            cout << "\n[Sender] Attempt " << attempts 
                 << " | Seq: " << sender_seq 
                 << " | Data: \"" << pkt.data << "\"\n";

            start_timer();
            
            Packet rx_pkt;
            bool transmitted = transmit(pkt, rx_pkt, loss, corrupt);
            AckPacket rx_ack;
            bool ack_received = false;

            if (transmitted) {
                AckPacket ack = receive_packet(rx_pkt);
                ack_received = transmit(ack, rx_ack, loss, corrupt);
            }

            if (ack_received) {
                bool valid = ack_checksum(rx_ack) == rx_ack.checksum;
                bool match = rx_ack.ack_num == sender_seq;

                if (valid && match) {
                    cout << "[Sender] Valid ACK " << rx_ack.ack_num << endl;
                    stop_timer();
                    sender_seq = 1 - sender_seq;
                    success = true;
                    continue;
                }
            }

            unique_lock<mutex> lock(timer.mtx);
            timer.cv.wait(lock, [&]{ return timer.expired; });
            cout << "[Sender] Timeout expired\n";
        }

        if (!success) {
            cerr << "[Sender] FAILED to send: \"" << msg << "\" after " 
                 << MAX_ATTEMPTS << " attempts\n";
            exit(1);
        }
    }
}

int main() {
    srand(time(0));
    vector<string> messages = {
        "Hello", "RDT3.0", "Reliable", "Data", 
        "Transfer", "Test", "Complete"
    };

    sender(messages, 0.3, 0.3);
    cout << "\nAll messages transmitted successfully!\n";
    return 0;
}