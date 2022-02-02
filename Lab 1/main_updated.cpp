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
    int packetId;
    int sourceId;
    double genTimestamp;
    double transTimestamp;
    double queueTimestamp;
    double sinkTimestamp;
};

class Event
{
public:
    int eventType;
    int packetId;
    double timeStamp;
};

// How to compare elements
struct my_comparator
{
    // queue elements are vectors so we need to compare those
    bool operator()(Event const &a, Event const &b) const
    {
        // reverse sort puts the lowest value at the top
        return a.timeStamp > b.timeStamp;
    }
};

int totalTime, switchBandwidth, queueLimit, packetSize;
int numOfSources = 4;
vector<Source> sources(numOfSources);
int numOfPackets;
vector<Packet> packets;
int numOfSinkedPackets;
vector<Packet> sinkedPackets;
int numOfLostPackets;
vector<Packet> lostPackets;
priority_queue<Event, vector<Event>, my_comparator> globalQueue;
ofstream graph1, graph2;

void getSources()
{
    for (int i = 0; i < numOfSources; i++)
    {
        cout << "\nDetails for Source " << i + 1 << "\n";
        sources[i].sourceId = i;
        cout << "Enter packet sending rate (number of packets send / second):";
        cin >> sources[i].rate;
        cout << "Enter bandwidth:";
        cin >> sources[i].bandwidth;
    }
}

void displaySources()
{
    cout << "SOURCES\n";
    cout << "Id | Rate | Bandwidth \n";
    for (int i = 0; i < numOfSources; i++)
        cout << sources[i].sourceId << " | " << sources[i].rate << " | " << sources[i].bandwidth << "\n";
}

void getPackets()
{
    int packetCount = 0;
    for (int i = 0; i < sources.size(); i++)
        for (double j = 0; j < totalTime; j += 1 / (double)(sources[i].rate))
        {
            Packet packet;
            packet.packetId = packetCount++;
            packet.sourceId = i;
            packet.genTimestamp = j;
            packets.push_back(packet);
            Event event;
            event.eventType = 1;
            event.packetId = packet.packetId;
            event.timeStamp = packet.genTimestamp;
            globalQueue.push(event);
        }
    numOfPackets = globalQueue.size();
}

void displayPackets()
{
    cout << "PACKETS\n";
    cout << "SId | Event | GenT\n";
    for (int i = 0; i < packets.size(); i++)
    {
        cout << fixed << setprecision(2) << packets[i].sourceId << " | " << packets[i].genTimestamp << "\n";
    }
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
        Event event = globalQueue.top();

        if (event.eventType == 1)
        {
            // Packet has now generated.
            // Add a event to transmit packet from source to switch

            Event newEvent;
            newEvent.eventType = 2;
            newEvent.packetId = event.packetId;
            newEvent.timeStamp = sourceTime[packets[event.packetId].sourceId] < event.timeStamp ? event.timeStamp : sourceTime[packets[event.packetId].sourceId];
            newEvent.timeStamp += (double)packetSize / sources[packets[event.packetId].sourceId].bandwidth;

            packets[event.packetId].transTimestamp = newEvent.timeStamp;
            sourceTime[packets[event.packetId].sourceId] = newEvent.timeStamp;

            globalQueue.push(newEvent);
        }
        else if (event.eventType == 2)
        {
            // Packet has now been transmitted.
            // Add packet to queue if space available else packet lost

            if (queueLimit == -1 || queueSize < queueLimit)
            {
                queueSize++;
                Event newEvent;
                newEvent.eventType = 3;
                newEvent.packetId = event.packetId;
                newEvent.timeStamp = switchTime < event.timeStamp ? event.timeStamp : switchTime;
                packets[event.packetId].queueTimestamp = newEvent.timeStamp;
                newEvent.timeStamp += (double)packetSize / switchBandwidth;
                packets[event.packetId].sinkTimestamp = newEvent.timeStamp;
                switchTime = newEvent.timeStamp;
                globalQueue.push(newEvent);
            }
            else
            {
                lostPackets.push_back(packets[event.packetId]);
            }
        }
        else if (event.eventType == 3)
        {
            // Packet has been sinked
            // Remove it from queue

            sinkedPackets.push_back(packets[event.packetId]);
            if (queueLimit != -1)
                queueSize--;
        }
        globalQueue.pop();
    }
    numOfLostPackets = lostPackets.size();
    numOfSinkedPackets = sinkedPackets.size();
}

void displaySinkedPackets()
{
    cout << "SINKED PACKETS\n";
    cout << "SId | Event | GenT | TransT | QueueT | SinkT\n";
    for (int i = 0; i < numOfSinkedPackets; i++)
        cout << fixed << setprecision(2) << sinkedPackets[i].sourceId << " | " << sinkedPackets[i].genTimestamp << " | " << sinkedPackets[i].transTimestamp << " | " << sinkedPackets[i].queueTimestamp << " | " << sinkedPackets[i].sinkTimestamp << "\n";
}

void displayLostPackets()
{
    cout << "LOST PACKETS\n";
    cout << "SId | Event | GenT | TransT | QueueT | SinkT\n";
    for (int i = 0; i < numOfLostPackets; i++)
        cout << fixed << setprecision(2) << lostPackets[i].sourceId << " | " << lostPackets[i].genTimestamp << " | " << lostPackets[i].transTimestamp << " | " << lostPackets[i].queueTimestamp << " | " << lostPackets[i].sinkTimestamp << "\n";
}

double calculatePacketArrivalRate()
{
    double avgSourceBandwidth = 0, rate = 0, totalPacketRate = 0;
    for (int i = 0; i < numOfSources; i++)
    {
        rate += min(sources[i].rate, (double)sources[i].bandwidth / packetSize);
    }
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
    {
        delay += sinkedPackets[i].sinkTimestamp - sinkedPackets[i].genTimestamp;
    }
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
    // displayPackets();
    processPackets();
    // displaySinkedPackets();
    // displayLostPackets();
    // cout << fixed << setprecision(2);
    // cout << "\nNumber of packets:" << numOfPackets;
    // cout << "\nNumber of sinked packets:" << numOfSinkedPackets;
    // cout << "\nNumber of lost packets:" << numOfLostPackets;
    // cout << "\nPacket Arrival Rate:" << calculatePacketArrivalRate();
    // cout << "\nSystem Transmission Rate:" << calculateSystemTransmissionCapacity();
    // cout << "\nUtilization Factor:" << calculatePacketArrivalRate() / calculateSystemTransmissionCapacity();
    // cout << "\nAverage Delay:" << calculateAverageDelay();
    // cout << "\nPacket Loss Rate:" << calculatePacketLossRate();
    if (queueLimit == -1)
    {
        // cout << calculatePacketArrivalRate() / calculateSystemTransmissionCapacity() << "  |  " << calculateAverageDelay();
        graph1 << calculatePacketArrivalRate() / calculateSystemTransmissionCapacity() << " " << calculateAverageDelay() << "\n";
    }
    else
    {
        // cout << calculatePacketArrivalRate() / calculateSystemTransmissionCapacity() << "  |  " << calculatePacketLossRate();
        graph2 << calculatePacketArrivalRate() / calculateSystemTransmissionCapacity() << " " << calculatePacketLossRate() << "\n";
    }
}

int main()
{
    graph1.open("graph1.txt");
    graph2.open("graph2.txt");
    int ql;
    cout << "Enter time for experiment:";
    cin >> totalTime;
    cout << "Switch queue limit:";
    cin >> ql;
    cout << "Enter packet size:";
    cin >> packetSize;
    getSources();
    // displaySources();
    for (int i = 1000; i >= 20; i -= 0.05)
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