#include <iostream>
#include <string>

using namespace std;

void findIPClass(string ip) {
    int firstOctet = 0;

    // Read characters until we hit the first '.'
    for (char c : ip) {
        if (c == '.') {
            break; // Stop when the first octet ends
        }
       
        // Ensure character is a digit
        if (c >= '0' && c <= '9') {
            firstOctet = firstOctet * 10 + (c - '0');
        }
    }

    // Determine the IP class
    if (firstOctet >= 1 && firstOctet <= 127) {
        cout << "IP Address          : " << ip << "\n";
        cout << "Class               : Class A\n";
        cout << "IP Range            : 1.0.0.0 to 126.255.255.255\n";
        cout << "Default Subnet Mask : 255.0.0.0\n";
        cout << "Hosts per Network   : 2^24 - 2 (16,777,214)\n";
        cout << "Number of Networks  : 2^7 (128)\n";
    }
    else if (firstOctet >= 128 && firstOctet <= 191) {
        cout << "IP Address          : " << ip << "\n";
        cout << "Class               : Class B\n";
        cout << "IP Range            : 128.0.0.0 to 191.255.255.255\n";
        cout << "Default Subnet Mask : 255.255.0.0\n";
        cout << "Hosts per Network   : 2^16 - 2 (65,534)\n";
        cout << "Number of Networks  : 2^14 (16,384)\n";
    }
    else if (firstOctet >= 192 && firstOctet <= 223) {
        cout << "IP Address          : " << ip << "\n";
        cout << "Class               : Class C\n";
        cout << "IP Range            : 192.0.0.0 to 223.255.255.255\n";
        cout << "Default Subnet Mask : 255.255.255.0\n";
        cout << "Hosts per Network   : 2^8 - 2 (254)\n";
        cout << "Number of Networks  : 2^21 (2,097,152)\n";
    }
    else {
        cout << "IP Address          : " << ip << "\n";
        cout << "Class               : Not Class A, B, or C (Class D, E, or invalid range)\n";
    }
}

int main() {
    cout<<"What is Subnet?\n";
    cout<<"A subnet (short for subnetwork) is a logical subdivision of an IP network. By dividing a single large network into multiple smaller, distinct sub-networks, organizations can reduce broadcast traffic, improve routing efficiency, enhance security by isolating network segments, and prevent address exhaustion.\n\n";
   
    cout<<"What is a Subnet Mask?\n";
    cout<<"A 32-bit number (like 255.255.255.0) that splits an IP address into two parts: the network part and the host part. It tells devices whether another computer is on the same local network or a different one.\n\n";
   
    cout<<"What is Loopback Addressing and its purpose?\n";
    cout<<"A special reserved IP address (127.0.0.1, or localhost) that points directly back to your own device.\n";
    cout<<"Purpose: Used to test if your computer's networking software is working, or to run web servers and apps locally without sending data onto a physical network.\n\n";
    cout<<"Why do we subtract 2 to find the Hosts per Network ID?\n";
   
    cout<<"Out of all total IP addresses in a network, two are permanently reserved and cannot be assigned to devices:\n";

    cout<<"First address (All 0s): The Network ID — identifies the network itself.\n";

    cout<<"Last address (All 1s): The Broadcast Address — sends messages to every device on that network simultaneously.\n";

    cout<<"What is Data-Link layer? explain in detail.\n";
    cout<<"The Data-Link Layer is Layer 2 of the OSI model responsible for the reliable node-to-node transfer of data between directly connected devices on the same local network.\n";
    cout<<"It takes packets from the network layer, encapsulates them into frames, and uses physical MAC addresses to ensure data reaches the correct destination.\n";
    cout<<"Additionally, it handles error detection and flow control to manage transmission reliability and prevent a fast sender from overwhelming a slow receiver.\n\n";
    string ipAddress;

    cout << "Enter an IPv4 address (e.g., 172.16.0.1): ";
    cin >> ipAddress;

    findIPClass(ipAddress);

    return 0;
}
