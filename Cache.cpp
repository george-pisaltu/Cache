#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

int hash(int key, char lvl) // functie de hash, lvl determina nivelul {L1,L2}
{
    int nr[31] = { 0 }, nou = 0, den = 1, limit;
    int i, aux, max = 0;
    aux = key;
    if (lvl == 1)
        limit = 18;
    else
        limit = 16;
    i = 30;
    while (key != 0) {
        nr[i] = key % 2;
        i--;
        key = key / 2;
    }
    for (i = 28; i >= limit; i--) {
        nou = nou + nr[i] * den;
        den = den * 2;
    }
    return nou;
}

template <typename Tkey, typename Tvalue> // structura pentru way
struct way {
public:
    Tkey key;
    Tvalue value;
    bool dirty;
};

template <typename Tkey, typename Tvalue> // structura pentru set
struct set {
public:
    struct way<Tkey, Tvalue> way[2];
    int old;
};

template <typename Tkey, typename Tvalue>
class Hashtable {
private:
    struct set<Tkey, Tvalue>* h;
    int SETMAX;
    int lvl; // nivelul {L1, L2}

public:
    Hashtable(int hmax, char level) // constructor
    {
        struct way<Tkey, Tvalue> aux2;
        struct set<Tkey, Tvalue> aux;
        int i;
        aux2.dirty = false;
        aux2.value = 0;
        aux2.key = 0;
        aux.way[0] = aux2;
        aux.way[1] = aux2;
        aux.old = 0;
        this->SETMAX = hmax;
        h = new struct set<Tkey, Tvalue>[hmax];
        for (i = 0; i < hmax; i++)
            h[i] = aux;
        this->lvl = level;
    }
    ~Hashtable() // destructor
    {
        delete[] h;
    }

    int getold(Tkey key) // returneaza nr way-ului mai vechi
    {
        return h[hash(key, lvl)].old;
    }

    void put(Tkey key, Tvalue value, int way) // adauga la o anumita locatie in hashtable
    {
        if (h[hash(key, lvl)].way[0].key == key) {
            h[hash(key, lvl)].way[0].value = value;
            h[hash(key, lvl)].old = 1;
        }
        else if (h[hash(key, lvl)].way[1].key == key) {
            h[hash(key, lvl)].way[1].value = value;
            h[hash(key, lvl)].old = 0;
        }
        else {
            if (h[hash(key, lvl)].way[0].value == 0) {
                h[hash(key, lvl)].way[0].key = key;
                h[hash(key, lvl)].way[0].value = value;
                h[hash(key, lvl)].old = 1;
            }
            else if (h[hash(key, lvl)].way[1].value == 0) {
                h[hash(key, lvl)].way[1].key = key;
                h[hash(key, lvl)].way[1].value = value;
                h[hash(key, lvl)].old = 0;
            }
            h[hash(key, lvl)].way[way].key = key;
            h[hash(key, lvl)].way[way].value = value;
            h[hash(key, lvl)].old = (way + 1) % 2;
        }
    }

    Tvalue get(Tkey key) // returneaza valoarea corespunzatoare cheii key
    {
        if (h[hash(key, lvl)].way[0].key == key) {
            return h[hash(key, lvl)].way[0].value;
        }
        else if (h[hash(key, lvl)].way[1].key == key) {
            return h[hash(key, lvl)].way[1].value;
        }
        else
            return 0;
    }
    bool has_key(Tkey key) // returneaza true daca exista cheia in hashtable
    {
        if (h[hash(key, lvl)].way[0].key == key || h[hash(key, lvl)].way[1].key == key)
            return true;
        else
            return false;
    }

    void print() // printeaza cache-ul in fisier
    {
        int i;
        ofstream cache;
        cache.open("cache.out", ios::app); // se deschide cache.out in modul append
        for (i = 0; i < SETMAX; i++) {
            if (h[i].way[0].value != 0) {
                cache << i << " 0 " << h[i].way[0].key << " " << h[i].way[0].value;
                if (h[i].way[0].dirty == true)
                    cache << " *";
                cache << endl;
            }
            if (h[i].way[1].value != 0) {
                cache << i << " 1 " << h[i].way[1].key << " " << h[i].way[1].value;
                if (h[i].way[1].dirty == true)
                    cache << " *";
                cache << endl;
            }
        }
        if (lvl == 1)
            cache << endl;
        cache.close();
    }

    void setdirty(Tkey key) // activeaza dirty bitul
    {
        if (h[hash(key, lvl)].way[0].key == key) {
            h[hash(key, lvl)].way[0].dirty = true;
        }
        else if (h[hash(key, lvl)].way[1].key == key) {
            h[hash(key, lvl)].way[1].dirty = true;
        }
    }

    void setclean(Tkey key) // sterge dirty bitul
    {
        if (h[hash(key, lvl)].way[0].key == key) {
            h[hash(key, lvl)].way[0].dirty = false;
        }
        else if (h[hash(key, lvl)].way[1].key == key) {
            h[hash(key, lvl)].way[1].dirty = false;
        }
    }

    bool getdirty(Tkey key) // returneaza dirty bitul
    {
        if (h[hash(key, lvl)].way[0].key == key) {
            return h[hash(key, lvl)].way[0].dirty;
        }
        else if (h[hash(key, lvl)].way[1].key == key) {
            return h[hash(key, lvl)].way[1].dirty;
        }
        return false;
    }

    int getlvl() //returneaza nivelul cache-ului
    {
        return lvl;
    }
};

int main()
{
    class Hashtable<int, int> l11(2048, 1), l12(2048, 1), l2(8192, 2); // cele 3 cache-uri simulate
    ofstream ramout, aux; // fisierele necesare
    ifstream ramin, op;
    int i, j, addr, newdata, aram[100001], vram[100001], coren, rammax;
    char operation;
    ramout.open("ram.out");
    ramin.open("ram.in");
    op.open("operations.in");
    aux.open("cache.out"); // se goleste fisierul cache.out inainte de utilizare
    aux.close();
    i = 0;
    while (true) {
        ramin >> aram[i] >> vram[i]; // se citeste memoria ram
        i++;
        if (ramin.eof())
            break;
    }
    rammax = i;
    while (true) {
        op >> coren >> operation >> addr; // se salveaza in coren nr nucleului
        if (operation == 'w') // in operation se salveaza tipul operatiei
            op >> newdata; // in addr se salveaza adresa la care se face operatia
        if (operation == 'r') // daca operatia e w se salveaza in newdata datele noi
        {
            if (coren == 0) // pentru ambele nuclee
            {
                if (l11.getdirty(addr) == true || l11.get(addr) == 0) {
                    if (l2.get(addr) != 0) {
                        l11.put(addr, l2.get(addr), l11.getold(addr));
                    }
                    else {
                        i = 0;
                        while (addr != aram[i]) {
                            i++;
                        }

                        l2.put(addr, vram[i], l2.getold(addr));
                        l11.put(addr, vram[i], l11.getold(addr));
                    }
                    l11.setclean(addr);
                }
            }

            else {
                if (l12.getdirty(addr) == true || l12.get(addr) == 0) {
                    if (l2.get(addr) != 0) {
                        l12.put(addr, l2.get(addr), l12.getold(addr));
                    }
                    else {
                        i = 0;
                        while (addr != aram[i]) {
                            i++;
                        }
                        l2.put(addr, vram[i], l2.getold(addr));
                        l12.put(addr, vram[i], l12.getold(addr));
                    }
                    l12.setclean(addr);
                }
            }
        }
        else {
            if (coren == 0) // pentru ambele nuclee
            {
                if (l11.get(addr) != 0 && l11.getdirty(addr) == false) {
                    l11.put(addr, newdata, l11.getold(addr));
                    l2.put(addr, newdata, l2.getold(addr));
                    l12.setdirty(addr);
                }
                else {
                    if (l2.get(addr) != 0) {
                        l11.put(addr, newdata, l11.getold(addr));
                        l2.put(addr, newdata, l2.getold(addr));
                        l12.setdirty(addr);
                    }
                    else {
                        i = 0;
                        while (addr == aram[i]) {
                            i++;
                        }
                        l11.put(addr, newdata, l11.getold(addr));
                        l2.put(addr, newdata, l2.getold(addr));
                    }
                }
            }
            else {
                if (l12.get(addr) != 0 && l12.getdirty(addr) == false) {
                    l12.put(addr, newdata, l12.getold(addr));
                    l2.put(addr, newdata, l2.getold(addr));
                    l11.setdirty(addr);
                }
                else {
                    if (l2.get(addr) != 0) {
                        l12.put(addr, newdata, l12.getold(addr));
                        l2.put(addr, newdata, l2.getold(addr));
                        l11.setdirty(addr);
                    }
                    else {
                        i = 0;
                        while (addr == aram[i]) {
                            i++;
                        }
                        l12.put(addr, newdata, l12.getold(addr));
                        l2.put(addr, newdata, l2.getold(addr));
                    }
                }
            }
        }
        if (op.eof()) // daca se termina operatiile se iese din while
            break;
    }
    i = 0;
    for (i = 0; i < rammax - 1; i++) {

        ramout << setw(10) << setfill('0') << aram[i]; // se scrie in ram cu zerouri in fata
        ramout << " ";
        ramout << setw(10) << setfill('0') << vram[i];
        ramout << endl;
    }

    l11.print();
    cout << endl;
    l12.print();
    cout << endl;
    l2.print();

    op.close();
    ramin.close();
    ramout.close();
    return 0;
}