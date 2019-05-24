#include<iostream>
#include<list>
#include<vector>
#include<utility>
#include<cassert>
using namespace std;

/////////////////////////////////  scris, compilat si rulat in VS 2019  /////////////////////////////////

template<typename K>
struct KeyHash
{
	unsigned long operator() (const K& key) const
	{
		return reinterpret_cast<unsigned long> (&key) % 111;
	}
};

template<class K, class V, class F = KeyHash<K>>
class HashMap
{
	static const size_t tableSize; //numarul de buckets
	vector<list<pair<K, V>>> table; //un vector de dimensiune tableSize ce contine liste de perechi <key, val>
	F HashFunction; //functia de dispersie

public:
	HashMap(); //constructor fara parametri ce initializeaza un HashMap gol
	HashMap(const HashMap<K, V, F>&); //constructor de copiere
	~HashMap(); //destructor

	void insert(const K&, const V&); //metoda de adaugare a unui nou element
	bool find(const K&, V&) const; //metoda de cautare a unui element
	void erase(const K&); //metoda de stergere a unui element
	vector<V> values(const K&) const; //metoda ce returneaza un vector cu valorile cunoscute pentru o anumita cheie
	unsigned numberOfDistinctKeys() const; //metoda pentru obtinerea numarului de chei distincte

	HashMap<K, V, F>& operator=(const HashMap<K, V, F>&); //supraincarcarea operatorului de atribuire
	V operator[](const K&) const; //supraincarcarea operatorului []
	template<class L, class B, class G> friend ostream& operator<<(ostream&, const HashMap<L, B, G>&); //supraincarcarea operatorului de afisare
};

template<class K, class V, class F>
const size_t HashMap<K, V, F>::tableSize = 111; //toate obiectele vor avea aceeasi dimensiune

template<class K, class V, class F>
HashMap<K, V, F>::HashMap()
{
	table.resize(tableSize);
}

template<class K, class V, class F>
HashMap<K, V, F>::HashMap(const HashMap<K, V, F>& hm)
	: table(hm.table)
{}

template<class K, class V, class F>
HashMap<K, V, F>::~HashMap()
{}

template<class K, class V, class F>
void HashMap<K, V, F>::insert(const K& key, const V& val)
{
	//daca "key" nu a mai fost introdusa pana acum, atunci va fi introdusa in HashMap
	//daca "key" a mai fost introdusa pana acum, atunci valoarea de la cheia respectiva va fi inlocuita cu "val"
	//acest HashMap retine valori cu chei diferite
	unsigned long index = HashFunction(key);
	for (auto k = table[index].begin(); k != table[index].end(); k++)
		if (k->first == key)
		{
			k->second = val;
			return;
		}
	table[index].push_back({ key,val });
}

template<class K, class V, class F>
bool HashMap<K,V,F>::find(const K& key, V& val) const
{
	//aceasta metoda returneaza true daca valoarea cu cheia "key" se afla in HashMap
	//daca metoda returneaza true (se gaseste) atunci parametrul "val" primeste valoarea de la cheia respectiva
	unsigned long index = HashFunction(key);
	for (auto k : table[index])
		if (k.first == key)
		{
			val = k.second;
			return true;
		}
	return false;
}

template<class K, class V, class F>
void HashMap<K, V, F>::erase(const K& key)
{
	//sterge elementul de la cheia "key" daca il gaseste in HashMap, daca nu, atunci nu are efect
	unsigned long index = HashFunction(key);
	for(auto k = table[index].begin(); k != table[index].end(); k++)
		if (k->first == key)
		{
			table[index].erase(k);
			return;
		}
}

template<class K, class V, class F>
vector<V> HashMap<K, V, F>::values(const K& key) const
{
	//de fapt, pentru "key" data ca parametru, functia returneaza un vector format din valorile ce se afla in bucket - ul indicat de functia de dispersie pentru cheia "key" (asta am inteles din cerinta)
	unsigned long index = HashFunction(key);
	vector<V> temp;
	for (auto x : table[index])
		temp.push_back(x.second);
	return temp;
}

template<class K, class V, class F>
unsigned HashMap<K, V, F>::numberOfDistinctKeys() const
{
	//returneaza, de fapt, numarul de buckets ocupate (asta am inteles ca trebuie sa faca metoda)
	unsigned nr = 0;
	for (size_t i = 0; i < tableSize; i++)
		if (table[i].size() > 0)
			nr++;
	return nr;
}

template<class K, class V, class F>
HashMap<K, V, F>& HashMap<K, V, F>::operator=(const HashMap<K, V, F>& hm)
{
	table = hm.table;
	return *this;
}

template<class K, class V, class F>
V HashMap<K, V, F>::operator[](const K& key) const
{
	//daca nu exista valoare la cheia indicata atunci functia arunca exceptie
	//daca exista, atunci returneaza acea valoare
	unsigned long index = HashFunction(key);
	if (table[index].size() == 0)
		throw out_of_range("Nu exista valoare la cheia indicata");
	return table[index].front().second;
}

template<class L, class B, class G>
ostream& operator<<(ostream& out, const HashMap<L, B, G>& hm)
{
	//afiseaza HashMap - ul
	for (size_t i = 0; i < hm.tableSize; i++)
		for (auto x : hm.table[i])
			out << x.first << " --> " << x.second << endl;
	return out;
}

int main()
{
	struct KeyHashInt
	{
		unsigned long operator() (const int& key) const
		{
			return key % 111;
		}
	};

	HashMap<int, string, KeyHashInt> hm;
	assert(hm.numberOfDistinctKeys() == 0);

	hm.insert(5, "Florin");
	assert(hm.numberOfDistinctKeys() == 1);
	assert(hm[5] == "Florin");

	hm.insert(5, "Florinel");
	assert(hm.numberOfDistinctKeys() == 1);
	assert(hm[5] == "Florinel");

	hm.insert(100, "Ionut");
	assert(hm.numberOfDistinctKeys() == 2);
	assert(hm[100] == "Ionut");

	hm.insert(116, "Denis");
	assert(hm.numberOfDistinctKeys() == 2);
	assert(hm[116] == "Florinel");

	string name;
	assert(hm.find(116, name) == true);
	assert(name == "Denis");

	HashMap<int, string, KeyHashInt> hm2(hm);
	assert(hm2.numberOfDistinctKeys() == 2);

	hm2.erase(116);
	assert(hm2.numberOfDistinctKeys() == 2);
	assert(hm2.find(116, name) == false);

	vector<string> names(hm.values(5));
	assert(names.size() == 2);
	assert(names[0] == "Florinel");
	assert(names[1] == "Denis");
}