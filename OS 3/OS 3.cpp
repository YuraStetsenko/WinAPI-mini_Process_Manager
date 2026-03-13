// OS 3.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "OS 3.h"
#include <string>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <ctime>
#include <CommCtrl.h> 
#include <map>

#define MAX_LOADSTRING 100
#define MAX_PROCESSES_POWER 3
#define ONE_CONSOLE 0
#define MAX_THREADS 1000

struct MyDataSingle {
    HANDLE lpHandle;
    int i;
    int j;
};
struct MyDataMultiple {
    HWND hWnd;
    HANDLE* lpHandles;
    clock_t start;
    int i;
    int N;
};


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

bool is_list_opened = 0;
DWORD ConsolePID = 0;
DWORD dwLastThreadId;
WCHAR buffer[256];
RECT rect;
HWND ListBoxWindow;
bool is_ListBox_ON = 0;
std::wstring result;
std::vector<std::vector<STARTUPINFO>> SIs;
std::vector<std::vector<PROCESS_INFORMATION>> PIs;
DWORD PriorityByIndex[] = {
    REALTIME_PRIORITY_CLASS,
    HIGH_PRIORITY_CLASS,
    ABOVE_NORMAL_PRIORITY_CLASS,
    NORMAL_PRIORITY_CLASS,
    BELOW_NORMAL_PRIORITY_CLASS,
    IDLE_PRIORITY_CLASS };
const WCHAR* Priorities[] = { 
    L"У реальному часі",
    L"Високий",
    L"Вище звичайного",
    L"Звичайний", 
    L"Нижче звичайного",
    L"Низький",  };
std::vector<std::vector<bool>> Processes_status;
std::map<DWORD, const WCHAR*> PriorityMap = { 
    {IDLE_PRIORITY_CLASS, Priorities[5]},
    {BELOW_NORMAL_PRIORITY_CLASS, Priorities[4]},
    {NORMAL_PRIORITY_CLASS, Priorities[3]}, 
    {ABOVE_NORMAL_PRIORITY_CLASS, Priorities[2]},
    {HIGH_PRIORITY_CLASS,Priorities[1]},
    {REALTIME_PRIORITY_CLASS, Priorities[0]} };


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

STARTUPINFO ConsoleSI;
PROCESS_INFORMATION ConsolePI;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OS3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

 

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OS3));

    MSG msg;
    if (ONE_CONSOLE) {
        CreateProcessW(L"Console.exe",   //Шлях до виконуваного файлу
            (LPWSTR)L"",             // argv
            NULL,           // Не використовуємо об'єкти забезпечення безпеки процесу
            NULL,           // Не використовуємо об'єкти забезпечення безпеки потоку
            FALSE,          // Не успадковуємо дескриптори
            0,              // Немає додаткових флагів створення
            NULL,           // Не використовуємо новий середовищний блок
            NULL,           // Не використовуємо поточний каталог
            &ConsoleSI,        // Посилання на структуру STARTUPINFO
            &ConsolePI);
        ConsolePID = ConsolePI.dwProcessId;
    }

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OS3));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_OS3);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));


    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
HWND hWndListView;
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

 

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//void AddListViewItem(HWND hwndListView, int column_index, int row_index, LPCWSTR text) {
//    LVITEM lvItem;
//    lvItem.mask = LVIF_TEXT ;
//    lvItem.iItem = row_index; // Index of the item (row)
//    lvItem.iSubItem = column_index; // Index of the subitem (column)
//    lvItem.pszText = const_cast<LPWSTR>(text); // Text of the item
//    //lvItem.iGroup 
//    // Insert the item into the ListView control
//    ListView_InsertItem(hwndListView, &lvItem);
//}



WCHAR buffer1[256], buffer2[256], buffer3[256], buffer4[256], buffer5[256];
int steps = 0, processN = 0;
double A = 0, B = 0, e = 0;
HWND hCombo1;

DWORD APIENTRY WaitForProcessEnd_multiple(LPVOID lpParam){
    MyDataMultiple *pArgs = (MyDataMultiple*)lpParam;

    WaitForMultipleObjects(pArgs->N, pArgs->lpHandles, 1, INFINITE);

    clock_t end = clock();

    PIs[pArgs->i].erase(PIs[pArgs->i].begin(), PIs[pArgs->i].end());
    SIs[pArgs->i].erase(SIs[pArgs->i].begin(), SIs[pArgs->i].end());
    Processes_status[pArgs->i].erase(Processes_status[pArgs->i].begin(), Processes_status[pArgs->i].end());
    
    PIs[pArgs->i].erase(PIs[pArgs->i].begin(), PIs[pArgs->i].end());
    SIs[pArgs->i].erase(SIs[pArgs->i].begin(), SIs[pArgs->i].end());
    Processes_status[pArgs->i].erase(Processes_status[pArgs->i].begin(), Processes_status[pArgs->i].end());

    std::wstringstream input1;
    input1.str(L"");
    input1.clear();
    input1 << pArgs->N;
    result = input1.str();
    result += L" процес(и/ів) виконано за ";
    input1.str(L"");
    input1.clear();
    input1 << (((double)(end - pArgs->start)) / CLOCKS_PER_SEC * 1000);
    result += input1.str();
    result += L" мілісекунд";

    MessageBox(GetTopWindow(pArgs->hWnd), result.c_str(), L"Готово!", MB_ICONINFORMATION);

    Sleep(1000);
    delete[] pArgs->lpHandles;
    HeapFree(GetProcessHeap(), 0, lpParam);
    
    return 0;
}


DWORD APIENTRY WaitForProcessEnd_single(LPVOID lpParam) {
    MyDataSingle* pArgs = (MyDataSingle*)lpParam;
   
    WaitForSingleObject(PIs[pArgs->i][pArgs->j].hProcess, INFINITE);

    int index = 0;
    for (int i = 0; i < pArgs->i; i++)
        for (int j = 0; j < PIs[i].size(); j++)
            if (PIs[i][j].hProcess != NULL)
                index++;
    for (int j = 0; j <= pArgs->j; j++)
        if (PIs[pArgs->i][j].hProcess != NULL)
            index++;

    CloseHandle(PIs[pArgs->i][pArgs->j].hProcess);
    PIs[pArgs->i][pArgs->j].hProcess = NULL;

    if (is_ListBox_ON)
        SendMessageW(ListBoxWindow, LB_DELETESTRING, index-1, 0);
    
    HeapFree(GetProcessHeap(), 0, lpParam);
   
    return 0;
}

std::vector<HANDLE> Threads;

INT_PTR CALLBACK InputDialogProc5(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParamm)
{
    switch (message)
    {
    case WM_INITDIALOG:
        hCombo1 = GetDlgItem(hDlg, IDC_COMBO_INPUT1);
        for (short i = 0; i <= MAX_PROCESSES_POWER; i++) {
            WCHAR choice[3];
            _itow_s(pow(2,i), choice, 10);
            SendMessage(hCombo1, CB_ADDSTRING, 0, (LPARAM) choice);
        }

        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_OK:
        {
            hCombo1 = GetDlgItem(hDlg, IDC_COMBO_INPUT1);
            int selectedIndex1 = SendMessage(hCombo1, CB_GETCURSEL, 0, 0);
            if (selectedIndex1 != CB_ERR)
            {
                SendMessage(hCombo1, CB_GETLBTEXT, selectedIndex1, (LPARAM) buffer1);
            }
            else
            {
                GetDlgItemText(hDlg, IDC_COMBO_INPUT1, buffer1, sizeof(buffer1) / sizeof(buffer1[0]));
            }


            GetDlgItemText(hDlg, IDC_EDIT_INPUT2, buffer2, sizeof(buffer2) / sizeof(buffer2[0]));
            GetDlgItemText(hDlg, IDC_EDIT_INPUT3, buffer3, sizeof(buffer3) / sizeof(buffer3[0]));
            GetDlgItemText(hDlg, IDC_EDIT_INPUT4, buffer4, sizeof(buffer4) / sizeof(buffer4[0]));
            GetDlgItemText(hDlg, IDC_EDIT_INPUT5, buffer5, sizeof(buffer5) / sizeof(buffer5[0]));


            if (!(swscanf_s(buffer1, L"%d", &processN) && 
                swscanf_s(buffer2, L"%lf", &A) && 
                swscanf_s(buffer3, L"%lf", &B) && 
                swscanf_s(buffer4, L"%d",&steps) && 
                swscanf_s(buffer5, L"%lf", &e) &&
                (processN == 1 || processN == 2 || processN == 4 || processN == 8) &&
                A < B &&
                steps >= 100000 &&
                e < 1)) {
                EndDialog(hDlg, LOWORD(wParam));
                MessageBox(hDlg, L"Invalid number input. Try again and make sure the left border is smaller than the right one, epsilon < 1, stepsN >= 100000, processN is chosen from the dropbox", L"Error", MB_OK | MB_ICONERROR);
                //DialogBox(hInst, MAKEINTRESOURCE(IDD_INPUT_DIALOG), hDlg, InputDialogProc5);
            }
            else
            { 
                
                double length = B - A, stepLen = length / steps, sublength = length / processN;
               
                /*for (int i = 0; i<PIs.size();i++)
                    i0 += PIs[i].size();*/
                std::wstringstream input;
                std::wstring args;
                HANDLE* lpHandles = new HANDLE[processN]{};

                
                int i = 0;
                bool icond = 0;
                
                for (auto IT = PIs.begin(); IT != PIs.end(); IT++)
                    if (IT->size() == 0)
                        PIs.erase(IT);
                i = PIs.size();

                PIs.push_back(std::vector<PROCESS_INFORMATION>());
                SIs.push_back(std::vector<STARTUPINFO>());
                Processes_status.push_back(std::vector<bool>());

                    
                clock_t start = clock();
                
                for (int j = 0; j < processN; j++)
                {
                    
                    SIs[i].push_back(STARTUPINFO());
                    PIs[i].push_back(PROCESS_INFORMATION());
                    Processes_status[i].push_back(bool(1));

                    // Очистка пам'яті для структур
                    ZeroMemory(&SIs[i][j], sizeof(&SIs[i][j]));
                    SIs[i][j].cb = sizeof(SIs[i]);
                    ZeroMemory(&PIs[i][j], sizeof(PIs[i][j]));

                    input.str(L"");
                    input.clear();
                    input << L"Process #" << j + 1;
                    result = input.str();
                    SIs[i][j].lpTitle = (WCHAR*)(result.c_str());

                    input.str(L"");
                    input.clear();                  

                    input << A + sublength * j  << L" " << A + sublength * (j + 1) + stepLen << L" " << e << L" " << stepLen << L" " <<  ConsolePID;
                    args = input.str();         
                    
                    if (!CreateProcessW(L"process.exe",   //Шлях до виконуваного файлу
                        (LPWSTR)args.c_str(),             // argv
                        NULL,           // Не використовуємо об'єкти забезпечення безпеки процесу
                        NULL,           // Не використовуємо об'єкти забезпечення безпеки потоку
                        FALSE,          // Не успадковуємо дескриптори
                        0,              // Немає додаткових флагів створення
                        NULL,           // Не використовуємо новий середовищний блок
                        NULL,           // Не використовуємо поточний каталог
                        &SIs[i][j],        // Посилання на структуру STARTUPINFO
                        &PIs[i][j]))       // Посилання на структуру PROCESS_INFORMATION
                    {
                        std::wfstream errorfile(L"errors.txt", std::ios::out);
                        errorfile << "Не вдалося створити процес. Помилка: " << GetLastError() << std::endl;
                        errorfile.close();
                        delete[] lpHandles;
                        EndDialog(hDlg, LOWORD(wParam));
                        return (INT_PTR)TRUE;
                    }
                    else {
                        //MessageBox(hDlg, L"Процес успішно створений", L"Готово!", MB_ICONINFORMATION);
                        lpHandles[j] = PIs[i][j].hProcess;
                    }
                }
                
                MyDataSingle* pArgsSingle;
                MyDataMultiple *pArgsMultiple;  

                pArgsMultiple = (MyDataMultiple*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MyDataMultiple));
                if (pArgsMultiple == NULL)
                {
                    // If the array allocation fails, the system is out of memory
                    // so there is no point in trying to print an error message.
                    // Just terminate execution.
                    ExitProcess(2);
                }
                pArgsMultiple->hWnd = hDlg;
                pArgsMultiple->start = start;
                pArgsMultiple->N = processN;
                pArgsMultiple->i = i;
                pArgsMultiple->lpHandles = lpHandles;

               

                DWORD dwExitCode = 0;
                icond = 0;
                int free_i = 0;
                for (; free_i < Threads.size(); free_i++) 
                {
                    GetExitCodeThread(Threads[free_i], &dwExitCode);
                    if (dwExitCode != STILL_ACTIVE) {
                        icond = 1;
                        break;
                    }
                }

                if (!icond)
                {
                    Threads.push_back(CreateThread(
                            NULL,                   // default security attributes
                            0,                      // use default stack size  
                            WaitForProcessEnd_multiple,       // thread function name
                            pArgsMultiple,          // argument to thread function 
                            0,                      // use default creation flags 
                            &dwLastThreadId));   // returns the thread identifier 
                }
                else
                {
                    CloseHandle(Threads[free_i]);
                    Threads[free_i] = (CreateThread(
                        NULL,                   // default security attributes
                        0,                      // use default stack size  
                        WaitForProcessEnd_multiple,       // thread function name
                        pArgsMultiple,          // argument to thread function 
                        0,                      // use default creation flags 
                        &dwLastThreadId));   // returns the thread identifier 
                    
                }
                for (int k = 0; k < processN; k++)
                {
                    icond = 0;
                    for (free_i = 0; free_i < Threads.size(); free_i++)
                    {
                        GetExitCodeThread(Threads[free_i], &dwExitCode);
                        if (dwExitCode != STILL_ACTIVE) {
                            icond = 1;
                            break;
                        }
                    }

                    pArgsSingle = (MyDataSingle*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MyDataSingle));
                    if(pArgsSingle == NULL)
                    {
                        // If the array allocation fails, the system is out of memory
                        // so there is no point in trying to print an error message.
                        // Just terminate execution.
                        ExitProcess(2);
                    }

                    pArgsSingle->i = i;
                    pArgsSingle->j = k;
                    pArgsSingle->lpHandle = lpHandles[k];

                    if (!icond)
                    {
                        Threads.push_back(CreateThread(
                            NULL,                   // default security attributes
                            0,                      // use default stack size  
                            WaitForProcessEnd_single ,       // thread function name
                            pArgsSingle,          // argument to thread function 
                            0,                      // use default creation flags 
                            &dwLastThreadId));   // returns the thread identifier 
                    }
                    else
                    {
                        CloseHandle(Threads[free_i]);
                        Threads[free_i] = (CreateThread(
                            NULL,                   // default security attributes
                            0,                      // use default stack size  
                            WaitForProcessEnd_single,       // thread function name
                            pArgsSingle,          // argument to thread function 
                            0,                      // use default creation flags 
                            &dwLastThreadId));   // returns the thread identifier 
                    }
                }
               

                

                /*if (ONE_CONSOLE) {
                    _getch();
                    FreeConsole();
                }*/

                
            }
            
            
            EndDialog(hDlg, LOWORD(wParam));
            

            return (INT_PTR)TRUE;
        }

        case IDC_BUTTON_CANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
template <unsigned N>
void CreateCentredButtons(HWND hWnd, RECT rect, const WCHAR* text[N], int startingID, bool enabled) {
    for (int i = 0; i < N; i++)
        CreateWindow(L"BUTTON", text[i], WS_TABSTOP | (enabled?WS_VISIBLE:0) | WS_CHILD | BS_DEFPUSHBUTTON,
            (rect.right - 500) / 2, ((rect.bottom - 50*N - 30*(N-1)) / 2) + i * 80, 500, 50, hWnd, (HMENU) (startingID + i), hInst, NULL);
}


void DrawRoundRadioButton(HDC hdc, int x, int y, BOOL checked)
{
    // Draw outer circle
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255)); // Fill color
    SelectObject(hdc, hPen);
    SelectObject(hdc, hBrush);
    Ellipse(hdc, x, y, x + 15, y + 15);

    // Draw inner circle if checked
    if (checked)
    {
        HBRUSH hInnerBrush = CreateSolidBrush(RGB(0, 0, 0)); // Fill color
        SelectObject(hdc, hInnerBrush);
        Ellipse(hdc, x + 3, y + 3, x + 12, y + 12);
        DeleteObject(hInnerBrush);
    }

    DeleteObject(hPen);
    DeleteObject(hBrush);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//



void InitListViewColumns(HWND hwndListView) {
    LVCOLUMN lvColumn;
    lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lvColumn.cx = 180; // Set the width of the column
    lvColumn.pszText = (WCHAR*)(L"Назва процесу"); // Set the text of the column header
    lvColumn.iSubItem = 0; // Set the subitem index (0-based)

    // Insert the first column header
    ListView_InsertColumn(hwndListView, 0, &lvColumn);

    // Repeat the process for additional columns
    // Example for a second column:
    lvColumn.pszText = (WCHAR*)(L"Пріоритет");
    lvColumn.cx = 150;
    lvColumn.iSubItem = 1;
    ListView_InsertColumn(hwndListView, 1, &lvColumn);

    lvColumn.pszText = (WCHAR*)(L"Стан");
    lvColumn.cx = 100;
    lvColumn.iSubItem = 2;
    ListView_InsertColumn(hwndListView, 2, &lvColumn);
}



bool is_cos_processes_launched = 0;
bool priority_setting[6] = { 0,0,0,0,0,0 };
int index_N = 0;

INT_PTR CALLBACK InputDialogProcList(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_LBUTTONDOWN:
    {
        //WCHAR text[256];
        POINT cursor;
        RECT rect;
        GetCursorPos(&cursor);
        GetWindowRect(hDlg, &rect);
        int x = cursor.x - rect.left, y = cursor.y - rect.top;
        if (x >= 502 && x <= 517) {
            for (int i = 0; i < 6; i++) {
                if (y >= (64 + i * 32) && y <= (79 + i * 32)) {
                    /*swprintf_s(text, 256, L"x: %d, y: %d", x, y);
                    MessageBox(hDlg, text, L"", MB_ICONINFORMATION);*/
                    for (int j = 0; j < 6; j++)
                    {
                        if (i != j && priority_setting[j])
                        {
                            priority_setting[j] = 0;
                            break;
                        }
                    }

                    priority_setting[i] = !(priority_setting[i]);
                    GetClientRect(hDlg, &rect);
                    InvalidateRect(hDlg, &rect, 0);
                    UpdateWindow(hDlg);
                    break;
                }

            }
        }
        return (INT_PTR)TRUE;
    }
    break;
    case WM_PAINT:
    {
        HDC hdc;
        PAINTSTRUCT ps;
        hdc = BeginPaint(hDlg, &ps);
        DrawRoundRadioButton(hdc, 495, 32, priority_setting[0]);
        DrawRoundRadioButton(hdc, 495, 64, priority_setting[1]);
        DrawRoundRadioButton(hdc, 495, 96, priority_setting[2]);
        DrawRoundRadioButton(hdc, 495, 128, priority_setting[3]);
        DrawRoundRadioButton(hdc, 495, 160, priority_setting[4]);
        DrawRoundRadioButton(hdc, 495, 192, priority_setting[5]);
        EndPaint(hDlg, &ps);
        return (INT_PTR)TRUE;
    }
    break;
    case WM_INITDIALOG:
        // Add items to the list box
    {
        is_ListBox_ON = 1;
        ListBoxWindow = GetDlgItem(hDlg, IDC_LIST_BOX);
        index_N = 0;
        std::wstringstream input;
        for (int i = 0; i < PIs.size(); i++)
            for (int j = 0; j < PIs[i].size(); j++, index_N++)
            {
                if (PIs[i][j].hProcess != NULL) {
                    input.str(L"");
                    input.clear();
                    input << L"Process #" << index_N + 1 << L"    " << ((Processes_status[i][j]) ? (L"Активний") : (L"Призупенений")) << L"    " << PriorityMap[GetPriorityClass(PIs[i][j].hProcess)];
                    result = input.str();
                    SendMessageW(ListBoxWindow, LB_ADDSTRING, index_N, reinterpret_cast<LPARAM>(result.c_str()));
                }
                /*AddListViewItem(hwndListView, 0,i, result.c_str());
                AddListViewItem(hwndListView, 1,i,PriorityMap[GetPriorityClass(PIs[i].hProcess)]);
                AddListViewItem(hwndListView, 2,i, L"Активний");*/
            }
        // Add more items as needed
    }
    return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_CHANGE_PRIORITY:
        {
            short ChangedPriority_index = -1;
            short selected_index = -1;
            bool condition = 1;


            for (int i = 0; i < 6; i++)
                if (priority_setting[i]) {
                    ChangedPriority_index = i;
                    break;
                }
            selected_index = SendMessage(ListBoxWindow, LB_GETCURSEL, 0, 0);
            if (selected_index < 0) {
                MessageBox(hDlg, L"Спочатку оберіть процес", L"Info", MB_ICONWARNING);
                condition = 0;
            }

            if (ChangedPriority_index < 0) {
                MessageBox(hDlg, L"Спочатку оберіть пріоритет для зміни", L"Info", MB_ICONWARNING);
                condition = 0;
            }

            if (condition)
            {
                int i = 0, j = 0, index_of_proc = 0;

                for (; i < PIs.size(); i++)
                {
                    for (j = 0; j < PIs[i].size(); )
                    {
                        if (index_of_proc == selected_index)
                            break;
                        if (PIs[i][j].hProcess != NULL)
                            index_of_proc++;
                        j++;
                        if (j != PIs[i].size() && index_of_proc == selected_index)
                            break;
                    }
                    if (j == PIs[i].size())
                        j--;
                    else
                        break;
                }

                while (j < PIs[i].size() && PIs[i][j].hProcess == NULL)
                    j++;

                if (j == PIs[i].size())
                {
                    for (i = i + 1, j = 0; i < PIs.size() && PIs[i][j].hProcess == NULL; i++)
                    {
                        for (j = 0; j < PIs[i].size(); j++)
                            if (PIs[i][j].hProcess != NULL)
                                break;

                        if (j == PIs[i].size())
                            j--;
                        else
                            break;
                    }
                    if (i == PIs.size())
                        i--;
                }

                if (index_of_proc != selected_index || PIs[i][j].hProcess == NULL)
                    MessageBox(hDlg, L"Процес вже не існує", L"", MB_ICONINFORMATION);
                else
                {
                    if (PIs[i][j].hProcess == NULL)
                        return (INT_PTR)TRUE;

                    std::wstringstream input;
                    input.str(L"");
                    input.clear();
                    input << L"Process #" << selected_index + 1 << L"    ";

                    result = input.str();
                    //WCHAR nigga[256];
                    //swprintf(nigga, 256, L"i: %d, j: %d", i, j);
                    //MessageBox(hDlg, nigga, L"", MB_ICONINFORMATION);
                    result += ((Processes_status[i][j]) ? (L"Активний    ") : (L"Призупенений    "));
                    result += Priorities[ChangedPriority_index];
                    SetPriorityClass(PIs[i][j].hProcess, PriorityByIndex[ChangedPriority_index]);
                    SendMessage(ListBoxWindow, LB_DELETESTRING, (WPARAM)selected_index, 0);
                    SendMessage(ListBoxWindow, LB_INSERTSTRING, selected_index, (LPARAM)result.c_str());
                }

                ////

            }


            //MessageBox(hDlg, L"nigga", L"", MB_ICONINFORMATION); 
            return (INT_PTR)TRUE;
        }
        break;
        case IDC_SUSPEND:
        {
            short selected_index = -1;
            bool condition = 1;

            selected_index = SendMessage(ListBoxWindow, LB_GETCURSEL, 0, 0);
            if (selected_index < 0) {
                MessageBox(hDlg, L"Спочатку оберіть процес", L"Info", MB_ICONWARNING);
                condition = 0;
            }

            if (condition)
            {
                int i = 0, j = 0, index_of_proc = 0;

                for (; i < PIs.size(); i++)
                {
                    for (j = 0; j < PIs[i].size(); )
                    {
                        if (index_of_proc == selected_index)
                            break;
                        if (PIs[i][j].hProcess != NULL)
                            index_of_proc++;
                        j++;
                        if (j != PIs[i].size() && index_of_proc == selected_index)
                            break;
                    }
                    if (j == PIs[i].size())
                        j--;
                    else
                        break;
                }

                while (j < PIs[i].size() && PIs[i][j].hProcess == NULL)
                    j++;

                if (j == PIs[i].size())
                {
                    for (i = i + 1, j = 0; i < PIs.size() && PIs[i][j].hProcess == NULL; i++)
                    {
                        for (j = 0; j < PIs[i].size(); j++)
                            if (PIs[i][j].hProcess != NULL)
                                break;

                        if (j == PIs[i].size())
                            j--;
                        else
                            break;
                    }
                    if (i == PIs.size())
                        i--;
                }

                if (index_of_proc != selected_index || PIs[i][j].hProcess == NULL)
                    MessageBox(hDlg, L"Процес вже не існує", L"", MB_ICONINFORMATION);
                else
                {
                    if (PIs[i][j].hProcess == NULL)
                        return (INT_PTR)TRUE;

                    std::wstringstream input;
                    input.str(L"");
                    input.clear();
                    input << L"Process #" << selected_index + 1 << L"    ";
                    result = input.str();

                    SuspendThread(PIs[i][j].hThread);
                    Processes_status[i][j] = 0;

                    result += ((Processes_status[i][j]) ? (L"Активний    ") : (L"Призупенений    "));
                    result += PriorityMap[GetPriorityClass(PIs[i][j].hProcess)];
                    SendMessage(ListBoxWindow, LB_DELETESTRING, (WPARAM)selected_index, 0);
                    SendMessage(ListBoxWindow, LB_INSERTSTRING, selected_index, (LPARAM)result.c_str());
                }
            }
            return (INT_PTR)TRUE;
        }
        break;
        case IDC_RESUME:
        {
            short selected_index = -1;
            bool condition = 1;

            selected_index = SendMessage(ListBoxWindow, LB_GETCURSEL, 0, 0);
            if (selected_index < 0) {
                MessageBox(hDlg, L"Спочатку оберіть процес", L"Info", MB_ICONWARNING);
                condition = 0;
            }

            if (condition)
            {
                int i = 0, j = 0, index_of_proc = 0;

                for (; i < PIs.size(); i++)
                {
                    for (j = 0; j < PIs[i].size(); )
                    {
                        if (index_of_proc == selected_index)
                            break;
                        if (PIs[i][j].hProcess != NULL)
                            index_of_proc++;
                        j++;
                        if (j != PIs[i].size() && index_of_proc == selected_index)
                            break;
                    }
                    if (j == PIs[i].size())
                        j--;
                    else
                        break;
                }

                while (j < PIs[i].size() && PIs[i][j].hProcess == NULL)
                    j++;

                if (j == PIs[i].size())
                {
                    for (i = i + 1, j = 0; i < PIs.size() && PIs[i][j].hProcess == NULL; i++)
                    {
                        for (j = 0; j < PIs[i].size(); j++)
                            if (PIs[i][j].hProcess != NULL)
                                break;

                        if (j == PIs[i].size())
                            j--;
                        else
                            break;
                    }
                    if (i == PIs.size())
                        i--;
                }

                if (index_of_proc != selected_index || PIs[i][j].hProcess == NULL)
                    MessageBox(hDlg, L"Процес вже не існує", L"", MB_ICONINFORMATION);
                else
                {
                    if (PIs[i][j].hProcess == NULL)
                        return (INT_PTR)TRUE;

                    std::wstringstream input;
                    input.str(L"");
                    input.clear();
                    input << L"Process #" << selected_index + 1 << L"    ";
                    result = input.str();

                    ResumeThread(PIs[i][j].hThread);
                    Processes_status[i][j] = 1;

                    result += ((Processes_status[i][j]) ? (L"Активний    ") : (L"Призупенений    "));
                    result += PriorityMap

                        [GetPriorityClass(PIs[i][j].hProcess)];
                    SendMessage(ListBoxWindow, LB_DELETESTRING, (WPARAM)selected_index, 0);
                    SendMessage(ListBoxWindow, LB_INSERTSTRING, selected_index, (LPARAM)result.c_str());
                }
            }
            return (INT_PTR)TRUE;
        }
        break;
        case IDC_TERMINATE:
        {
            short selected_index = -1;
            bool condition = 1;

            selected_index = SendMessage(ListBoxWindow, LB_GETCURSEL, 0, 0);
            if (selected_index < 0) {
                MessageBox(hDlg, L"Спочатку оберіть процес", L"Info", MB_ICONWARNING);
                condition = 0;
            }

            if (condition)
            {
                int i = 0, j = 0, index_of_proc = 0;

                for (; i < PIs.size(); i++)
                {
                    for (j = 0; j < PIs[i].size(); )
                    {
                        if (index_of_proc == selected_index)
                            break;
                        if (PIs[i][j].hProcess != NULL)
                            index_of_proc++;
                        j++;
                        if (j != PIs[i].size() && index_of_proc == selected_index)
                            break;
                    }
                    if (j == PIs[i].size())
                        j--;
                    else
                        break;
                }

                while (j < PIs[i].size() && PIs[i][j].hProcess == NULL)
                    j++;

                if (j == PIs[i].size())
                {
                    for (i = i + 1, j = 0; i < PIs.size() && PIs[i][j].hProcess == NULL; i++)
                    {
                        for (j = 0; j < PIs[i].size(); j++)
                            if (PIs[i][j].hProcess != NULL)
                                break;

                        if (j == PIs[i].size())
                            j--;
                        else
                            break;
                    }
                    if (i == PIs.size())
                        i--;
                }

                if (index_of_proc != selected_index || PIs[i][j].hProcess == NULL)
                    MessageBox(hDlg, L"Процес вже не існує", L"", MB_ICONINFORMATION);
                else
                {
                    if (PIs[i][j].hProcess == NULL)
                        return (INT_PTR)TRUE;

                    TerminateProcess(PIs[i][j].hProcess, 0);
                    //CloseHandle(PIs[i][j].hProcess);

                    //PIs[i][j].hProcess = NULL;
                    /*SIs[i].erase(std::next(SIs[i].begin(), j));
                    Processes_status[i].erase(std::next(Processes_status[i].begin(),j));*/

                    //SendMessage(ListBoxWindow, LB_DELETESTRING, (WPARAM)selected_index, 0);
                }
            }
            return (INT_PTR)TRUE;
        }
        break;
        //case IDC_BUTTON_OK:
        //    // Handle OK button click
        //    // Add your code here to process the selected item from the list box
        //{
        //    HWND hListBox = GetDlgItem(hDlg, IDC_LIST_BOX);
        //    int selectedIndex = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
        //    if (selectedIndex != LB_ERR)
        //    {
        //       // wchar_t buffer[256];
        //        SendMessage(hListBox, LB_GETTEXT, selectedIndex, (LPARAM)buffer);

        //        // Process the selected item (in 'buffer') as needed
        //        // ...



        //        // For testing, display the selected item in a message box
        //        //MessageBox(hDlg, buffer, L"Selected Item", MB_OK | MB_ICONINFORMATION);
        //    }

        //}

            //EndDialog(hDlg, LOWORD(wParam));
        return (INT_PTR)TRUE;

        case ID_CANCEL:
            // Handle Cancel button click
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }



    return (INT_PTR)FALSE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        RECT rect;
        GetClientRect(hWnd, &rect);
        const WCHAR* texts[] = { L"Табулювати функцію cos(x)", L"Управління процесами"};
        CreateCentredButtons<sizeof(texts)/sizeof(texts[0])>(hWnd, rect, texts, ID_BUTTON_COSTAB, 1);
    }
    break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case ID_BUTTON_MANAGE_PROCESSES:
                is_ListBox_ON = 1;
                DialogBox(hInst, MAKEINTRESOURCE(IDD_LIST_DIALOG), hWnd, InputDialogProcList);
                is_ListBox_ON = 0;
                break;
            case ID_BUTTON_COSTAB:
                

                DialogBox(hInst, MAKEINTRESOURCE(IDD_INPUT_DIALOG), hWnd, InputDialogProc5); 
                break; 
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:

                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        for (int i = 0; i < PIs.size(); i++)
            for (auto IT = PIs[i].begin(); IT != PIs[i].end(); IT++)
                TerminateProcess(IT->hProcess, 0);
        /*for (int i = 0; i < Threads.size(); i++)
            CloseHandle(Threads[i]);*/
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return (INT_PTR)FALSE;
}
