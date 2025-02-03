#pragma once

#include<fstream>
#include<iostream>
#include<string>
#include <cstring>

#ifndef PASS_H
#define PASS_H

#define MAX_LAST_NAME 10
#define MAX_FIRST_NAME 50
#define MAX_ID_PASS 12

using namespace std;

struct Passenger
{
    char lastName[MAX_LAST_NAME + 1];
    char firstName[MAX_FIRST_NAME + 1];
    char idPass[MAX_ID_PASS + 1];
    /*
    * 0 NAM
    * 1 NU
    */
    int gender;

    Passenger()
    {
        // lastName[0] = '\0';
        // firstName[0] = '\0';
        // idPass[0] = '\0';
        // idPlane[0] = '\0';
        // int gender = 0;
    };
    Passenger(string cc, string ho, string ten, int g)
    {
        strcpy(idPass, cc.c_str());
        strcpy(firstName, ho.c_str());
        strcpy(lastName, ten.c_str());
        gender = g;
    };
};

struct NodePass
{
    Passenger data;
    NodePass* pleft;
    NodePass* pright;
    int height;
};

typedef struct NodePass* AVLTree;

class PassengerManager
{
public:
    AVLTree passengerList;

    PassengerManager(){};


    AVLTree createTree(Passenger& data)
    {
        AVLTree newPass = new NodePass;
        newPass->data = data;
        newPass->pleft = NULL;
        newPass->pright = NULL;
        newPass->height = 1;
        return newPass;

    }

    AVLTree getLastLeftNode()
    {
        auto temp = passengerList;
        while (temp->pleft != nullptr)
        {
            temp = temp->pleft;
        }
        return temp;
    }

    int getSize() {
        int cnt = 0;

        if (passengerList == NULL)
            return cnt;

        for (auto k = passengerList; k != NULL; k = k->pright) {
            cnt++;
        }
        for (auto k = passengerList; k != NULL; k = k->pleft) {
            cnt++;
        }
        return cnt;
    }

    bool checkIDPassenger(char s[MAX_ID_PASS])
    {
        if (strlen(s) == 12)
            return true;
        return false;
    }



    int findHeight(AVLTree& root)
    {
        if (root == NULL)
            return 0;
        return root->height;

    }

    // XOAY PHAI
    AVLTree rotateRight(AVLTree& root) {
        AVLTree x = root->pleft;
        AVLTree y = x->pright;

        x->pright = root;
        root->pleft = y;

        root->height = std::max(findHeight(root->pleft), findHeight(root->pright)) + 1;
        x->height = std::max(findHeight(x->pleft), findHeight(x->pright)) + 1;

        return x;
    }

    // XOAY TRAI
    AVLTree rotateLeft(AVLTree& root) {
        AVLTree x = root->pright;
        AVLTree y = x->pleft;

        x->pleft = root;
        root->pright = y;

        root->height = std::max(findHeight(root->pleft), findHeight(root->pright)) + 1;
        x->height = std::max(findHeight(x->pleft), findHeight(x->pright)) + 1;

        return x;

    }



    int getBalanceFactor(AVLTree& root) {
        if (root == NULL)
            return 0;
        return findHeight(root->pleft) - findHeight(root->pright);

    }

    void removeAll(AVLTree& t) {
        if (t != NULL) {
            removeAll(t->pleft);
            removeAll(t->pright);
            delete t;
        }
    }

    void deletePassengerByID(string id)
    {
        //tao node p lam node the mang
        AVLTree root = findPassenger(passengerList, id.c_str());
        if (!root)
        {
            return;
        }

        AVLTree p = root;

        //neu cay con trai la rong
        if(root->pleft == NULL)
        {
            //gan lai node goc theo nhanh phai
            root = root->pright;
        } else if(root->pright == NULL)
        {
            //neu cay con phai la rong
            //gan lai node goc theo nhanh trai
            root = root->pleft;
        } else
        { // nguoc lai, neu cay co du 2 con
            //su dung node the mang
            FindReplNode(p, root->pright);
        }
        //xoa di node p
        delete p;
    }

    void FindReplNode(AVLTree p, AVLTree q)
    {
        if(q->pleft)
        {
            FindReplNode(p, q->pleft);
        } else
        {
            p->data = q->data;
            p = q;
            q = q->pright;
        }
    }

    AVLTree findPassenger(AVLTree& root, const char foundID[MAX_ID_PASS + 1]) {
        if (root != NULL) {
            if (strcmp(root->data.idPass, foundID) == 0)
                return root;

            else if (strcmp(foundID, root->data.idPass) > 0)
                return findPassenger(root->pright, foundID);

            else if (strcmp(foundID, root->data.idPass) < 0)
                return findPassenger(root->pleft, foundID);
        }
        return NULL;
    }

    void adjustPassenger(Passenger& p, AVLTree& temp) {
        temp->data = p;
    }



    AVLTree addPassenger(AVLTree& root, Passenger& data) {
        // Tim vi tri thich hop de them nut
        if (root == NULL)
        {
            return createTree(data);
        }


        if (strcmp(data.idPass, root->data.idPass) < 0)
            root->pleft = addPassenger(root->pleft, data);
        else if (strcmp(data.idPass, root->data.idPass) > 0)
            root->pright = addPassenger(root->pright, data);
        else
            return root;

        // Cap nhat lai chieu cao cho cay
        root->height = std::max(findHeight(root->pleft), findHeight(root->pright)) + 1;

        // Cap nhat lai he so can bang
        int balance = getBalanceFactor(root);

        // Neu cay mat can bang -> Co 4 truong hop
        // 1. Trai - Trai
        if (balance > 1 && strcmp(data.idPass, root->pleft->data.idPass) < 0)
            return rotateRight(root);

        //2. Phai - Phai
        if (balance < -1 && strcmp(data.idPass, root->pright->data.idPass) > 0)
            return rotateLeft(root);

        //3. Trai - Phai
        if (balance > 1 && strcmp(data.idPass, root->pleft->data.idPass) > 0) {
            root->pleft = rotateLeft(root->pleft);
            return rotateRight(root);
        }

        //4. Phai - Trai
        if (balance < -1 && strcmp(data.idPass, root->pright->data.idPass) < 0) {
            root->pright = rotateRight(root->pright);
            return rotateLeft(root);
        }

        return root;
    }



    void savePassengerData(AVLTree& passengerList, std::ofstream& out) {
        if (passengerList != NULL)
        {
            out << passengerList->data.idPass << " " << passengerList->data.firstName << " " << passengerList->data.lastName << " " << passengerList->data.gender << endl;
            savePassengerData(passengerList->pleft, out);
            savePassengerData(passengerList->pright, out);
        }

    }

    void writeFilePassenger(AVLTree& passengerList)
    {
        std::ofstream out;
        out.open("../../PassengersData.txt");

        if (!out.is_open())
        {
            printf("Cannot open file passenger data\n");
            return;
        }
        out << "CCCD HO TEN GIOI_TINH\n";
        savePassengerData(passengerList, out);
        out << "#";
        out.close();
    }


    void readFilePassenger(string path)
    {
        std::ifstream inp(path);
        if (inp.is_open())
        {
            printf("Opened file Passenger Data\n");
        }
        else
        {
            printf("Cannot open file Passenger Data\n");
            return;
        }

        passengerList = nullptr;
        string cccd, ho, ten, skip;
        int gender;

        inp >> skip >> skip >> skip >> skip;
        inp >> cccd;
        while (cccd != "#")
        {
            inp >> ho >> ten >> gender;
            Passenger passenger = Passenger(cccd, ho, ten, gender);
            passengerList = addPassenger(passengerList, passenger);
            inp >> cccd;
        }
        inp.close();
    }

};

#endif
