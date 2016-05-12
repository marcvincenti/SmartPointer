#include "../include/pointeur.hpp"
#include <iostream>
#include <string.h>

#define POINTEUR(T) SmartPointer<T>

POINTEUR(int) pntg;

struct CC;

struct CC
{
    POINTEUR(CC) p;
    CC() : p(0) {}
};

void test()
{
    pntg = new int;
    POINTEUR(int) test = pntg;
}

struct Chien
{
    char grpSang = 'a';
    int nbDents;
    char* nom;
    SmartPointer<Chien> parent;
    int blble[500];
};

int main()
{
	{
		POINTEUR(POINTEUR(int)) ult = new POINTEUR(int);
		ult = new POINTEUR(int);
	}
	std::cout << "Programme de test" << std::endl;
	SmartPointer<int> p1;
	SmartPointer<int> ptrInt = new int;
	SmartPointer<int> ptrInt2 = new int;
	SmartPointer<int> p500(p1);
	SmartPointer<int> p501(p500);
	SmartPointer<int> p502(NULL);


	*ptrInt = 4;
	p1 = ptrInt;

	SmartPointer<int> p3(ptrInt2);
	SmartPointer<int> p4;
	*p1=0;
	p4 = p1;

{
	SmartPointer<int> dentition;
	SmartPointer<Chien> p5;
{
	SmartPointer<Chien> chihuahua = new Chien;
	p5 = chihuahua;
	chihuahua->parent = new Chien;
	chihuahua->nbDents = 42;
	dentition = chihuahua->nbDents;
	chihuahua->parent->parent = chihuahua;
}

	std::cout << "dentition : " << *dentition << std::endl;
	//delete &dentition; // la ca bug mais pas avant ? WTF ??
	//p5 = new Chien;
}
	POINTEUR(int) p;
	{
	//Tests basiques OK
     POINTEUR(int) p1 = new int;
     POINTEUR(int) p2 = p1;
     *p2 = 5;
     if (p1==p2) std::cout << "Test1 OK !" << std::endl;
     p1 = new int;
     if (p1!=p2) std::cout << "Test2 OK !" << std::endl;

     //Tests mémoire on realloue plusieurs int sur un smartpointeur OK
     p1 = new int;
     p = p1;
     *p1 = 2;
     p1 = new int;
     *p1 = 1337;
     std::cout << "p1 = " << *p1 << std::endl;
    }

    //Test access
    if (*p == 2 ) std::cout << "Test3 OK !" << std::endl;

    POINTEUR(POINTEUR(int)) ps4;
	{
		 //Tests pointeurs vers pile OK
		 int a = 42;
		 POINTEUR(int) p3 = a;

		 //Tests de pointeurs dans le tas
		 ps4 = p3;


		 //L'instruction suivante n'est pas possible, normale, p3 et p4
		 //ont 2 types différents
		 //if (p3!=p4) std::cout << "Test4 OK !" << std::endl;
	}

	std::cout << "p4 = " << *ps4 << std::endl;


	 POINTEUR(int) p5 = new int;
	 *p5 = 1337;
	 ps4 = p5;
	 std::cout << "p4 = " << *p5 << std::endl;

	 //Test cin OK
	 POINTEUR(char) pt = new char;
	 std::cout << "pt = " << *pt << std::endl;

	 {
		 POINTEUR(char) ptab[100];
		 ptab[1] = new char('a');
		 48[ptab] = ptab[1];
		 ptab[4] = new char('a');
		 if (ptab[1]!=ptab[4]) std::cout << "Test4 OK !" << std::endl;
		 if (*ptab[1]==*ptab[4]) std::cout << "Test5 OK !" << std::endl;
	}

	 //Tests boucles OK
	 {
		   POINTEUR(CC) ppile = new CC;
			ppile->p=new CC;
			ppile->p->p=ppile;
	}

	 //Tests Unif ? Pointers OK
	 POINTEUR(int) pp2 = new int(5); // ou bien new(1) CC
	 POINTEUR(int) pp3;
	 {
		 POINTEUR(POINTEUR(int)) ppile; // ou bien new(1) CC
		 ppile = pp2;
		 pp3 = *ppile;
		 std::cout << *pp3 << std::endl;
		 pp2 = new int;
	}

	//test t=t
	POINTEUR(int) t;
	t = t = t;
	t = new int;
	t = t = t;

	//Test tableau
	t = new int[20];
	t[15] = 16;
	std::cout << "t = " << t[15] << std::endl;

	//operateurs:
	POINTEUR(int) pplus;
	pplus = t[14];
	(*pplus)++;
	if (*pplus==t[15]) std::cout << "Test6 OK !" << std::endl;

	POINTEUR(char) ptinc;
	if(! ptinc) std::cout << "Test7 OK !" << std::endl;
	ptinc = new char;
	if(ptinc) std::cout << "Test8 OK !" << std::endl;


	test();
	POINTEUR(POINTEUR(int)) qsifh = pntg;
	t = new int(9);
	POINTEUR(POINTEUR(int)) qsifh2 = t;
	std::cout << *qsifh2 << std::endl;

	POINTEUR(int) za;
	{
		POINTEUR(int) zb = new int[10];
		za = zb[1];//zb[0]
	}

	//HARD ONE
	{
		POINTEUR(POINTEUR(POINTEUR(int))) ppp3 (new POINTEUR(POINTEUR(int))[3]);
		POINTEUR(POINTEUR(int)) ppp2 (new POINTEUR(int)[3]);
		ppp2 = new POINTEUR(int);
		ppp3 = new POINTEUR(POINTEUR(int))(new POINTEUR(int)(new int));
		POINTEUR(int) ppp5 = new int[10];
	}

	{
		POINTEUR(POINTEUR(int)) ppp1;
		ppp1 = new POINTEUR(int)(new int);
		*ppp1 = new int;
	}

	return 0;
}

