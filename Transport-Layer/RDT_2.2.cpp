#include <bits/stdc++.h>

using namespace std;

// the structure of the packet from sender to reciever
struct Packet{
    int seq_num;
    string data;
    int checksum = 0;
};

// the structure of the packet from reciever to sender
struct AckPacket{
    int ack_num;
    int checksum  = 0;
};

double corruption_probability = 0.3;

// function to caluclate the checksum of the packet
int calculate_checksum(struct Packet* pkt){
    int sum = pkt->seq_num;
    for(char c : pkt->data){
        sum += c;
    }
    return sum;
}

int calculate_checksum_4_reciever(struct AckPacket* ack) {
    return ack->ack_num;
}

// function to corrupt the packet by changing the checksum to a random value
Packet corrupt_randomly(struct Packet* pkt){
    Packet corrupted = *pkt;
    int i = rand() % sizeof(Packet);  // just inducing randomness in the checksum
    corrupted.checksum = i;
    return corrupted;
}

AckPacket corrupt_randomly_4_reciever(AckPacket* ack){
    AckPacket corrupted = *ack;
    int i = rand() % sizeof(AckPacket);
    corrupted.checksum = i;
    return corrupted;
}

// function to checkt if the given packet is corrupted or not
bool isCorrupted(struct Packet* pkt) {
    return calculate_checksum(pkt) != pkt->checksum;
}

bool isCorrupted_4_reciever(struct AckPacket* ack) {
    return calculate_checksum_4_reciever(ack) != ack->checksum;
}

// unreliable channel for simulating corruption
Packet* unreliableChannel(Packet* pkt, double corruption_prob);
AckPacket* unreliableChannel_4_reciever(AckPacket* ack, double corruption_prob);

// Now implementing Reciever and Sender FSM

struct AckPacket* receiver(struct Packet* pkt, int* expected_seq) {
    double corruption_prob = 0.3;
    struct AckPacket* ack = new AckPacket;
    if (!isCorrupted(pkt) && pkt->seq_num == *expected_seq) {
        cout << "[Receiver] Received valid packet: " << pkt->data << endl;
        ack->ack_num = pkt->seq_num;                // we send ack for the packet we just received.
        ack->checksum = calculate_checksum_4_reciever(ack);
        *expected_seq = 1 - *expected_seq;
    } else {
        cout << "[Receiver] Corrupted or duplicate packet. Sending duplicate ACK.\n";
        ack->ack_num = 1 - *expected_seq;           // we send ack for the previously correctly recieved packet.
        ack->checksum = calculate_checksum_4_reciever(ack);
    }
    return unreliableChannel_4_reciever(ack, corruption_prob);
}

// Sender FSM
void sender(vector<string>& messages, double corruption_prob) {
    int seq_num = 0;
    int receiver_expected_seq = 0; // initially set sequence number to 0

    for (string& msg : messages) {
        Packet* pkt = new Packet{seq_num, msg, 0};
        pkt->checksum = calculate_checksum(pkt);

        while (true) {
            cout << "\n[Sender] Sending packet: \"" << pkt->data << "\", seq: " << pkt->seq_num << endl;

            Packet* to_send = unreliableChannel(pkt, corruption_prob);          // the packet which we are sending to the receiver

            AckPacket* ack = receiver(to_send, &receiver_expected_seq);         // the acknowlwdgement packet which we are receiving from the receiver

            cout << "[Sender] Received ACK: " << ack->ack_num << ", ";

            if (isCorrupted_4_reciever(ack)){
                cout << "but it's corrupted. Retransmitting..." << endl;
            } 
            else if (ack->ack_num == seq_num){
                cout << "valid ACK. Proceeding to next message." << endl;
                seq_num = 1 - seq_num;
                break;          // exiting the loop as we sent the packet successfully and received the valid ACK
            } 
            else{
                cout << "duplicate ACK. Retransmitting..." << endl;
            }

            delete to_send;
            delete ack;
        }

        delete pkt;
    }
}

// unreliable channel for simulating corruption
Packet* unreliableChannel(Packet* pkt, double corruption_prob) {
    if ((rand() / (double)RAND_MAX) < corruption_prob) {
        return new Packet(corrupt_randomly(pkt));
    } else {
        return new Packet(*pkt);
    }
}

AckPacket* unreliableChannel_4_reciever(AckPacket* ack, double corruption_prob) {
    if ((rand() / (double)RAND_MAX) < corruption_prob) {
        return new AckPacket(corrupt_randomly_4_reciever(ack));
    } else {
        return new AckPacket(*ack);
    }
}


int main() {
    // Seed random number generator for consistent simulation
    srand(time(0));

    // Example set of messages to be sent
    vector<string> messages = {
        "Hello",
        "This is",
        "the simulation",
        "of rdt2.2",
        "protocol",
        "done."
    };

    // Set probability of corruption (e.g., 30%)
    double corruption_probability = 0.3;

    // Start the sender FSM
    sender(messages, corruption_probability);

    cout << "\n All messages sent successfully." << endl;

    return 0;
}

