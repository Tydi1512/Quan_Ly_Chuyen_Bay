#pragma once
#ifndef FLIGHT_H
#define FLIGHT_H

#include<fstream>
#include<string>
#include<cstring>
#include"Planes.h"
#include"Passengers.h"
#include"Day.h"
#include <ctime>
#include <vector>
#include <algorithm>
#include <map>

#define MAX_ID_FLIGHT 15
#define MAX_ARRIVE 40

using namespace std;

enum Status
{
    CANCLE_FLIGHT = 0,
    HAVE_TICKET,
    OUT_OF_TICKET,
    COMPLETE_FLIGHT
};


struct Flight
{
public:
    char idFlight[MAX_ID_FLIGHT + 1];
    DateType date;
    char arrive[MAX_ARRIVE + 1];
    char idPlane[MAX_ID_PLANE + 1];

    /*
    * 0 huy chuyen
    * 1 con ve
    * 2 het ve
    * 3 hoan tat
    */
    Status status;

    //-----CHI SO PHU
    char** ticketList = 0;
    map<string, int> passengers;
    int totalTicket = 0;
    bool isValib;
    Flight(){};

    Flight(string idF, string idP, string d, string arr, Status sta, int total)
        : status((Status)sta), totalTicket(total), isValib(true)
    {
        strcpy(idFlight, idF.c_str());
        strcpy(idPlane, idP.c_str());
        date = DateType(d);
        strcpy(arrive, arr.c_str());
    }

    int findLocationPassengerByID(string id)
    {
        for (auto it : passengers)
        {
            if (it.first == id)
                return it.second;
        }
        return -1;
    }

    void removePassenger(string str)
    {
        auto itr = passengers.begin();
        for (auto &it : passengers)
        {
            if (it.first == str)
                break;
            itr++;
        }
        passengers.erase(itr);
        totalTicket--;
    }

    int compareDate(string time)
    {
        int y, m ,d;
        if (sscanf(time.c_str(), "%d:%d:%d", &y, &m, &d) != 3)
        {
            printf("ERROR: time is'n format!!\n");
            return -1;
        }
        if (date.year == y && date.month == m && date.day == d)
        {
            return 0;
        }
        return 1;
    }
};

struct FlightNode
{
    Flight info;
    FlightNode* next;
};
typedef FlightNode* PTR;

class FlightManager
{
public:
    PTR flightList;
    FlightManager()
    {
        flightList = NULL;
    }

    PTR newNode(Flight& flight)
    {
        PTR p = new FlightNode;
        p->info = flight;
        p->next = NULL;
        return p;
    }

    bool isEmpty()
    {
        return(flightList == NULL ? true : false);
    }

    void insertAfter(Flight& flight)
    {
        PTR temp = newNode(flight);

        if (flightList == NULL) {
            flightList = temp;
            flightList->next = NULL;
        }
        else
        {
            PTR p = flightList;
            while (p->next != NULL)
                p = p->next;
            p->next = temp;
        }
    }

    int getSize() {

        int cnt = 0;

        if (flightList == NULL)
            return cnt;

        for (PTR k = flightList; k != NULL; k = k->next) {
            cnt++;
        }
        return cnt;
    }

    void adjustFlight(PTR& flight, DateType& date) {
        flight->info.date = date;
    }

    //pre la nhap, str la chuoi co san
    bool isPrefix(const char* pre, const char* str) {
        if (strlen(pre) > strlen(str))
            return false;
        for (int i = 0; i < (int)strlen(pre); i++)
            if (pre[i] != str[i])
                return false;

        return true;
    }

    PTR findFlight(const char  id[MAX_ID_FLIGHT + 1])
    {
        for (PTR p = flightList; p != NULL; p = p->next)
            if (strcmp(p->info.idFlight, id) == 0)
                return p;
        return NULL;
    }

    PTR findFlightByIdFlight(const char id[MAX_ID_PLANE + 1]) {
        for (PTR k = flightList; k != NULL; k = k->next) {
            if (strcmp(k->info.idFlight, id) == 0)
                return k;
        }
        return NULL;
    }

    int countTicketLeft(PTR& flight)
    {
        int cnt = 0;
        for (int i = 0; i < flight->info.totalTicket; i++) {
            if (strcmp(flight->info.ticketList[i], "0") == 0)
                cnt++;
        }
        return cnt;
    }

    void checkFull(PTR flight) {
        if (countTicketLeft(flight) == 0)
            flight->info.status = Status::OUT_OF_TICKET;
        else
            flight->info.status = Status::HAVE_TICKET;
    }

    bool checkCancleFlight(PTR& first) {
        if (first->info.status == HAVE_TICKET || first->info.status == OUT_OF_TICKET) {
            first->info.status = CANCLE_FLIGHT;
            return true;
        }
        return false;
    }

    void checkCompleted(PlanesList& planeList, PTR& temp)
    {
        if (temp->info.status == HAVE_TICKET || temp->info.status == OUT_OF_TICKET)
        {
            DateType now;
            now = now.getCurTime();

            if (now.calSpaceTime(temp->info.date) >= 0) {
                if (countTicketLeft(temp) == temp->info.totalTicket) {
                    temp->info.status = CANCLE_FLIGHT;
                    return;
                }

                temp->info.status = COMPLETE_FLIGHT;
                planeList.data[planeList.findPlane(temp->info.idPlane)]->flyTimes++;

            }
        }
    }



    void checkCompletedAll(PlanesList& planeList) {
        for (PTR k = flightList; k != NULL; k = k->next) {
            checkCompleted(planeList, k);
        }
    }

    //Kiem tra 1 hanh khach tren 1 chuyen bay co trung ID PASS khong
    int checkDupIDOnFlight(char id[MAX_ID_PASS + 1]) {
        for (int i = 0; i < flightList->info.totalTicket; i++) {
            if (strcmp(flightList->info.ticketList[i], id) == 0) {
                return i + 1;
            }
        }
        return -1; // CMND khong trung
    }


    //Kiem tra xem hanh khach co dat ve tren chuyen bay nao < 12 tieng
    PTR checkPassOnOtherFlightIn12Hours(PTR& flight, char id[MAX_ID_PASS + 1])
    {
        for (PTR k = flightList; k != NULL; k = k->next)
        {
            if (k != flight)
            {
                for (int i = 0; i < k->info.totalTicket; i++)
                {
                    if (strcmp(k->info.ticketList[i], id) == 0 && strcmp(id, "0") != 0 &&
                        !k->info.date.in12Hour(flight->info.date) && (k->info.status == 1 || k->info.status == 2)) {

                        return k;
                    }
                }
            }
        }
        return NULL;
    }

    //Kiem tra xem co the chinh sua thoi gian khi co 1 hanh khach cung ngoi tren 2 chuyen bay
    PTR canEditTime(PTR& flight, DateType date)
    {
        for (PTR k = flightList; k != NULL; k = k->next)
        {
            if (strcmp(k->info.idFlight,flight->info.idFlight) != 0)
            {
                for (int i = 0; i < flight->info.totalTicket; i++)
                {
                    if (strcmp(flight->info.ticketList[i], "0") != 0 && checkDupIDOnFlight(flight->info.ticketList[i]) != -1 &&
                        !k->info.date.in12Hour(date) && (k->info.status == 1 || k->info.status == 2))
                    {
                        return k;
                    }
                }
            }
        }
        return NULL;
    }


    //Thay doi danh sach ve khi so cho ngoi cua may bay duoc thay doi
    void adjustTicketList(int totalTicket, char id[MAX_ID_PLANE + 1])
    {
        char s[MAX_ID_PASS + 1] = "0";

        for (PTR k = flightList; k != NULL; k = k->next) {
            if (strcmp(k->info.idPlane, id) == 0) {

                for (int i = k->info.totalTicket; i < totalTicket; i++) {
                    k->info.ticketList[i] = new char[MAX_ID_PASS + 1];
                    strncpy(k->info.ticketList[i], s, sizeof(k->info.ticketList[i]) - 1);
                    k->info.ticketList[i][sizeof(k->info.ticketList[i]) - 1] = '\0'; // Đảm bảo chuỗi có dấu kết thúc

                }
                k->info.totalTicket = totalTicket;
            }
        }
    }

    void bookTicket(int index, char id[MAX_ID_PASS + 1]) {
        strncpy(flightList->info.ticketList[index], id, MAX_ID_PASS);
        flightList->info.ticketList[index][MAX_ID_PASS] = '\0'; // Đảm bảo chuỗi có dấu kết thúc

    }



    void writeFileFlight() {
        ofstream out("../../FlightsData.txt");

        if (!out.is_open())
        {
            printf("Cant open file Flight Data\n");
            return;
        }
        out << "IDF IDP DATE ARRIVE STATUS TOTAL\n";

        for (PTR k = flightList; k != NULL; k = k->next)
        {
            out << k->info.idFlight << " " << k->info.idPlane << " " << k->info.date.strDate << " " << k->info.arrive << " " << k->info.status << " " << k->info.totalTicket << endl;
            for (auto it : k->info.passengers)
            {
                out <<  it.first << " " << it.second << endl;
            }
        }
        out << "#";
        out.close();
    }

    void readFile(string path)
    {
        std::ifstream inp(path);
        if (inp.is_open())
        {
            printf("Opened file Flight Data\n");
        }
        else
        {
            printf("Cannot open file Flight Data\n");
            return;
        }

        flightList = nullptr;
        string idFlight, idPlane, date, arrive, skip, idPass;
        int status, totalTicket, pos;

        inp >> skip >> skip >> skip >> skip >> skip >> skip;
        inp >> idFlight;
        vector<string> passengersList;

        while (idFlight != "#")
        {
            inp >> idPlane >> date >> arrive >> status >> totalTicket;
            Flight flight = Flight(idFlight, idPlane, date, arrive, (Status)status, totalTicket);
            //flight.passengersList[0] = "";
            for (int i = 1; i <= totalTicket; i++)
            {
                inp >> idPass >> pos;
                flight.passengers.insert(make_pair(idPass, pos));
            }
            insertAfter(flight);
            inp >> idFlight;
        }

        inp.close();
    }

    void deleteFlightList(PTR& first)
    {
        PTR temp = first;
        while (temp != NULL) {
            PTR t = temp;
            temp = temp->next;
            for (int i = 0; i < t->info.totalTicket; i++) {
                delete[]  t->info.ticketList[i];
            }
            delete[] t->info.ticketList;
            delete t;
        }
    }

    void deleteFlightByID(string id)
    {
        PTR temp = flightList;
        PTR pre = NULL;

        if (flightList->info.idFlight == id)
        {
            flightList = flightList->next;
            temp->next = nullptr;
            delete temp;
            return;
        }

        while (temp != NULL)
        {
            if (temp->info.idFlight == id)
            {
                pre->next = temp->next;
                delete temp;
                return;
            }
            pre = temp;
            temp = temp->next;
        }
    }
};


#endif
