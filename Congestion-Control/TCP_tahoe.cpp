#include <bits/stdc++.h>

using namespace std;

void tahoe(string &data, int cwnd, int ssthresh, int MSS_size) {
    int sent = 0;
    while (sent < data.size()) {
        int remaining = data.size() - sent;
        int chunks_in_round = min(cwnd, (remaining + MSS_size - 1) / MSS_size); 
        bool loss_occurred = false;
        
        // Send all chunks in the current round
        for (int j = 0; j < chunks_in_round; j++) {
            int start = sent + j * MSS_size;
            string chunk = data.substr(start, MSS_size);
            cout << "Sending chunk: " << chunk << endl;
            
            if (rand() % 100 < 20) {  // 20% chance of packet loss
                loss_occurred = true;
            }
        }

        if (loss_occurred) {
            cout << "Packet loss detected. new cwnd = 1, new ssthresh = " << cwnd / 2 << endl << endl;
            ssthresh = cwnd / 2;
            cwnd = 1;
        } else {
            cout << "All packets in round sent.";
            // Updating cwnd based on current phase....... (slow start or congestion avoidance)
            if (cwnd < ssthresh) {
                cwnd *= 2;  // Exponential growth in slow start
            } else {
                cwnd += 1;  // Linear growth in congestion avoidance
            }
            cout << " cwnd =  " << cwnd <<  ", ssthresh = " << ssthresh << endl << endl;
        }
        
        // Updating the total sent data for the next round.......
        sent += chunks_in_round * MSS_size;
        if (sent > data.size()) {
            sent = data.size();
        }
    }
    cout << "All data sent." << endl;
}

int main() {
    srand(time(0));  // Seed for random number generation
    string data = "Hello.this.is.a.test.message.to.be.sent.over.TCP.Tahoe.protocol.I.am.using.this.to.test.the.tahoe.protocol.";
    int cwnd, ssthresh, MSS_size;
    cout << "Enter the initial value of cwnd: ";
    cin >> cwnd;
    cout << "Enter the initial value of ssthresh: ";
    cin >> ssthresh;
    cout << "Enter the value of MSS_size: ";
    cin >> MSS_size;
    cout << endl;
    tahoe(data, cwnd, ssthresh, MSS_size);
    return 0;
}