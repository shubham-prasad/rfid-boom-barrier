#ifndef MENU_H
#define MENU_H

using namespace std; 

class MenuItem
{
public:
	String menuName;
  MenuItem();
	MenuItem(String n, void (*fn)(void)){
		menuName = n;
		function= fn;
	}
	void execute(){
		(*function)();
	}
private:
	void (*function)(void);
};	

class Menu
{
public:
	Menu(MenuItem* mi, int l, void (*f)(String)){
		menuItems = mi;
		index = 0;
    length = l;
		print = f;
		(*print)((menuItems[index]).menuName+" "+(index+1) +"/"+length);
	}
	void update(int input){
		switch(input){
			case BTN_UP : 
			index -= 1;
			if(index < 0) index = 0;
			(*print)((menuItems[index]).menuName+" "+(index+1) +"/"+length);
			break;
			case BTN_DOWN : 
			index += 1;
			if(index > length - 1) index = length - 1;
			(*print)((menuItems[index]).menuName+" "+(index+1) +"/"+length);
			break;
			case BTN_SELECT :
			(menuItems[index]).execute();
			break;
		}
	}
private:
	int index;
  int length;
	MenuItem* menuItems;
	void (*print)(String);
};


#endif
