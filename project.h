#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include<queue>
#include <algorithm>
#include<cstdio>
#include<cstdlib>
#include <limits>

using namespace std;

struct Edge
{
    int destination;
    int weight;
    int binLevel;
    Edge(int dest, int w, int lvl) : destination(dest), weight(w), binLevel(lvl) {}
};

class Graph
{
public:
    int count1 = 0;
    map<int, std::vector<Edge>> adjacencyList;
    void addEdge(int source, int destination, int weight, int binLevel)
    {
        Edge edge(destination, weight, binLevel);
        adjacencyList[source].push_back(edge);
    }

    void addBin(int binID)// Function to add bin
    {
        adjacencyList[binID] = {};  // Initialize with an empty vector for edges
    }

    void connectBinsFromFile(const string& connectionsFile, const vector<int>& binLevelsRange)// Function to connect bins based on the provided connections file
    {
        ifstream read;
        read.open(connectionsFile, ios::in);
        if (!read.is_open()) {
            cout << "Error opening connections1 file.\n";
            return;
        }
        int arr[11][4];  // Assuming a maximum of 10 bins and 4 connections per bin+ 1 dumping site
        for (int i = 0; i < 11; i++) {
            for (int j = 0; j < 4; j++) {
                read >> arr[i][j];
            }
        }
        read.close();
        // Connecting bins based on the read connections
        for (int i = 0; i < 11; i++) {
            int sourceBin = arr[i][0];
            for (int j = 1; j < 4; j++) {
                int destinationBin = arr[i][j];
                if (destinationBin != 0) {
                    int weight = readWeight(sourceBin, destinationBin);
                    if (i < 10) {
                        int binLevel = binLevelsRange[i];  // Get the corresponding bin level
                        addEdge(sourceBin, destinationBin, weight, binLevel);
                    }
                    else
                        addEdge(sourceBin, destinationBin, weight, 0);
                }

            }
        }
    }
    // Function to display the graph
    void displayGraph()
    {
        ifstream inputFile("locations.txt");
        if (!inputFile.is_open()) {
            cout << "Error opening locations file." << endl;
            return;
        }
        map<int, std::string> binLocations;
        int binID;
        string location;
        while (inputFile >> binID >> location) {
            binLocations[binID] = location;
        }
        inputFile.close();
        for (const auto& pair : adjacencyList) {
            int sourceBin = pair.first;
            if (sourceBin >= 51 && sourceBin <= 55) {
                cout << "Dumping site " << sourceBin % 50 << " : " << binLocations[sourceBin] << " is connected to:" << "\t";
            }
            else {
                cout << "Bin " << sourceBin << " : " << binLocations[sourceBin] << ", bin_lvl: " << pair.second.front().binLevel << " is connected to:" << "\t";
            }
            for (const auto& edge : pair.second) {
                cout << " Bin " << edge.destination << " having distance: " << edge.weight << "\t";
            }
            cout << endl;
        }
        cout << endl;
    }
    // Function to read weight from the connections file based on source and destination bins
    int readWeight(int sourceBin, int destinationBin)
    {
        ifstream read;
        int i = sourceBin;
        string file;
        if (i > 0 && i <= 10 || i == 51)
            file = "distance1.txt";
        else if (i > 10 && i <= 20 || i == 52)
            file = "distance2.txt";
        else if (i > 20 && i <= 30 || i == 53)
            file = "distance3.txt";
        else if (i > 30 && i <= 40 || i == 54)
            file = "distance4.txt";
        else if (i > 40 && i <= 50 || i == 55)
            file = "distance5.txt";
        else {
            cout << "Invalid bin ID.\n";
            return -1;
        }
        read.open(file, std::ios::in);
        if (!read.is_open()) {
            cout << "Error opening connections file for reading weight.\n";
            return -1;
        }
        int weight = -1;
        int currentSource, currentDest, currentWeight;
        while (read >> currentSource >> currentDest >> currentWeight) {
            if ((currentSource == sourceBin && currentDest == destinationBin) || (currentSource == destinationBin && currentDest == sourceBin)) {
                weight = currentWeight;
                break;
            }
        }
        read.close();
        return weight;
    }
    vector<int> getBinLevelsForHour(string day, int targetHour)
    {
        vector<int> binLevels;
        ifstream dataFile(day + ".txt");
        if (!dataFile.is_open()) {
            cerr << "Error opening Bin level Data files." << endl;
            return binLevels;  // Empty vector indicates an error
        }

        // Find the row corresponding to the target hour
        string line;
        bool found = false;
        while (getline(dataFile, line)) {
            istringstream iss(line);
            int hour;
            iss >> hour;
            // Check if the current row corresponds to the target hour
            if (hour == targetHour) {
                found = true;
                // Store the values in an array
                int binLevel;
                while (iss >> binLevel) {
                    binLevels.push_back(binLevel);
                }
                break;
            }
        }
        // If the target hour was not found
        if (!found) {
            cout << "Hour " << targetHour << " not found in the data." << endl;
        }
        dataFile.close();
        return binLevels;
    }
    void dijkstraShortestPath(int startBin, int endBin, ofstream& output, int& currentTime, bool displayTime)
    {
        // Using a priority queue to store unvisited bins with distances and times and 
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        map<int, pair<int, int>> distanceTime;
        // Initialize distances and times
        for (const auto& pair : adjacencyList)
        {
            int binID = pair.first;
            distanceTime[binID] = { numeric_limits<int>::max(), 0 };
        }
        // Set the distance for the starting bin to 0 and time to the current time
        distanceTime[startBin] = { 0, currentTime };
        pq.push({ 0, startBin });
        // Dijkstra's algorithm
        while (!pq.empty()) {
            int currentDistance = distanceTime[pq.top().second].first;
            int currentBin = pq.top().second;
            int currentTime = distanceTime[pq.top().second].second; // Update the current time
            pq.pop();
            // Update the distances and times of neighbors
            for (const auto& edge : adjacencyList[currentBin]) {
                int neighborBin = edge.destination;
                int newDistance = currentDistance + edge.weight;
                int newTime = currentTime + edge.weight; // Update the time
                // Only update if the new distance is smaller
                if (newDistance < distanceTime[neighborBin].first) {
                    distanceTime[neighborBin] = { newDistance, newTime };
                    pq.push({ distanceTime[neighborBin].first, neighborBin });
                }
            }
        }
        // Display the shortest path
        cout << "Shortest Path from Bin " << startBin << " to Bin " << endBin << " : ";
        output << "Shortest Path from Bin " << startBin << " to Bin " << endBin << " : ";
        int currentBin = endBin;
        while (currentBin != startBin)
        {
            cout << "Bin " << currentBin;
            if (displayTime) {
                cout << "(time: " << distanceTime[currentBin].second << ")";
            }
            cout << " <- ";
            output << "Bin " << currentBin;
            if (displayTime) {
                output << "(time: " << distanceTime[currentBin].second << ")";
            }
            output << " <- ";
            int nextBin = -1;
            int minDistance = numeric_limits<int>::max();

            // Find the neighbor with the minimum distance
            for (const auto& edge : adjacencyList[currentBin])
            {
                if (distanceTime[edge.destination].first < minDistance)
                {
                    minDistance = distanceTime[edge.destination].first;
                    nextBin = edge.destination;
                }
            }

            currentBin = nextBin;
        }
        // Display the starting bin
        cout << "Bin " << startBin;
        if (displayTime) {
            cout << "(time: " << distanceTime[startBin].second << ")";
        }
        cout << endl;
        output << "Bin " << startBin;
        if (displayTime) {
            output << "(time: " << distanceTime[startBin].second << ")";
        }
        output << endl;

        // Update the current time
        currentTime = distanceTime[endBin].second;
    }
    void dijkstraShortestPath2(int startBin, int endBin, int& currentTime, bool displayTime)
    {
        // Using a priority queue to store unvisited bins with distances and times and 
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        map<int, pair<int, int>> distanceTime;
        // Initialize distances and times
        for (const auto& pair : adjacencyList)
        {
            int binID = pair.first;
            distanceTime[binID] = { numeric_limits<int>::max(), 0 };
        }
        // Set the distance for the starting bin to 0 and time to the current time
        distanceTime[startBin] = { 0, currentTime };
        pq.push({ 0, startBin });
        // Dijkstra's algorithm
        while (!pq.empty()) {
            int currentDistance = distanceTime[pq.top().second].first;
            int currentBin = pq.top().second;
            int currentTime = distanceTime[pq.top().second].second; // Update the current time
            pq.pop();
            // Update the distances and times of neighbors
            for (const auto& edge : adjacencyList[currentBin]) {
                int neighborBin = edge.destination;
                int newDistance = currentDistance + edge.weight;
                int newTime = currentTime + edge.weight; // Update the time
                // Only update if the new distance is smaller
                if (newDistance < distanceTime[neighborBin].first) {
                    distanceTime[neighborBin] = { newDistance, newTime };
                    pq.push({ distanceTime[neighborBin].first, neighborBin });
                }
            }
        }
        // Display the shortest path
        cout << "Shortest Path from Bin " << startBin << " to Bin " << endBin << " : ";

        int currentBin = endBin;
        while (currentBin != startBin)
        {
            cout << "Bin " << currentBin;
            if (displayTime) {
                cout << "(time: " << distanceTime[currentBin].second << ")";
            }
            cout << " <- ";

            int nextBin = -1;
            int minDistance = numeric_limits<int>::max();

            // Find the neighbor with the minimum distance
            for (const auto& edge : adjacencyList[currentBin])
            {
                if (distanceTime[edge.destination].first < minDistance)
                {
                    minDistance = distanceTime[edge.destination].first;
                    nextBin = edge.destination;
                }
            }

            currentBin = nextBin;
        }
        // Display the starting bin
        cout << "Bin " << startBin;
        if (displayTime) {
            cout << "(time: " << distanceTime[startBin].second << ")";
        }
        cout << endl;

        // Update the current time
        currentTime = distanceTime[endBin].second;
    }

    // Add this function to the Graph class
    vector<int> binlvl(int threshold) const
    {
        vector<int> highLevelBins;
        for (const auto& pair : adjacencyList)
        {
            int binID = pair.first;
            // Check if the binID exists in adjacencyList
            if (adjacencyList.find(binID) != adjacencyList.end() && !adjacencyList.at(binID).empty() && adjacencyList.at(binID).front().binLevel >= threshold)
            {
                highLevelBins.push_back(binID);
            }
        }
        return highLevelBins;
    }
    // Function to display the route with or without time information
    void route(int start, int threshold, string text, bool displayTime)
    {
        ofstream output;
        output.open(text, ios::app);
        if (count1 == 1) {
            output << "\nRoute has been updated!" << endl;
        }
        vector<int> highLevelBins = binlvl(threshold);
        // Check if highLevelBins is empty
        if (highLevelBins.empty()) {
            cout << "No bin is full or almost full." << endl;
        }
        else {
            int startingPoint = start; // Set your desired starting point
            int currenttime = 0;
            for (int bin : highLevelBins) {
                // Open the file before the loop
                output.open(text, ios::app);
                dijkstraShortestPath(startingPoint, bin, output, currenttime, displayTime);
                startingPoint = bin;
                // Close the file inside the loop
                output.close();
            }
        }
        count1 = 1;
        // Close the file after the loop
        output.close();
    }
    void route2(int start, int threshold, string text, bool displayTime)
    {

        vector<int> highLevelBins = binlvl(threshold);
        // Check if highLevelBins is empty
        if (highLevelBins.empty()) {
            cout << "No bin is full or almost full." << endl;
        }
        else {
            int startingPoint = start; // Set your desired starting point
            int currenttime = 0;
            for (int bin : highLevelBins)
            {
                dijkstraShortestPath2(startingPoint, bin, currenttime, displayTime);
                startingPoint = bin;
            }
        }
    }
    void displaySystem(Graph g1, Graph g2, Graph g3, Graph g4, Graph g5)
    {
        cout << "\t\t\t\t**************************F7********************************" << endl;
        g1.displayGraph();
        cout << "\t\t\t\t**************************F8********************************" << endl;
        g2.displayGraph();
        cout << "\t\t\t\t**************************G8********************************" << endl;
        g3.displayGraph();
        cout << "\t\t\t\t**************************G9********************************" << endl;
        g4.displayGraph();
        cout << "\t\t\t\t**************************G10********************************" << endl;
        g5.displayGraph();
        cout << "\t\t\t\t*************************************************************" << endl << endl;
    }
    bool login(string username, string password, const string& text) {
        ifstream file(text);
        string line;
        if (!file) {
            cerr << "Error opening file: " << text << endl;
            return false;
        }
        while (getline(file, line)) {
            istringstream iss(line);
            string storedUsername, storedPassword;
            if (iss >> storedUsername >> storedPassword) {
                if (storedUsername == username && storedPassword == password) {
                    cout << "\nLogged in successfully!!!.\n";
                    file.close();
                    return true;
                }
            }
        }
        cout << "\nLogin failed. Incorrect username or password.\n";
        file.close();
        return false;
    }

    void registration()
    {
        cout << "\t\t\t\n\n**********Registration Page**********" << endl;

        string username, password;

        cout << "\nEnter a username: ";
        cin >> username;

        cout << "Enter a password: ";
        cin >> password;

        ofstream file("user.txt", ios::app);

        if (!file.is_open()) {
            cerr << "\nError opening user file." << endl;
            return;
        }

        file << username << " " << password << "\n";

        cout << "\nRegistration successful!\n";

        file.close();
    }
    void driver(const string& username, const string& password) {
        int choice2;
        cout << "Do you want to check if there is any dumping waste assignment for you (1/0): ";
        cin >> choice2;
        if (choice2) {
            string filename = username + ".txt";
            ifstream read(filename);
            if (!read.is_open()) {
                cerr << "Error opening the file: " << filename << endl;
                return;
            }
            cout << "Dumping waste assignment for " << username << " and you will be using following route:" << endl;
            string line;
            while (getline(read, line)) {
                cout << line << endl;
            }
            read.close();
        }
    }

    void displayHighLvl(Graph g1, Graph g2, Graph g3, Graph g4, Graph g5)
    {
        cout << "\t\t\t\t*************************************************************" << endl << endl;
        cout << "Following bins need to be cleared:" << endl;
        vector<int> highLevelBins1 = g1.binlvl(75);
        cout << "\t*****F7*****" << endl;
        for (int bin : highLevelBins1) {
            cout << "Bin " << bin << endl;
        }
        vector<int> highLevelBins2 = g2.binlvl(75);
        cout << "\t*****F8*****" << endl;
        for (int bin : highLevelBins2) {
            cout << "Bin " << bin << endl;
        }
        vector<int> highLevelBins3 = g3.binlvl(75);
        cout << "\t*****G8*****" << endl;
        for (int bin : highLevelBins3) {
            cout << "Bin " << bin << endl;
        }
        vector<int> highLevelBins4 = g4.binlvl(75);
        cout << "\t*****G9*****" << endl;
        for (int bin : highLevelBins4) {
            cout << "Bin " << bin << endl;
        }
        vector<int> highLevelBins5 = g5.binlvl(75);
        cout << "\t*****G10*****" << endl;
        for (int bin : highLevelBins5) {
            cout << "Bin " << bin << endl;
        }
        cout << "\t\t\t\t*************************************************************" << endl << endl;
    }
    void updateFile(string update_bin, int targethour, vector<int>& visited_hours) {
        ifstream infile(update_bin + ".txt");
        ofstream outfile("temp.txt");
        string line;
        while (getline(infile, line)) {
            stringstream ss(line);
            int hour, value;
            // Read the hour from the first column
            ss >> hour;
            if (hour == targethour) {
                // If the target hour is found in the first column
                int rand_col = rand() % 4 + 2; // Random column selection
                vector<int> values;
                // Loop through the rest of the columns and store their values
                while (ss >> value) {
                    values.push_back(value);
                }

                // Randomly select a column and set its value to 75 or higher
                int old_value = values[rand_col - 2];
                values[rand_col - 2] = 75;

                // Delete or add the target hour to the visited_hours vector
                auto it = find(visited_hours.begin(), visited_hours.end(), targethour);
                if (it != visited_hours.end()) {
                    visited_hours.erase(it);
                }

                // Write the updated line to the output file
                outfile << hour << " ";
                for (int j = 0; j < values.size(); ++j) {
                    outfile << values[j] << " ";
                }
                outfile << '\n';

                // Print the information about the changed column
                cout << "Bin ID " << rand_col << " changed bin level from " << old_value << " to 75" << endl;
            }
            else {
                // If the target hour is not found, write the line to the output file
                outfile << line << '\n';
            }
        }
        infile.close();
        outfile.close();
        string filename = update_bin + ".txt";
        // Remove the original file
        remove(filename.c_str());
        // Rename the temporary file to the original filename
        rename("temp.txt", filename.c_str());
    }

    void assign_routes(Graph g1, Graph g2, Graph g3, Graph g4, Graph g5, bool time, bool write)
    {
        // If writeToFiles is true, ask for assigning drivers
        if (write && !time)
        {
            string  sector1, sector2, sector3, sector4, sector5;
            cout << "Assign sectors to drivers:";
            cout << endl << "F7 to : ";
            cin >> sector1;
            cout << "F8 to : ";
            cin >> sector2;
            cout << "G8 to : ";
            cin >> sector3;
            cout << "G9 to : ";
            cin >> sector4;
            cout << "G10 to : ";
            cin >> sector5;
            cout << "\n\t\t\t***********************ROUTES********************************" << endl;
            cout << "\t\t\t\t****** F7 sector -> " << sector1 << " ******" << endl;
            g1.route(1, 75, sector1 + ".txt", time);
            cout << "\t\t\t\t****** F8 sector -> " << sector2 << " *****" << endl;
            g2.route(11, 75, sector2 + ".txt", time);
            cout << "\t\t\t\t****** G8 sector -> " << sector3 << " *****" << endl;
            g3.route(21, 75, sector3 + ".txt", time);
            cout << "\t\t\t\t****** G9 sector -> " << sector4 << " *****" << endl;
            g4.route(31, 75, sector4 + ".txt", time);
            cout << "\t\t\t\t****** G10 sector -> " << sector5 << " *****" << endl;
            g5.route(41, 75, sector5 + ".txt", time);
        }
        if (write && time)
        {
            cout << "\n\t\t\t***********************ROUTES********************************" << endl;
            cout << "\t\t\t\t****** F7 sector -> " << " ******" << endl;
            g1.route(1, 75, "driver1.txt", time);
            cout << "\t\t\t\t****** F8 sector -> " << " *****" << endl;
            g2.route(11, 75, "driver2.txt", time);
            cout << "\t\t\t\t****** G8 sector -> " << " *****" << endl;
            g3.route(21, 75, "driver3.txt", time);
            cout << "\t\t\t\t****** G9 sector -> " << " *****" << endl;
            g4.route(31, 75, "driver4.txt", time);
            cout << "\t\t\t\t****** G10 sector ->" << " *****" << endl;
            g5.route(41, 75, "driver5.txt", time);
        }
        else
        {
            cout << "\n\t\t\t***********************ROUTES*******************************" << endl;
            cout << "\t\t\t\t****** F7 Markaz*****" << endl;
            g1.route2(1, 75, "driver1.txt", time);
            cout << "\t\t\t\t****** F8 Markaz*****" << endl;
            g2.route2(11, 75, "driver2.txt", time);
            cout << "\t\t\t\t****** G8 Markaz*****" << endl;
            g3.route2(21, 75, "driver3.txt", time);
            cout << "\t\t\t\t****** G9 Markaz*****" << endl;
            g4.route2(31, 75, "driver4.txt", time);
            cout << "\t\t\t\t****** G10 Markaz*****" << endl;
            g5.route2(41, 75, "driver5.txt", time);
        }
        cout << "\t\t\t*************************************************************" << endl;
    }
    int menu()
    {
        int choice3;
        cout << "\t\t\t\n\n**********HOME PAGE**********" << endl;
        cout << "Press 1 to display all Dumping system of Islamabad." << endl;
        cout << "Press 2 to display bins with higher bin level." << endl;
        cout << "Press 3 to display all routes to dump high bin level bins." << endl;
        cout << "Press 4 to send notifications to respective truck driver." << endl;
        cout << "press 5 to check any changes in bin lvls since truck left." << endl;
        cout << "press 6 to view live tracking of trucks." << endl;
        cout << "Press 0 to logout." << endl;
        cout << "enter your choice:";
        cin >> choice3;
        return choice3;
    }
    void write()
    {
        for (int i = 1; i <= 5; ++i) {
            string driverFile = "driver" + to_string(i) + ".txt";
            ofstream out(driverFile, ios::app);
            out << "route has been updated!" << endl;
            out.close();
        }
    }

};
