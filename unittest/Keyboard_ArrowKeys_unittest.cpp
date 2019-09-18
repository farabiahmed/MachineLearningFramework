#include <iostream>

using namespace std;


int main()
{
    char c = 0;
    while(1)
    {
        cin>>c;
        switch(c) {
        case 'W':
            cout << endl << "Up" << endl;//key up
            break;
        case 'S':
            cout << endl << "Down" << endl;   // key down
            break;
        case 'A':
            cout << endl << "Left" << endl;  // key left
            break;
        case 'D':
            cout << endl << "Right" << endl;  // key right
            break;
        case 'E':
            cout << endl << "Wait" << endl;  // key wait
            break;
        case 'Q':
            cout << endl << "Quit" << endl;  // key quit
			goto program_end;
            break;
        default:
            //cout << endl << "null" << endl;  // not arrow
            break;
        }
    }

program_end:

    return 0;
}
