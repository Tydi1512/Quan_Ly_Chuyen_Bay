#pragma once

#ifndef PLANE_H
#define PLANE_H

#include<fstream>
#include<string.h>
#include <sstream>


#define MAX_ID_PLANE 15
#define MAX_PLANE 300
#define MAX_TYPE_PLANE 40



using namespace std;

struct Plane
{
    char idPlane[MAX_ID_PLANE + 1];
    char type[MAX_TYPE_PLANE + 1];
    int seats;
    int row, column;

    //--- CHI SO PHU

    //So luot thuc hien chuyen bay
    int flyTimes = 0;
    bool isAvai = true;

    Plane(){};


    Plane(string id, string _type, int r, int c)
        : row(r), column(c), seats(r*c)
    {
        strcpy(idPlane, id.c_str());
        strcpy(type, _type.c_str());
        flyTimes = 0;
    }
};


struct PlanesList {
    int size = 0;
    Plane* data[MAX_PLANE];

    bool isEmpty() {
        return (size == 0 ? true : false);
    }

    bool isFull() {
        return (size >= MAX_PLANE ? true : false);
    }

    void clearFlyTimes()
    {
        for (int i = 0; i < size; i++)
        {
            data[i]->flyTimes = 0;
        }
    }



    void addPlane(Plane* plane)
    {
        if (size == MAX_PLANE)
            return;
        data[size] = plane;
        size++;
    }

    int findPlane(string idPlane) {
        for (int i = 0; i < size; i++) {
            if (strcmp(data[i]->idPlane, idPlane.c_str()) == 0) //hàm so sánh
                return i;
        }
        return -1;
    }

    Plane* findPlaneByID(string idPlane) {
        for (int i = 0; i < size; i++) {
            if (strcmp(data[i]->idPlane, idPlane.c_str()) == 0) //hàm so sánh
                return data[i];
        }
        return NULL;
    }

    void removePlane(int index) {
        delete data[index];
        for (int i = index; i < size - 1; i++) {
            data[i] = data[i + 1];
        }
        size--;
    }



    void swap(int& A, int& B) {
        int temp;
        temp = A;
        A = B;
        B = temp;
    }

    void bubbleSortPlane(int A[])
    {
        for (int i = 0; i < size - 1; i++)
            for (int j = i + 1; j < size; j++)
                if (data[A[i]]->flyTimes < data[A[j]]->flyTimes)
                {
                    swap(A[i], A[j]);
                }
    }

    void writeFilePlane() {
        fstream out;
        out.open("../../PlanesData.txt", ios::out);

        if (!out.is_open()) {
            printf("Cannot open file plane data\n");
            return;
        }

        out << "IDP TYPE ROW COLUMN\n";

        for (int i = 0; i < size; i++) {
            out << data[i]->idPlane << " " << data[i]->type << " " << data[i]->row << " " << data[i]->column << endl;
        }
        out << "#";
        out.close();
    }

    void readFilePlane(string path)
    {
        std::ifstream inp(path);
        if (!inp.is_open())
        {
            printf("Cannot open file plane data\n");
            return;
        }

        string id, type, skip;
        int r, c;
        inp >> skip >> skip >> skip >> skip;
        inp >> id;
        while (id != "#")
        {
            inp >> type >> r >> c;
            Plane* plane = new Plane(id, type, r, c);
            addPlane(plane);
            inp >> id;
        }
        inp.close();
    }


    void deletePlaneList() {
        for (int i = 0; i < size; i++)
            delete data[i];
    }
};


#endif

