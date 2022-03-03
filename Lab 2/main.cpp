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

int numOfSources, switchBandwidth, queueLimit, packetSize, avgQueueSize, roundNo;
vector<Source> sources;
int numOfPackets, numOfSinkedPackets, numOfLostPackets;
vector<Packet> packets, sinkedPackets, lostPackets;
priority_queue<Event, vector<Event>, my_comparator> globalQueue;
ofstream graph1, graph2, graph3, graph4;

void getSources(vector<double> rate, int bandwidth, bool sourceRateSame)
{
    for (int i = 0; i < numOfSources; i++)
    {
        Source source;
        source.sourceId = i;
        if (sourceRateSame)
            source.rate = rate[0];
        else
            source.rate = rate[i + 1];
        if (!sourceRateSame)
            bandwidth = bandwidth + (double)roundNo / 10;
        source.bandwidth = bandwidth;
        sources.push_back(source);
    }
}

void getPackets(bool sourceRateSame, bool queueInfinite)
{
    int packetCount = 0;
    for (int i = 0; i < sources.size(); i++)
    {
        double firstPacketTime = 0.01;
        for (double u = firstPacketTime, j = firstPacketTime; u < 1 && j < 1000; u += 0.005, j += -1 * log(1 - u) / sources[i].rate)
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
    }
    numOfPackets = globalQueue.size();
}

void processPackets(bool queueInfinite)
{
    // SwitchTime maintained for switch so that it can transmit one packet at a time.
    // SourceTime maintained for each source so that one source can transmit one packet at a time.
    int queueSize = 0;
    avgQueueSize = 0;
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

            if (queueInfinite || queueSize < queueLimit)
            {
                queueSize++;
                avgQueueSize += queueSize;
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
            if (!queueInfinite)
                queueSize--;
        }
        globalQueue.pop();
    }
    numOfLostPackets = lostPackets.size();
    numOfSinkedPackets = sinkedPackets.size();
    avgQueueSize /= numOfSinkedPackets;
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

vector<double> calculateAverageDelayPerSource()
{
    vector<double> delayPerSource(numOfSources, 0);
    vector<int> numOfSinkedPacketsPerSource(numOfSources, 0);
    for (int i = 0; i < numOfSinkedPackets; i++)
    {
        int sourceId = sinkedPackets[i].sourceId;
        delayPerSource[sourceId] += sinkedPackets[i].sinkTimestamp - sinkedPackets[i].genTimestamp;
        numOfSinkedPacketsPerSource[sourceId]++;
    }
    for (int sourceId = 0; sourceId < numOfSources; sourceId++)
        delayPerSource[sourceId] /= numOfSinkedPacketsPerSource[sourceId];
    return delayPerSource;
}

vector<double> calculatePacketLossRatePerSource()
{
    vector<double> packetLossRatePerSource(numOfSources, 0);
    vector<int> numOfPacketsPerSource(numOfSources, 0), numOfLostPacketsPerSource(numOfSources, 0);
    for (int i = 0; i < numOfSinkedPackets; i++)
    {
        int sourceId = sinkedPackets[i].sourceId;
        numOfPacketsPerSource[sourceId]++;
    }
    for (int i = 0; i < numOfLostPackets; i++)
    {
        int sourceId = lostPackets[i].sourceId;
        numOfPacketsPerSource[sourceId]++;
        numOfLostPacketsPerSource[sourceId]++;
    }
    for (int sourceId = 0; sourceId < numOfSources; sourceId++)
        packetLossRatePerSource[sourceId] = (double)numOfLostPacketsPerSource[sourceId] / numOfPacketsPerSource[sourceId];
    return packetLossRatePerSource;
}

void simulateNetwork(vector<double> sourceRate, int sourceBandwidth, bool queueInfinite, bool sourceRateSame)
{
    sources.clear();
    packets.clear();
    sinkedPackets.clear();
    lostPackets.clear();

    getSources(sourceRate, sourceBandwidth, sourceRateSame);
    getPackets(sourceRateSame, queueInfinite);
    processPackets(queueInfinite);

    if (queueInfinite && sourceRateSame)
    {
        graph1 << sources[0].rate << " " << calculateAverageDelay() << "\n";
        graph2 << sources[0].rate << " " << avgQueueSize << "\n";
    }
    else if (queueInfinite && !sourceRateSame)
    {
        vector<double> delayPerSource = calculateAverageDelayPerSource();
        for (int i = 0; i < numOfSources; i++)
        {
            graph3 << delayPerSource[i];
            if (i == numOfSources - 1)
                graph3 << "\n";
            else
                graph3 << " ";
        }
    }
    else if (!queueInfinite && !sourceRateSame)
    {
        vector<double> packetLossRatePerSource = calculatePacketLossRatePerSource();
        for (int i = 0; i < numOfSources; i++)
        {
            graph4 << packetLossRatePerSource[i];
            if (i == numOfSources - 1)
                graph4 << "\n";
            else
                graph4 << " ";
        }
    }
}

int main()
{
    graph1.open("graph1.txt");
    graph2.open("graph2.txt");
    graph3.open("graph3.txt");
    graph4.open("graph4.txt");

    int sourceBandwidth;
    vector<double> sourceRate;
    cout << "Switch queue limit: ";
    cin >> queueLimit;
    cout << "Enter packet size: ";
    cin >> packetSize;
    cout << "Number of sources: ";
    cin >> numOfSources;
    cout << "Enter source bandwidth: ";
    cin >> sourceBandwidth;
    cout << "Enter switch bandwidth: ";
    cin >> switchBandwidth;
    for (int i = 0; i <= numOfSources; i++)
    {
        double rate;
        if (i == 0)
            cout << "Packet Generation Rate (lambda) for all sources: ";
        else
            cout << "Unique packet Generation Rate (lambda) for source " << i << ": ";
        cin >> rate;
        sourceRate.push_back(rate);
    }

    // Lambda from  sourceRate[0]+0.1 to  sourceRate[0]+5
    for (double i = 0; i < 100; i++)
    {
        sourceRate[0] += 0.1;
        // Graph1 and 2
        simulateNetwork(sourceRate, sourceBandwidth, true, true);
    }

    for (double i = 0; i < 100; i++)
    {
        roundNo = i + 1;
        // Graph3
        simulateNetwork(sourceRate, sourceBandwidth, true, false);

        // Graph4
        simulateNetwork(sourceRate, sourceBandwidth, false, false);
    }

    graph1.close();
    graph2.close();
    graph3.close();
    graph4.close();
    system("python plotGraphs.py");
    return 0;
}
