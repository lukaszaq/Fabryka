#pragma once
#include <iostream>
#include <string>
#include <sstream>

using namespace std;
enum smak
{
	truskawka,
	jagoda,
	gowno
};
inline const char* EnumToString(smak v)
{
	switch (v)
	{
		case truskawka:   return "Truskawkowy";
		case jagoda:   return "Jagodowy";
		case gowno: return "dotnetowy";
	}
}
class Koncentrat
{
public:
	unsigned int ilosc;
	smak smakKoncentratu;
	Koncentrat() {};
	Koncentrat(smak smakKoncentratu, unsigned int ilosc) : smakKoncentratu(smakKoncentratu), ilosc(ilosc) {};
	void dodajSkladnik(unsigned int ilosc)
	{
		this->ilosc += ilosc;
	}

	int operator+(const int b) const
	{
		return ilosc + b;
	}

	string toString() 
	{ 
		std::ostringstream oss;
		oss << " Smak: " << EnumToString(smakKoncentratu) << " Ilosc: " << ilosc;
		return oss.str();
	}
};

class LiniaProdukcyjna
{
protected:
	bool stan;
	int towar;
	double wspolczynnikProdukcji = 1;
	string nazwa;
public:

	LiniaProdukcyjna(string nazwa): nazwa(nazwa)
	{
		stan = false;
		towar = 0;
	}

	void dodajTowar(unsigned int iloscProdukcji) 
	{
		towar += iloscProdukcji;
	}

	virtual void sprzedaj(unsigned int iloscSprzedarzy)
	{
		towar -= iloscSprzedarzy;
	}

	virtual void wypisz(ostream& out)
	{
		out << " Nazwa: " << nazwa << (stan ? " Wlaczona " : " Wylaczona ") << "Na stanie: " << towar << " Sztuk ";
	}

	void zmienStan() 
	{
		stan = !stan;
	}

};

ostream& operator<< (ostream& out, LiniaProdukcyjna& lp) {
	lp.wypisz(out);
	return out;
}

class LiniaJogurt : public LiniaProdukcyjna
{
private:
	int mleko;
public:
	LiniaJogurt(string nazwa) : LiniaProdukcyjna(nazwa){}
	LiniaJogurt(string nazwa, int ilosc) : LiniaProdukcyjna(nazwa)
	{
		this->mleko = ilosc;
	}

	void dodajSkladnik(unsigned int ilosc) 
	{
		this->mleko += ilosc;
	}

	void wypisz(ostream& out) override
	{
		LiniaProdukcyjna::wypisz(out);
		out << "Mleko: " << mleko << endl;
	}

	void wyprodukuj(unsigned int iloscProdukcji)
	{
		LiniaProdukcyjna::dodajTowar(iloscProdukcji);
		mleko -= iloscProdukcji * wspolczynnikProdukcji;
	}

};


class LiniaNapoje : public LiniaProdukcyjna
{
private:
	int woda;
	Koncentrat* koncetraty;
public:
	LiniaNapoje(string nazwa): LiniaProdukcyjna(nazwa){}	
	
	LiniaNapoje(string nazwa, smak smak, unsigned int ilosc, unsigned int woda ): LiniaProdukcyjna(nazwa)
	{
		this->woda = woda;
		this->dodajKoncentrat(smak, ilosc);
	}

	void dodajWode(unsigned int ilosc)
	{
		woda += ilosc;
	}

	void dodajKoncentrat(smak smak, unsigned int ilosc) 
	{
		int rozmiar = sizeof(koncetraty)/sizeof(koncetraty[0])-1;
		Koncentrat *temp = koncetraty;
		delete[] koncetraty;
		koncetraty = new Koncentrat[rozmiar + 1];
		for (int i = 0; i < rozmiar; i++)
		{
			koncetraty[i] = temp[i];
		}
		koncetraty[rozmiar] = Koncentrat(smak, ilosc);
		delete[] temp;
	}

	void wypisz(ostream& out) override
	{
		LiniaProdukcyjna::wypisz(out);
		out << "Woda: " << woda;
		for (int i = 0; i < sizeof(koncetraty) / sizeof(koncetraty[0]); i++)
		{
			out << koncetraty[i].toString();
		}
		out << endl << endl;
	}


	void wyprodukuj(unsigned int iloscProdukcji, smak smak)
	{
		LiniaProdukcyjna::dodajTowar(iloscProdukcji);
		woda -= iloscProdukcji * wspolczynnikProdukcji;
		for (int i = 0; i < sizeof(koncetraty) / sizeof(koncetraty[0]); i++)
		{
			if (koncetraty[i].smakKoncentratu == smak) 
			{
				koncetraty[i].ilosc -= iloscProdukcji * wspolczynnikProdukcji;
			}
		}
	}
};

int main() 
{
	LiniaProdukcyjna** linia = new LiniaProdukcyjna*[3];

	const LiniaNapoje truskawkowyNapoj = LiniaNapoje("truskawkowyNapoj", truskawka, 15, 15);
	linia[0] = new LiniaNapoje(truskawkowyNapoj);
	linia[1] = new LiniaJogurt("Jogurt Naturalny", 10);
	linia[2] = new LiniaNapoje("dotnetowyNapuj", gowno, 10, 150);

	for (int i = 0; i < 3; i++)
	{
		cout << *linia[i];
		linia[i]->zmienStan();
	}

	static_cast<LiniaNapoje&>(*linia[0]).wyprodukuj(10, truskawka);
	static_cast<LiniaJogurt&>(*linia[1]).wyprodukuj(10);
	static_cast<LiniaNapoje&>(*linia[2]).wyprodukuj(10, gowno);

	for (int i = 0; i < 3; i++)
	{
		cout << *linia[i];
	}

//	Nazwa: truskawkowyNapoj Wylaczona Na stanie : 0 Sztuk Woda : 0 Smak : Truskawkowy Ilosc : 15
//	Nazwa : Jogurt Naturalny Wylaczona Na stanie : 0 Sztuk Mleko : 0
//	Nazwa : dotnetowyNapuj Wylaczona Na stanie : 0 Sztuk Woda : 0 Smak : dotnetowy Ilosc : 10
// 
//	Nazwa : truskawkowyNapoj Wlaczona Na stanie : 10 Sztuk Woda : -10 Smak : Truskawkowy Ilosc : 5
//	Nazwa : Jogurt Naturalny Wlaczona Na stanie : 10 Sztuk Mleko : -10
//	Nazwa : dotnetowyNapuj Wlaczona Na stanie : 10 Sztuk Woda : -10 Smak : dotnetowy Ilosc : 0
}
