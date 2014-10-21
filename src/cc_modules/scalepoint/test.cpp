#include <iostream>
#include "stdint.h"
#include "scalepoint.h"

using namespace std;

int main(){

	ScalePoint *sp_list;
	ScalePoint *sp_list_aux;
	ScalePoint sp;
	ScalePoint sp2;
	ScalePoint sp3;

	uint8_t* pchar;

	char buff[10];
	char buff2[10];
	char buff3[10];

	float val = 12.4;

	pchar = (uint8_t*) &val;

	sp_list = &sp;

	if(sp_list->allocScalePoint()){
		sp_list->setLabel("Scale1", 6);
		sp_list->setValue(pchar);

		sp_list_aux = sp_list;
		sp_list->setNext(&sp2);
		sp_list = sp_list->getNext();
		sp_list->setPrevious(sp_list_aux);

	}
	val = 2*12.4;
	if(sp_list->allocScalePoint()){
		sp_list->setLabel("Scale2", 6);

		cout << "val" << val << endl;

		sp_list->setValue(pchar);

		sp_list_aux = sp_list;
		sp_list->setNext(&sp3);
		sp_list = sp_list->getNext();
		sp_list->setPrevious(sp_list_aux);

	}
	val = 3*12.4;
	if(sp_list->allocScalePoint()){
		sp_list->setLabel("Scale3", 6);
		sp_list->setValue(pchar);

		sp_list = sp_list->getPrevious();
		sp_list = sp_list->getPrevious();
	}

	sp_list->getLabel(buff,6);
	sp_list = sp_list->getNext();
	sp_list->getLabel(buff2,6);
	sp_list = sp_list->getNext();
	sp_list->getLabel(buff3,6);

	sp_list = sp_list->getPrevious();
	sp_list = sp_list->getPrevious();

	buff[6] = 0;
	buff2[6] = 0;
	buff3[6] = 0;

	cout << buff << ": " << sp_list->getValue() << endl;
	sp_list = sp_list->getNext();
	cout << buff2 << ": " << sp_list->getValue() << endl;
	sp_list = sp_list->getNext();
	cout << buff3 << ": " << sp_list->getValue() << endl;

	// sp.freeScalePoint();

	// sp1.setLabel("lucas", 5);
	// if(sp2.allocScalePoint()){
	// 	cout << "RESET: " << sp2.setLabel("locas", 3) << endl;
	// 	sp2.getLabel(buff2,5);
	// }

	// while(!sp3.allocScalePoint()){
	// 	cout << "nao tem mais" << endl;
	// 	sp.freeScalePoint();
	// }

	return 0;
}