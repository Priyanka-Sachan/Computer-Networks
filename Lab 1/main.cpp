#include <bits/stdc++.h>
#include <fstream>
using namespace std;

class Source
{
public:
    int sourceId;
    double rate;
    int bandwidth;
};

class Packet
{
public:
    int sourceId;
    int currentEventId;
    double genTimestamp;
    double transTimestamp;
    double queueTimestamp;
    double sinkTimestamp;
};

// How to compare elements
struct my_comparator
{
    // queue elements are vectors so we need to compare those
    bool operator()(Packet const &a, Packet const &b) const
    {
        // reverse sort puts the lowest value at the top
        return a.sinkTimestamp > b.sinkTimestamp;
    }
};

int totalTime, switchBandwidth, queueLimit, packetSize;
int numOfSources = 4;
vector<Source> sources(numOfSources);
int numOfPackets;
priority_queue<Packet, vector<Packet>, my_comparator> globalQueue;
int numOfSinkedPackets;
vector<Packet> sinkedPackets;
int numOfLostPackets;
vector<Packet> lostPackets;
ofstream graph1, graph2;

void getSources()
{
    for (int i = 0; i < numOfSources; i++)
    {
        cout << "\nDetails for Source " << i + 1 << "\n";
        sources[i].sourceId = i + 1;
        cout << "Enter packet sending rate (number of packets send/second):";
        cin >> sources[i].rate;
        cout << "Enter bandwidth (in kb/second):";
        cin >> sources[i].bandwidth;
    }
}

void getPackets()
{
    for (int i = 0; i < sources.size(); i++)
        for (double j = 0; j < totalTime; j += 1 / (double)(sources[i].rate))
        {
            Packet packet;
            packet.sourceId = i + 1;
            packet.currentEventId = 1;
            packet.genTimestamp = j;
            packet.sinkTimestamp = packet.genTimestamp;
            globalQueue.push(packet);
        }
    numOfPackets = globalQueue.size();
}

void processPackets()
{
    // SwitchTime maintained for switch so that it can transmit one packet at a time.
    // SourceTime maintained for each source so that one source can transmit one packet at a time.
    int queueSize = 0;
    double switchTime = 0;
    vector<double> sourceTime(numOfSources, 0);
    while (!globalQueue.empty())
    {
        Packet packet = globalQueue.top();
        // Transmit packet from source
        if (packet.currentEventId == 1)
        {
            packet.currentEventId = 2;
            packet.transTimestamp = sourceTime[packet.sourceId - 1] < packet.genTimestamp ? packet.genTimestamp : sourceTime[packet.sourceId - 1];
            packet.transTimestamp += (double)packetSize / sources[packet.sourceId - 1].bandwidth;
            packet.sinkTimestamp = packet.transTimestamp;
            sourceTime[packet.sourceId - 1] = packet.queueTimestamp;
            globalQueue.push(packet);
        }
        // Add packet to queue if space available
        else if (packet.currentEventId == 2)
        {
            if (queueLimit == -1 || queueSize < queueLimit)
            {
                packet.currentEventId = 3;
                packet.queueTimestamp = switchTime < packet.transTimestamp ? packet.transTimestamp : switchTime;
                packet.sinkTimestamp = packet.queueTimestamp + (double)packetSize / switchBandwidth;
                switchTime = packet.sinkTimestamp;
                globalQueue.push(packet);
                if (queueLimit != -1)
                    queueSize++;
            }
            else
                lostPackets.push_back(packet);
        }
        // Transmit packet from switch to sink
        else if (packet.currentEventId == 3)
        {
            packet.currentEventId = 4;
            sinkedPackets.push_back(packet);
            if (queueLimit != -1)
                queueSize--;
        }
        globalQueue.pop();
    }
    numOfLostPackets = lostPackets.size();
    numOfSinkedPackets = sinkedPackets.size();
}

double calculatePacketArrivalRate()
{
    double avgSourceBandwidth = 0, rate = 0, totalPacketRate = 0;
    for (int i = 0; i < numOfSources; i++)
        rate += min(sources[i].rate, (double)sources[i].bandwidth / packetSize);
    return rate;
}

double calculateSystemTransmissionCapacity()
{
    double rate = 0;
    rate = (double)switchBandwidth / packetSize;
    return rate;
}

double calculateAverageDelay()
{
    double delay = 0;
    for (int i = 0; i < numOfSinkedPackets; i++)
        delay += sinkedPackets[i].sinkTimestamp - sinkedPackets[i].genTimestamp;
    delay /= numOfSinkedPackets;
    return delay;
}

double calculatePacketLossRate()
{
    int numOfLostPackets = numOfPackets - numOfSinkedPackets;
    double packetLossRate = (double)numOfLostPackets / numOfPackets;
    return packetLossRate;
}

void simulateNetwork()
{
    sinkedPackets.clear();
    lostPackets.clear();
    getPackets();
    processPackets();
    cout << fixed << setprecision(2);
    if (queueLimit == -1)
        graph1 << calculatePacketArrivalRate() / calculateSystemTransmissionCapacity() << " " << calculateAverageDelay() << "\n";
    else
        graph2 << calculatePacketArrivalRate() / calculateSystemTransmissionCapacity() << " " << calculatePacketLossRate() << "\n";
}

int main()
{

    graph1.open("graph1.txt");
    graph2.open("graph2.txt");
    int ql;
    cout << "Enter time for experiment (in seconds):";
    cin >> totalTime;
    cout << "Switch queue limit:";
    cin >> ql;
    cout << "Enter packet size (in kb):";
    cin >> packetSize;
    getSources();
    for (int i = 2000; i >= 100; i -= 0.05)
    {
        switchBandwidth = i;
        queueLimit = -1;
        simulateNetwork();
        queueLimit = ql;
        simulateNetwork();
    }
    graph1.close();
    graph2.close();
    system("python plotGraphs.py");
    return 0;
}

//*This one was the latest
// 1000 20 20
// 3 25
// 4 25
// 5 35
// 8 20
// 1000 20 20
// 3 4 5 8
// 100 80 100 200