#include <bits/stdc++.h>

using namespace std;

void reno(string &data, int cwnd, int ssthresh, int MSS_size) {
    int sent = 0;
    while (sent < data.size()) {
        int remaining = data.size() - sent;
        int chunks_in_round = min(cwnd, (remaining + MSS_size - 1) / MSS_size); 
        int loss_occurred = 0;

        for (int j = 0; j < chunks_in_round; j++) {
            int start = sent + j * MSS_size;
            string chunk = data.substr(start, MSS_size);
            cout << "Sending chunk: " << chunk << endl;
            
        }
        
        // After sending all chunks in the round, check for loss
        if (rand() % 100 < 35 && cwnd > 0){  // 15% chance of packet loss               // if user wants to have better simulation then if MSS = 1 -> 20%, 2 -> 35%, 3 -> 50%
            loss_occurred = rand() % 100;
            if(loss_occurred > 50 && cwnd > 0){
                cout << "Packet loss detected due to recognition of a triple duplicate ACK. new cwnd = " << cwnd / 2 << ", new ssthresh = " << cwnd / 2 << endl << endl;
                ssthresh = cwnd / 2;
                cwnd /= 2; 
            } 
            else if(loss_occurred <= 50 && cwnd > 0){
                cout << "Packet loss detected due to a packet Timeout. new cwnd = " << 1 << ", new ssthresh = " << cwnd / 2 << endl << endl;
                ssthresh = cwnd / 2;
                cwnd = 1; 
            }
        }
        else{
            cout << "All packets in round sent." ;
            // Update cwnd based on current phase (slow start or congestion avoidance)
            if (cwnd < ssthresh) {
                cwnd *= 2;  // Exponential growth in slow start
            } else {
                cwnd += 1;  // Linear growth in congestion avoidance
            }
            cout << " cwnd = " << cwnd << ", ssthresh = " << ssthresh << endl << endl;
        }
        
        sent += chunks_in_round * MSS_size;
    }
    cout << "All data sent." << endl;
}

int main() {
    srand(time(0));  
    string data = "Hello.this.is.a.test.message.to.be.sent.over.TCP.Reno.protocol.I.am.using.this.to.test.the.tahoe.protocol.";
    int cwnd, ssthresh, MSS_size;
    cout << "Enter the value of cwnd: ";
    cin >> cwnd;
    cout << "Enter the value of initial ssthresh: ";
    cin >> ssthresh;
    cout << "Enter the value of MSS_size: ";
    cin >> MSS_size;
    cout << endl;
    reno(data, cwnd, ssthresh, MSS_size);
    return 0;
}