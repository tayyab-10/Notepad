#include <iostream>
#include <conio.h>
#include <fstream>
#include <Windows.h>
#include <string>
#include <string.h>
#include <list>
#include <stack>
#include <deque>

using namespace std;
void GetrowcolbyLeftClick(int &rops, int &cpos)
{
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD prev_mode;
    GetConsoleMode(hInput, &prev_mode);
    SetConsoleMode(hInput, ENABLE_EXTENDED_FLAGS | (prev_mode & ~ENABLE_QUICK_EDIT_MODE));
    INPUT_RECORD event;
    DWORD numRead;
    ReadConsoleInput(hInput, &event, 1, &numRead);
    if (event.EventType == MOUSE_EVENT)
    {
        rops = event.Event.MouseEvent.dwMousePosition.Y;
        cpos = event.Event.MouseEvent.dwMousePosition.X;
    }
    SetConsoleMode(hInput, prev_mode);
}

void gotoRowCol(int rpos, int cpos)
{
    COORD coord;
    coord.X = cpos;
    coord.Y = rpos;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
list<string> Files;

struct state
{
    list<list<char>> text;
    list<list<char>>::iterator ritr;
    list<list<char>>::iterator citr;
    int r, c;
};

class CurrentFile
{
public:
    int CurRow, CurCol; // Fixed typo: Currow to CurRow
    list<list<char>> text;
    list<list<char>>::iterator ritr;
    list<list<char>>::iterator citr;
    deque<state> Undo;
    stack<state> Redo;

    CurrentFile()
    {
        text.push_back(list<char>());
        ritr = text.begin();
        (*ritr).push_back(' ');

        citr = (*ritr).begin();
        CurRow = 0; // Fixed typo: Currow to CurRow
        CurCol = 0;
    }

    state SaveState()
    {
        state *s = new state;
        s->text.push_back(list<char>());
        auto r_itr = s->text.begin();
        for (auto row = text.begin(); row != text.end(); row++)
        {
            s->text.push_back(list<char>());
            for (auto col = (*row).begin(); col != (*row).end(); col++)
            {
                char ch = *col;
                (*r_itr).push_back(ch); // Fixed typo: (*ritr) to (*r_itr)
            }
        }
        s->ritr = s->text.begin();
        s->citr = (*s->ritr).begin();
        s->c = CurCol;
        s->r = CurRow;
        return *s;
    }

    void LoadState(state s)
    {
        text = s.text;
        ritr = text.begin();
        CurCol = s.c;
        CurRow = s.r;
        for (int r = 0; r < s.r; r++)
        {
            ritr++;
        }
        citr = (*ritr).begin();
        for (int c = 0; c < s.c; c++)
        {
            citr++;
        }
    }
    void Create_File()
    {
        system("cls");
        string file_name;
        cout << "ENter File Name  :";
        cin >> file_name;
        if (find(Files.begin(), Files.end(), file_name) != Files.end()) // this is used to search the

        {
            cout << "File With this Name Already Exists :";
            return;
        }
        Files.push_back(file_name);
        ofstream Wrt(file_name.c_str());

        system("cls");
        Edit_File(Wrt);
        system("cls");
        system("color 09");
        Wrt.close();
    }
    void Edit_File(ofstream &Wrt)
    {
        system("Color F0");
        char c;
        // To get the characters from the user
        c = _getch();
        gotoRowCol(Currow, CurCol);
        // We replace it with the empty space character that we use earlier :
        cout << c;
        (*citr) = c;
        CurCol++;
        while (true)
        {
            if (Currow == 0)
            {
                gotoRowCol(Currow, CurCol);
            }
            else
            {
                gotoRowCol(Currow, CurCol + 1);
            }
            c = _getch();
            if (c == -32) // if arrow keys or del is pressed
            {
            A:
                c = _getch();
                if (c == 72) // up
                {
                    if (Currow == 0)
                        continue;
                    ritr--;
                    citr = (*ritr).begin();
                    Currow--;
                    CurCol = 0;
                }
                else if (c == 80) // down
                {
                    ritr++;
                    citr = (*ritr).begin();
                    Currow++;
                    CurCol = 0;
                }
                else if (c == 75) // left
                {
                    if (CurCol - 1 > 0)
                    {
                        citr--;
                        CurCol--;
                    }
                }
                else if (c == 75) // Right
                {
                    if (CurCol == 159)
                        continue;
                    citr++;
                    CurCol++;
                }
                else if (c == 83) // del
                {
                    auto temp = citr;
                    citr++;
                    (*ritr).erase(citr);
                    citr = temp;
                    system("cls");
                    print();
                    UpdateUndo();
                }
                if (CurCol == 0)
                {
                    gotoRowCol(Currow, 0);
                    c = _getch();
                    if (c == -32)
                        goto A;
                    (*ritr).push_front(c);
                    citr = (*ritr).begin();
                    CurCol = 1;
                    system("cls");
                    print();
                    UpdateUndo();
                }
                continue;

                else if (c == 13) // ENter
                {
                    auto temp = ritr;
                    ritr++;
                    text.insert(ritr, list<char>());
                    ritr = ++temp;
                    Currow++;
                    CurCol = 0;
                    gotoRowCol(Currow, CurCol);
                    c = _getch();
                    if (c == -32)
                    {
                        goto A;
                    }
                    (*ritr).push_back(c);
                    citr = (*ritr).begin();
                    system("cls");
                    print();
                    UpdateUndo();
                    continue;
                }
                else if (c == 8) // backspace
                {
                    if (CurCol == = 0)
                    {
                        gotoRowCol(Currow, 0);
                        c = _getch();
                        if (c == -32)
                        {
                            goto A;
                        }
                        if (c == 8)
                            continue;
                        (*citr) = c;
                        citr = (*ritr).begin();
                        CurCol = 1;
                        continue;
                    }
                    auto temp = --citr;
                    citr++;
                    (*ritr).erase(citr)
                        citr = temp;
                    CurCol--;
                    system("cls");
                    print();
                    UpdateUndo();
                    continue;
                }
                else if (c == 26) // undo
                {
                    if (!undo.empty())
                    {
                        state s = undo.back();
                        LoadState(s);
                        redo.push(undo.back());
                        undo.pop_back();
                        system("cls");
                        print();
                    }
                    continue;
                }
                else if (c == 25) // redo
                {
                    if (!redo.empty())
                    {
                        undo.push_back(redo.top());
                        state s = redo.top();
                        redo.pop();
                        LoadState(s);
                        system("cls");
                        print();
                    }
                    continue;
                }
                else if (c == 27) // esc to close the editor
                {
                    WriteTextToFile(Wrt);
                    break;
                }
                else if (CurCol == 159) // if we reach the end of the console we have to fix this it is not properly working
                {
                    text.push_back(list<char>());
                    ritr++;
                    CurCol;
                    CurCol = 0;
                    Currow++;
                }
                // If the user wants to wite something on the file simply inserts it
                else
                {
                    auto temp = citr;
                    citr++;
                    (*ritr).insert(citr, c);
                    citr = ++temp;
                    CurCol++;
                }
            }
            system("cls");
            print();
            UpdateUndo();
        }
        system("Color 07");
    };
    void WriteTextToFile(ofstream &wrt)
    {
        for (auto r = text.begin(); r != text.end(); r++)
        {
            for (auto c = (*r).begin(); c != (*r).end(); c++)
            {
                wrt << *c;
            }
            wrt << '\n';
        }
    }
    void OpenSavedFile(ifstream &rdr)
    {
        char ch;
        while (!rdr.eof()) // this loop iterates till the end of an Existing file and loads the existing file contents
        {
            rdr.get(ch);
            if (ch != '\n')
            {
                (*ritr).push_back(ch);
            }
            else
            {
                text.push_back(list<char>());
                ritr++;
            }
        }
    }

    void Closing(ifstream &Rdr)
    {
        Rdr.close();
        ofstream wrt;
        wrt.open("SaveFile.txt");
        for (auto i = Files.begin(); i != Files.end(); i++)
        {
            wrt << (*i) << endl;
        }
        wrt.close();
    }
    void Open_File() v // Existing File Open
    {
        system("cls");
        string file_name;
        cout << "Enter the name of the file :";
        cin >> file_name;
        if (find(Files.begin(), Files.end(), file_name) == Files.end())
        {
            cout << "File does not Exist ::";
            return;
        }
        ifstream Rdr(file_name.c_str());
        OpenSavedFile(Rdr);
        Rdr.close();
        ofstream Wrt(file_name.c_str());
        system("cls");
        print();
        gotoRowCol(Currow, CurCol);
        Edit_File(Wrt);
        system("cls");
        system("color 09");
        Wrt.close();
    }
    void UpdateUndo()
    {
        if (undo.size() > 5)
            undo.erase(undo.begin());
        state s;
        s = SaveState();
        undo.push_back(s);
    }
    void print()
    {
        for (auto r = text.begin(); r != text.end(); r++)
        {
            for (auto c = (*r).begin(); c != (*r).end(); c++)
            {
                cout << *c;
            }
            cout << endl;
        }
    }

    // This function is only loading our files
    void Input(ifstream &Rdr)
    {
        string fn;
        Rdr >> fn;
        while (!Rdr.eof())
        {
            Files.push_back(fn); // insert function called
            Rdr >> fn;           // Rdr is reading and storing in fn
        }
        {
            /* code */
        }
    }
    void Choice(ifstream &Rdr)
    {
        int choice = -1;
        int R = 0, C = 0;
        cout << "                  *****************" << endl;
        cout << "                  New File " << endl;
        cout << "                  Open Existing File" << endl;
        cout << "                  Exit" << endl;
        cout << "                  *****************" << endl;
        while (choice == -1)
        {
            GetrowcolbyLeftClick(R, C); // This function is use to use Mouse click
            if ((R >= 5 && R <= 7) && (C >= 80 && C <= 136))
                choice = 1;
            if ((R >= 8 && R <= 10) && (C >= 80 && C <= 136))
                choice = 2;
            if ((R >= 11 && R <= 13) && (C >= 80 && C <= 136))
                choice = 3;
        }
        switch (choice)
        {
        case 1:
            Create_File();
            break;
        case 2:
            Open_file();
            break;
        case 3:
            Closing(Rdr);
            exit(1);
            break;
        }
    }
};
int main()
{
    system("color 09");
    int r = 0, c = 0;
    CurrentFile N;
    ifstream Rdr;
    ofstream wrtr;
    Rdr.open("SaveFile.text"); // SaveFile is used to store the Existing file name
    N.Input(Rdr);
    while (true)
    {
        N.Choice(Rdr);
    }
    Rdr.close();
}
