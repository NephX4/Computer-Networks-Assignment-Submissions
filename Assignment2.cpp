#include <iostream>
#include <string>

using namespace std;

int main() {
    string dataInput;
    cout << "Enter 4-bit data message (e.g. 1100): ";
    cin >> dataInput;

    int code[8] = {0};

    code[7] = dataInput[0] - '0';
    code[6] = dataInput[1] - '0';
    code[5] = dataInput[2] - '0';
    code[3] = dataInput[3] - '0';

    code[1] = code[3] ^ code[5] ^ code[7];
    code[2] = code[3] ^ code[6] ^ code[7];
    code[4] = code[5] ^ code[6] ^ code[7];

    cout << "\n=== ENCODED RESULT ===" << endl;
    cout << "Parity Bits        : P1 = " << code[1] << ", P2 = " << code[2] << ", P4 = " << code[4] << endl;
    cout << "Generated Codeword: ";
    for (int i = 7; i >= 1; i--) {
        cout << code[i];
    }
    cout << "\n\n";
    
    //Decoding and Error Checking
    
    string receivedInput;
    cout << "Enter 7-bit received codeword (e.g. 1000001): ";
    cin >> receivedInput;

    int received[8] = {0};

    // Store received data in reverse
    received[7] = receivedInput[0] - '0';
    received[6] = receivedInput[1] - '0';
    received[5] = receivedInput[2] - '0';
    received[4] = receivedInput[3] - '0';
    received[3] = receivedInput[4] - '0';
    received[2] = receivedInput[5] - '0';
    received[1] = receivedInput[6] - '0';

    // Calculate Syndrome Bits
    int s1 = received[1] ^ received[3] ^ received[5] ^ received[7];
    int s2 = received[2] ^ received[3] ^ received[6] ^ received[7];
    int s4 = received[4] ^ received[5] ^ received[6] ^ received[7];

    // Decimal error position find
    int errorPos = (s4 * 4) + (s2 * 2) + (s1 * 1);

    cout << "\n=== DECODING RESULT ===" << endl;
    cout << "Syndrome (S4 S2 S1) : " << s4 << s2 << s1 << endl;
    cout << "Error Position      : " << errorPos << endl;

    if (errorPos != 0) {
        cout << "\nError detected at position " << errorPos << "!" << endl;

        // Flip the corrupted bit (0 to 1, 1 to 0 etc)
        received[errorPos] ^= 1;

        cout << "Corrected Codeword   : ";
        for (int i = 7; i >= 1; i--) {
            cout << received[i];
        }
        cout << endl;
    } else {
        cout << "\nNo error detected in transmission." << endl;
    }

    return 0;
}
