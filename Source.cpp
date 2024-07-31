#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include<queue>
#include <algorithm>
#include<cstdio>
#include<cstdlib>
#include"project.h"
#include <limits>

using namespace std;


int main()
{
    string username, password;
    string day;
    char ch;
    int targetHour;
    int count=0;
    Graph g, g1, g2, g3, g4, g5;
    int choice1;
    // Initialize a vector to store visited hours
    vector<int> visitedHours;
    // Add bins to the graph
    for (int i = 1; i <= 55; i++)
    {
        if (i > 0 && i <= 10 || i == 51)
            g1.addBin(i);
        if (i > 10 && i <= 20 || i == 52)
            g2.addBin(i);
        if (i > 20 && i <= 30 || i == 53)
            g3.addBin(i);
        if (i > 30 && i <= 40 || i == 54)
            g4.addBin(i);
        if (i > 40 && i <= 50 || i == 55)
            g5.addBin(i);

    }
    // *******************************************************starting system**************************************************
    cout << "\t****************************************************************************************************" << endl;
    cout << "\t\t\tWELCOME TO WASTE COLLECTION OPTIMIZATION SYSTEM" << endl;
    cout << "\t****************************************************************************************************" << endl;
    //cout << endl << "1. Press 1 to Login" << endl;
    cout << "\nAre you a new user? (Y or N): ";
    cin >> ch;

    if (ch == 'Y' || ch == 'y')
    {
        g.registration();
    }
    do {
        cout << "\t\t\t\n\n**********SIGN IN PAGE**********" << endl;
        cout << "Select your role:" << endl;
        cout << ">Press 1 for driver." << endl;
        cout << ">press 2 for controller." << endl;
        cout << ">Press 3 to exit system." << endl;
        cout << "Enter your choice: ";
        cin >> choice1;
        switch (choice1)
        {
        case 1:
            cout << "Enter your username: ";
            cin >> username;
            cout << "Enter your password: ";
            cin >> password;
            if (g.login(username,password,"user.txt")) {
                g.driver(username, password);
            }
            break;
        case 2:
            cout << "Enter your username: ";
            cin >> username;
            cout << "Enter your password: ";
            cin >> password;
            if (g.login(username,password,"user.txt")) {
                int choice3 = 0;
                do {
                    choice3 = g.menu();
                    if (count == 0)
                    {
                        while (true)
                        {
                            cout << "Enter the day you want to check (e.g., Day1): ";
                            cin >> day;
                            cout << "Enter the hour of the day you want to check bin level for: ";
                            cin >> targetHour;
                            if (find(visitedHours.begin(), visitedHours.end(), targetHour) != visitedHours.end())
                            {
                                cout << "Invalid hour. Hour " << targetHour << " has already been processed.\n";// Check if the target hour is present in the vector
                            }
                            else
                            {
                                vector<int> binLevels = g.getBinLevelsForHour(day, targetHour);// Get bin levels
                                // Connect bins based on the connections file within specific ranges
                                g1.connectBinsFromFile("connections1.txt", vector<int>(binLevels.begin() + 0, binLevels.begin() + 10));
                                g2.connectBinsFromFile("connections2.txt", vector<int>(binLevels.begin() + 10, binLevels.begin() + 20));
                                g3.connectBinsFromFile("connections3.txt", vector<int>(binLevels.begin() + 20, binLevels.begin() + 30));
                                g4.connectBinsFromFile("connections4.txt", vector<int>(binLevels.begin() + 30, binLevels.begin() + 40));
                                g5.connectBinsFromFile("connections5.txt", vector<int>(binLevels.begin() + 40, binLevels.begin() + 50));
                                count = 1;
                                break;  // Valid hour
                            }
                        }
                    }
                    switch (choice3)
                    {

                    case 1:
                        g.displaySystem(g1, g2, g3, g4, g5);//display whole city
                        break;
                    case 2:
                        g.displayHighLvl(g1, g2, g3, g4, g5);//display bins with high binlvl
                        break;
                    case 3:
                        g.assign_routes(g1, g2, g3, g4, g5, false,false);// Call route func for all objects
                        break;
                    case 4:
                        g.assign_routes(g1, g2, g3, g4, g5, false,true);//assign routes to drivers
                        cout << "\nnotification sent to all drivers." << endl;
                        break;
                    case 5:
                        g.updateFile(day,targetHour, visitedHours);//check for any alert from the bin
                        cout << "routes have been updated." << endl;
                        g.write();
                        cout << "As there are change in bin level so again assigning routes to truck drivers.";
                        g.assign_routes(g1, g2, g3, g4, g5, false, true);

                        break;
                    case 6:
                        g.assign_routes(g1, g2, g3, g4, g5, true,true);//live tracking of routes
                        break;
                    case 0:
                        cout << "logged out successfully." << endl;
                        visitedHours.push_back(targetHour);
                    }
                } while (choice3 != 0);
                count = 0;
            }
            break;
        case 3:
            cout << "\t*********************************THANKYOU FOR VISITING WCOS********************************************" << endl;
        }
    } while (choice1!=3);
    return 0;
}
