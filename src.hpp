#include <iostream>
#include <algorithm>
#include "base.hpp"
using namespace std;

struct date {
    int year, month, day;
    date() = default;
    date(int y, int m, int d) : year(y), month(m), day(d) {}
};

// stream input for date: "y m d"
inline istream& operator>>(istream& is, date& dt){
    return (is >> dt.year >> dt.month >> dt.day);
}

// compare: earlier means smaller; assume 360-day year, 30-day month
inline bool operator<(const date& a, const date& b){
    int da = a.year * 360 + (a.month - 1) * 30 + (a.day - 1);
    int db = b.year * 360 + (b.month - 1) * 30 + (b.day - 1);
    return da < db;
}

class mail : public object {
protected:
    string postmark;
    date send_date;
    date arrive_date;
public:
    mail() = default;
    mail(string _contain_, string _postmark_, date send_d, date arrive_d)
        : object(_contain_), postmark(_postmark_), send_date(send_d), arrive_date(arrive_d) {}
    // overrides
    void copy(object* o) override {
        contain      = reinterpret_cast<mail*>(o)->contain;
        postmark     = reinterpret_cast<mail*>(o)->postmark;
        send_date    = reinterpret_cast<mail*>(o)->send_date;
        arrive_date  = reinterpret_cast<mail*>(o)->arrive_date;
    }
    string send_status(int, int, int) override { return string("not send"); }
    string type() override { return string("no type"); }
    void print() override {
        object::print();
        cout << "[mail] postmark: " << postmark << endl;
    }
    virtual ~mail() {}
};

class air_mail : public mail {
protected:
    string airlines;
    date take_off_date;
    date land_date;
public:
    air_mail() = default;
    air_mail(string _contain_, string _postmark_, date send_d, date arrive_d,
             date take_off, date land, string _airline)
        : mail(_contain_, _postmark_, send_d, arrive_d), airlines(_airline),
          take_off_date(take_off), land_date(land) {}
    void copy(object* o) override {
        mail::copy(o);
        airlines = reinterpret_cast<air_mail*>(o)->airlines;
        take_off_date = reinterpret_cast<air_mail*>(o)->take_off_date;
        land_date = reinterpret_cast<air_mail*>(o)->land_date;
    }
    string send_status(int y, int m, int d) override {
        date ask(y,m,d);
        if (ask < send_date) return "mail not send";
        else if (ask < take_off_date) return "wait in airport";
        else if (ask < land_date) return "in flight";
        else if (ask < arrive_date) return "already land";
        else return "already arrive";
    }
    string type() override { return string("air"); }
    void print() override {
        mail::print();
        cout << "[air] airlines: " << airlines << endl;
    }
    ~air_mail() override {}
};

class train_mail : public mail {
protected:
    string* station_name = nullptr;
    date* station_time = nullptr;
    int len = 0;
public:
    train_mail() = default;
    train_mail(string _contain_, string _postmark_, date send_d, date arrive_d,
               string* sname, date* stime, int station_num)
        : mail(_contain_, _postmark_, send_d, arrive_d), len(station_num) {
        if (len > 0) {
            station_name = new string[len];
            station_time = new date[len];
            for (int i=0;i<len;i++){ station_name[i]=sname[i]; station_time[i]=stime[i]; }
        }
    }
    void copy(object* o) override {
        mail::copy(o);
        train_mail* t = reinterpret_cast<train_mail*>(o);
        // free old
        delete [] station_name; station_name=nullptr;
        delete [] station_time; station_time=nullptr;
        len = t->len;
        if (len>0){
            station_name = new string[len];
            station_time = new date[len];
            for(int i=0;i<len;i++){ station_name[i]=t->station_name[i]; station_time[i]=t->station_time[i]; }
        }
    }
    string send_status(int y, int m, int d) override {
        date q(y,m,d);
        if (q < send_date) return "mail not send";
        if (len==0){
            if (q < arrive_date) return "in train"; // no stations, just traveling
            else return "already arrive";
        }
        if (q < station_time[0]) return "wait in station";
        for (int i=0;i<len-1;i++){
            if (q < station_time[i+1]) return string("on the way to ")+station_name[i+1];
        }
        if (q < arrive_date) return "wait in destination";
        return "already arrive";
    }
    string type() override { return string("train"); }
    void print() override {
        mail::print();
        cout << "[train] station_num: " << len << endl;
    }
    ~train_mail() override {
        delete [] station_name;
        delete [] station_time;
    }
};

class car_mail : public mail {
protected:
    int total_mile = 0;
    string driver;
public:
    car_mail() = default;
    car_mail(string _contain_, string _postmark_, date send_d, date arrive_d,
             int mile, string _driver)
        : mail(_contain_, _postmark_, send_d, arrive_d), total_mile(mile), driver(_driver) {}
    void copy(object* o) override {
        mail::copy(o);
        total_mile = reinterpret_cast<car_mail*>(o)->total_mile;
        driver = reinterpret_cast<car_mail*>(o)->driver;
    }
    string send_status(int y, int m, int d) override {
        date q(y,m,d);
        if (q < send_date) return "mail not send";
        if (q < arrive_date){
            int qs = q.year*360 + (q.month-1)*30 + (q.day-1);
            int ss = send_date.year*360 + (send_date.month-1)*30 + (send_date.day-1);
            int as = arrive_date.year*360 + (arrive_date.month-1)*30 + (arrive_date.day-1);
            double used = (double)(qs - ss);
            double total = (double)(as - ss);
            double current_mile = total>0 ? (used/total) * (double)total_mile : 0.0;
            return to_string(current_mile);
        }
        return "already arrive";
    }
    string type() override { return string("car"); }
    void print() override {
        mail::print();
        cout << "[car] driver_name: " << driver << endl;
    }
    ~car_mail() override {}
};

inline void obj_swap(object *&lhs, object *&rhs){
    object* tmp = lhs; lhs = rhs; rhs = tmp;
}
