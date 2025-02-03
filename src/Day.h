#ifndef DAY_H
#define DAY_H

#pragma once

#include<ctime>
#include<string>
#include <cstring>

using namespace std;
const int DAY_MONTH[13] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };

struct DateType
{
public:
    int year, month, day, hour, minute;
    string strDate;
    DateType(){};
    DateType(string date)
    {
        strDate = date;
        if (sscanf(date.c_str(), "%d:%d:%d:%d:%d", &year, &month, &day, &hour, &minute) != 5)
        {
            printf("Error parsing date and time format!!\n");
        }
    }

    DateType(int y, int m, int d, int h, int mi)
        : year(y), month(m), day(d), hour(h), minute(mi)
    {
        strDate = to_string(year) + ":" + std::to_string(month) + ":" + std::to_string(day) + ":" + std::to_string(hour) + ":" + std::to_string(minute);
    };

    DateType getCurTime()
    {
        tm newTime;//(định dạng y m d h m)
        time_t now = time(0);//biến thừoi gian hiện tại thành epoch(1/1/1970)

        localtime_r(&now, &newTime);


        DateType date;
        date.year = newTime.tm_year;
        date.month = newTime.tm_mon;
        date.day = newTime.tm_mday;
        date.hour = newTime.tm_hour;
        date.minute = newTime.tm_min;

        return date;
    }

    string formatStr()
    {
        return to_string(year) + "-" + std::to_string(month) + "-" + std::to_string(day) + " " + std::to_string(hour) + ":" + std::to_string(minute);
    }

    bool isLeapYear(int year) {
        if (year % 400 == 0)
            return true;
        if (year % 100 != 0 && year % 4 == 0)
            return true;
        return false;
    }



    bool checkTime() {
        switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
        {
            if (day > 31)
                return false;
            return true;
        }
        case 4: case 6: case 9: case 11: {
            if (day > 30)
                return false;
            return true;
        }
        case 2: {
            if (isLeapYear(year)) {
                if (day > 29)
                    return false;
                return true;
            }
            else {
                if (day > 28)
                    return false;
                return true;
            }
        }
        }
        return true;
    }

    bool checkDay()
    {
        return (day <= 31 && day >= 1);
    }

    bool checkMonth()
    {
        return (month <= 12 && month >= 1);

    }

    bool checkYear()
    {
        return (year >= 2024 && year <= 2026);
    }

    bool checkHour()
    {
        return (hour <= 23 && hour >= 0);
    }

    bool checkMinute()
    {
        return (minute <= 59 && minute >= 0);

    }

    int calDateToday(DateType date) // trả về số ngày dùng để so sánh và tínhn khaongr cáchtgian
    {
        if (date.month < 3)
        {
            date.year--;
            date.month += 12;
        }
        return 365*date.year + date.year/4 - date.year/100 + date.year/400 + (153*date.month - 457)/5 + date.day - 306;
    }

    //Tra ve so phut giua hai moc thoi gian
    long long calSpaceTime(DateType& _date)
    {
        int deltaDay = calDateToday(DateType(year,month,day,hour, minute)) - calDateToday(_date);
        return deltaDay * 24 * 60;
    }

    // kiem tra thoi gian hien tai voi moc thgian nhap vao cach bao nhieu phut
    bool checkTimeBeforeMinute(int min)
    {
        DateType now = getCurTime();
        if (calSpaceTime(now) >= min)
            return true;

        return false;
    }

    //trả về true false dựa trên mốc tgian,đẩm bảo hợp lệ

    bool in12Hour(DateType& d2) {

        if (abs(calSpaceTime(d2)) >= 720)
            return true;
        return false;


    }
};


#endif // DAY_H
