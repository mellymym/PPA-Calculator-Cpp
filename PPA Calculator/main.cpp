#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

// Control IDs
#define ID_TOGGLE_LANG    101
#define ID_BTN_ADD        102
#define ID_BTN_CALC       103
#define ID_BTN_CLEAR      104
#define ID_EDIT_SUBJ      105
#define ID_EDIT_CRED      106
#define ID_EDIT_GRAD      107
#define ID_LIST_VIEW      108
#define ID_BTN_DELETE     109
#define ID_BTN_SAVE_SET   110
#define ID_TOGGLE_SETS    111
#define ID_LIST_SETS      112
#define ID_BTN_LOAD_SET   113
#define ID_BTN_DELETE_SET 114

// Global variables
HINSTANCE hInst;
bool isEnglish = true;
bool showSetsPanel = false;

// UI Handles
HWND hToggleLang, hBtnAdd, hBtnCalc, hBtnClear, hBtnDelete;
HWND hLblTitle, hLblSubj, hLblCred, hLblGrad;
HWND hEditSubj, hEditCred, hEditGrad;
HWND hList;
HWND hLblResult;
HWND hLblCredit; // mellymym
HWND hBtnSaveSet, hToggleSets, hListSets, hBtnLoadSet, hBtnDeleteSet, hLblSetsHeader;

HFONT hFont, hFontLarge, hFontTitle;

// Accent colors
const COLORREF COLOR_ACCENT = RGB(0, 120, 215);   // frame borders
const COLORREF COLOR_TITLE_TEXT = RGB(0, 90, 160);     // title text
const COLORREF COLOR_RESULT_TEXT = RGB(0, 130, 90);     // result text

// Data
struct Subject {
    std::wstring name;
    double credits;
    double grade;
    bool hasGrade;
};
std::vector<Subject> subjects;

struct SavedSet {
    std::wstring name;
    std::vector<Subject> subjects;
};
std::vector<SavedSet> savedSets;

// Texts
const wchar_t* txtToggleEn = L"ES";
const wchar_t* txtToggleEs = L"EN";
const wchar_t* txtSubjEn = L"Subject Name:";
const wchar_t* txtSubjEs = L"Nombre Asignatura:";
const wchar_t* txtCredEn = L"Credits:";
const wchar_t* txtCredEs = L"Cr\u00E9ditos:";
const wchar_t* txtGradEn = L"Grade (optional):";
const wchar_t* txtGradEs = L"Nota (opcional):";
const wchar_t* txtAddEn = L"Add";
const wchar_t* txtAddEs = L"Agregar";
const wchar_t* txtCalcEn = L"Calculate";
const wchar_t* txtCalcEs = L"Calcular";
const wchar_t* txtClearEn = L"Clear All";
const wchar_t* txtClearEs = L"Borrar Todo";
const wchar_t* txtDeleteEn = L"Delete Sel.";
const wchar_t* txtDeleteEs = L"Eliminar Sel.";
const wchar_t* txtResEn = L"PAPA: 0.00";
const wchar_t* txtResEs = L"PAPA: 0.00";
const wchar_t* txtCredit = L"mellymym";
const wchar_t* txtTitleEn = L"PAPA Calculator";
const wchar_t* txtTitleEs = L"Calculadora PAPA";

const wchar_t* txtSaveSetEn = L"Save Set";
const wchar_t* txtSaveSetEs = L"Guardar Set";
const wchar_t* txtSetsCollapsedEn = L"Saved Sets \u25B8";
const wchar_t* txtSetsCollapsedEs = L"Sets Guardados \u25B8";
const wchar_t* txtSetsExpandedEn = L"Saved Sets \u25BE";
const wchar_t* txtSetsExpandedEs = L"Sets Guardados \u25BE";
const wchar_t* txtSetsHeaderEn = L"Saved Sets";
const wchar_t* txtSetsHeaderEs = L"Sets Guardados";
const wchar_t* txtLoadSetEn = L"Load";
const wchar_t* txtLoadSetEs = L"Cargar";
const wchar_t* txtDeleteSetEn = L"Delete";
const wchar_t* txtDeleteSetEs = L"Eliminar";
const wchar_t* txtGradeDashEn = L" | Grade: -";
const wchar_t* txtGradeDashEs = L" | Nota: -";
const wchar_t* txtNoSubjectsEn = L"Add at least one subject before saving a set.";
const wchar_t* txtNoSubjectsEs = L"Agrega al menos una asignatura antes de guardar un set.";
const wchar_t* txtWarnTitleEn = L"PAPA Calculator";
const wchar_t* txtWarnTitleEs = L"Calculadora PAPA";
const wchar_t* txtPromptTitleEn = L"Save Set";
const wchar_t* txtPromptTitleEs = L"Guardar Set";
const wchar_t* txtPromptLabelEn = L"Set name:";
const wchar_t* txtPromptLabelEs = L"Nombre del set:";
const wchar_t* txtOkEn = L"OK";
const wchar_t* txtOkEs = L"Aceptar";
const wchar_t* txtCancelEn = L"Cancel";
const wchar_t* txtCancelEs = L"Cancelar";

// Layout rectangles (kept as constants so drawing code and control
// creation code always agree with each other)
namespace Layout {
    // Input frame
    const RECT inputFrame = { 15, 55, 505, 155 };
    // List frame
    const RECT listFrame = { 15, 165, 505, 345 };
    // Result frame
    const RECT resultFrame = { 15, 440, 505, 500 };
    // Saved sets side panel (right column)
    const RECT setsPanelFrame = { 525, 55, 745, 500 };
}

// ---------------------------------------------------------------------
// Small hand-rolled modal prompt (no .rc resources needed) used to ask
// the user for a name when saving a set.
// ---------------------------------------------------------------------
static std::wstring g_promptResult;
static bool g_promptOk = false;
static HWND g_promptEdit = NULL;

LRESULT CALLBACK PromptWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        HWND hLbl = CreateWindowW(L"STATIC", isEnglish ? txtPromptLabelEn : txtPromptLabelEs,
            WS_CHILD | WS_VISIBLE, 15, 15, 250, 20, hWnd, NULL, hInst, NULL);
        g_promptEdit = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER,
            15, 40, 250, 25, hWnd, (HMENU)1, hInst, NULL);
        HWND hOk = CreateWindowW(L"BUTTON", isEnglish ? txtOkEn : txtOkEs,
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 15, 80, 115, 32, hWnd, (HMENU)2, hInst, NULL);
        HWND hCancel = CreateWindowW(L"BUTTON", isEnglish ? txtCancelEn : txtCancelEs,
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 80, 115, 32, hWnd, (HMENU)3, hInst, NULL);
        SendMessage(hLbl, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(g_promptEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hOk, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hCancel, WM_SETFONT, (WPARAM)hFont, TRUE);
        SetFocus(g_promptEdit);
        break;
    }
    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == 2) { // OK
            wchar_t buf[100] = { 0 };
            GetWindowTextW(g_promptEdit, buf, 100);
            g_promptResult = buf;
            g_promptOk = (wcslen(buf) > 0);
            DestroyWindow(hWnd);
        }
        else if (id == 3) { // Cancel
            g_promptOk = false;
            DestroyWindow(hWnd);
        }
        break;
    }
    case WM_CLOSE:
        g_promptOk = false;
        DestroyWindow(hWnd);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

bool PromptForSetName(HWND parent, std::wstring& outName) {
    static bool classRegistered = false;
    const wchar_t* CLASS_NAME = L"SaveSetPromptClass";
    if (!classRegistered) {
        WNDCLASSW wc = {};
        wc.lpfnWndProc = PromptWndProc;
        wc.hInstance = hInst;
        wc.lpszClassName = CLASS_NAME;
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClassW(&wc);
        classRegistered = true;
    }

    g_promptOk = false;
    g_promptResult.clear();

    RECT rcParent;
    GetWindowRect(parent, &rcParent);
    int w = 300, h = 155;
    int x = rcParent.left + ((rcParent.right - rcParent.left) - w) / 2;
    int y = rcParent.top + ((rcParent.bottom - rcParent.top) - h) / 2;

    DWORD style = WS_POPUP | WS_CAPTION | WS_SYSMENU;
    RECT rect = { 0, 0, w, h };
    AdjustWindowRect(&rect, style, FALSE);

    HWND hPrompt = CreateWindowW(CLASS_NAME, isEnglish ? txtPromptTitleEn : txtPromptTitleEs,
        style, x, y, rect.right - rect.left, rect.bottom - rect.top,
        parent, NULL, hInst, NULL);

    if (!hPrompt) return false;

    EnableWindow(parent, FALSE);
    ShowWindow(hPrompt, SW_SHOW);
    UpdateWindow(hPrompt);

    MSG msg;
    while (IsWindow(hPrompt) && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    EnableWindow(parent, TRUE);
    SetForegroundWindow(parent);

    if (g_promptOk) {
        outName = g_promptResult;
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------

std::wstring FormatSubjectLine(const Subject& s) {
    std::wstringstream ss;
    ss << s.name << L" | " << s.credits << (isEnglish ? L" credits" : L" cr\u00E9ditos");
    if (s.hasGrade) {
        ss << (isEnglish ? L" | Grade: " : L" | Nota: ") << s.grade;
    }
    else {
        ss << (isEnglish ? txtGradeDashEn : txtGradeDashEs);
    }
    return ss.str();
}

void RefreshListBox() {
    SendMessageW(hList, LB_RESETCONTENT, 0, 0);
    for (const auto& s : subjects) {
        std::wstring line = FormatSubjectLine(s);
        SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)line.c_str());
    }
}

void RefreshSetsListBox() {
    SendMessageW(hListSets, LB_RESETCONTENT, 0, 0);
    for (const auto& set : savedSets) {
        SendMessageW(hListSets, LB_ADDSTRING, 0, (LPARAM)set.name.c_str());
    }
}

void UpdateUI(HWND hWnd) {
    SetWindowTextW(hWnd, isEnglish ? txtTitleEn : txtTitleEs);
    SetWindowTextW(hLblTitle, isEnglish ? txtTitleEn : txtTitleEs);
    SetWindowTextW(hToggleLang, isEnglish ? txtToggleEn : txtToggleEs);
    SetWindowTextW(hLblSubj, isEnglish ? txtSubjEn : txtSubjEs);
    SetWindowTextW(hLblCred, isEnglish ? txtCredEn : txtCredEs);
    SetWindowTextW(hLblGrad, isEnglish ? txtGradEn : txtGradEs);
    SetWindowTextW(hBtnAdd, isEnglish ? txtAddEn : txtAddEs);
    SetWindowTextW(hBtnCalc, isEnglish ? txtCalcEn : txtCalcEs);
    SetWindowTextW(hBtnClear, isEnglish ? txtClearEn : txtClearEs);
    SetWindowTextW(hBtnDelete, isEnglish ? txtDeleteEn : txtDeleteEs);
    SetWindowTextW(hBtnSaveSet, isEnglish ? txtSaveSetEn : txtSaveSetEs);
    SetWindowTextW(hLblSetsHeader, isEnglish ? txtSetsHeaderEn : txtSetsHeaderEs);
    SetWindowTextW(hBtnLoadSet, isEnglish ? txtLoadSetEn : txtLoadSetEs);
    SetWindowTextW(hBtnDeleteSet, isEnglish ? txtDeleteSetEn : txtDeleteSetEs);
    SetWindowTextW(hToggleSets, showSetsPanel
        ? (isEnglish ? txtSetsExpandedEn : txtSetsExpandedEs)
        : (isEnglish ? txtSetsCollapsedEn : txtSetsCollapsedEs));

    RefreshListBox();
    RefreshSetsListBox();

    // Force a full, clean repaint of the window. Combined with the
    // WM_CTLCOLORSTATIC fix below, this guarantees no leftover text
    // fragments remain when a control's caption changes length
    // (e.g. "Grade:" -> "Calificación:" and back).
    InvalidateRect(hWnd, NULL, TRUE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
        hFontLarge = CreateFontW(26, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
        hFontTitle = CreateFontW(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

        // Title
        hLblTitle = CreateWindowW(L"STATIC", txtTitleEn, WS_CHILD | WS_VISIBLE, 20, 15, 340, 30, hWnd, NULL, hInst, NULL);

        // Lang Toggle
        hToggleLang = CreateWindowW(L"BUTTON", txtToggleEn, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            440, 15, 50, 30, hWnd, (HMENU)ID_TOGGLE_LANG, hInst, NULL);

        // Saved-sets panel toggle (top right)
        hToggleSets = CreateWindowW(L"BUTTON", txtSetsCollapsedEn, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            610, 15, 130, 30, hWnd, (HMENU)ID_TOGGLE_SETS, hInst, NULL);

        // Input Area (widened so Spanish captions are not clipped)
        hLblSubj = CreateWindowW(L"STATIC", txtSubjEn, WS_CHILD | WS_VISIBLE, 25, 65, 175, 20, hWnd, NULL, hInst, NULL);
        hEditSubj = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 25, 87, 175, 25, hWnd, (HMENU)ID_EDIT_SUBJ, hInst, NULL);
        hLblCred = CreateWindowW(L"STATIC", txtCredEn, WS_CHILD | WS_VISIBLE, 210, 65, 100, 20, hWnd, NULL, hInst, NULL);
        hEditCred = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 210, 87, 90, 25, hWnd, (HMENU)ID_EDIT_CRED, hInst, NULL);
        hLblGrad = CreateWindowW(L"STATIC", txtGradEn, WS_CHILD | WS_VISIBLE, 315, 65, 165, 20, hWnd, NULL, hInst, NULL);
        hEditGrad = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 315, 87, 90, 25, hWnd, (HMENU)ID_EDIT_GRAD, hInst, NULL);
        hBtnAdd = CreateWindowW(L"BUTTON", txtAddEn, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 415, 85, 75, 30, hWnd, (HMENU)ID_BTN_ADD, hInst, NULL);

        // List View
        hList = CreateWindowW(L"LISTBOX", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
            25, 175, 465, 160, hWnd, (HMENU)ID_LIST_VIEW, hInst, NULL);

        // Save Set, Calculate, Clear, Delete (four evenly spaced buttons)
        hBtnSaveSet = CreateWindowW(L"BUTTON", txtSaveSetEn, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 25, 350, 112, 40, hWnd, (HMENU)ID_BTN_SAVE_SET, hInst, NULL);
        hBtnCalc = CreateWindowW(L"BUTTON", txtCalcEn, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 147, 350, 112, 40, hWnd, (HMENU)ID_BTN_CALC, hInst, NULL);
        hBtnClear = CreateWindowW(L"BUTTON", txtClearEn, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 269, 350, 112, 40, hWnd, (HMENU)ID_BTN_CLEAR, hInst, NULL);
        hBtnDelete = CreateWindowW(L"BUTTON", txtDeleteEn, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 391, 350, 112, 40, hWnd, (HMENU)ID_BTN_DELETE, hInst, NULL);

        // Result
        hLblResult = CreateWindowW(L"STATIC", txtResEn, WS_CHILD | WS_VISIBLE, 25, 450, 350, 35, hWnd, NULL, hInst, NULL);

        // Credit
        hLblCredit = CreateWindowW(L"STATIC", txtCredit, WS_CHILD | WS_VISIBLE | SS_RIGHT, 340, 505, 165, 20, hWnd, NULL, hInst, NULL);

        // Saved-sets side panel (right column) — hidden until toggled on
        hLblSetsHeader = CreateWindowW(L"STATIC", txtSetsHeaderEn, WS_CHILD | SS_LEFT, 535, 65, 200, 20, hWnd, NULL, hInst, NULL);
        hListSets = CreateWindowW(L"LISTBOX", NULL, WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
            535, 90, 200, 300, hWnd, (HMENU)ID_LIST_SETS, hInst, NULL);
        hBtnLoadSet = CreateWindowW(L"BUTTON", txtLoadSetEn, WS_CHILD | BS_PUSHBUTTON, 535, 400, 95, 35, hWnd, (HMENU)ID_BTN_LOAD_SET, hInst, NULL);
        hBtnDeleteSet = CreateWindowW(L"BUTTON", txtDeleteSetEn, WS_CHILD | BS_PUSHBUTTON, 640, 400, 95, 35, hWnd, (HMENU)ID_BTN_DELETE_SET, hInst, NULL);

        // Set Fonts
        SendMessage(hLblTitle, WM_SETFONT, (WPARAM)hFontTitle, TRUE);
        SendMessage(hToggleLang, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hToggleSets, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hLblSubj, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hEditSubj, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hLblCred, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hEditCred, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hLblGrad, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hEditGrad, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hBtnAdd, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hList, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hBtnSaveSet, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hBtnCalc, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hBtnClear, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hBtnDelete, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hLblResult, WM_SETFONT, (WPARAM)hFontLarge, TRUE);
        SendMessage(hLblCredit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hLblSetsHeader, WM_SETFONT, (WPARAM)hFontTitle, TRUE);
        SendMessage(hListSets, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hBtnLoadSet, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hBtnDeleteSet, WM_SETFONT, (WPARAM)hFont, TRUE);
        break;
    }
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case ID_TOGGLE_LANG:
            isEnglish = !isEnglish;
            UpdateUI(hWnd);
            break;
        case ID_TOGGLE_SETS: {
            showSetsPanel = !showSetsPanel;
            int showCmd = showSetsPanel ? SW_SHOW : SW_HIDE;
            ShowWindow(hLblSetsHeader, showCmd);
            ShowWindow(hListSets, showCmd);
            ShowWindow(hBtnLoadSet, showCmd);
            ShowWindow(hBtnDeleteSet, showCmd);
            SetWindowTextW(hToggleSets, showSetsPanel
                ? (isEnglish ? txtSetsExpandedEn : txtSetsExpandedEs)
                : (isEnglish ? txtSetsCollapsedEn : txtSetsCollapsedEs));
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        }
        case ID_BTN_ADD: {
            wchar_t wName[100], wCred[20], wGrad[20];
            GetWindowTextW(hEditSubj, wName, 100);
            GetWindowTextW(hEditCred, wCred, 20);
            GetWindowTextW(hEditGrad, wGrad, 20);
            // Grade is optional: only Name and Credits are required.
            if (wcslen(wName) > 0 && wcslen(wCred) > 0) {
                Subject s;
                s.name = wName;

                // Replace comma with dot for float parsing just in case
                std::wstring sCred = wCred;
                size_t pos;
                while ((pos = sCred.find(L",")) != std::wstring::npos) sCred.replace(pos, 1, L".");
                s.credits = _wtof(sCred.c_str());

                s.hasGrade = (wcslen(wGrad) > 0);
                if (s.hasGrade) {
                    std::wstring sGrad = wGrad;
                    while ((pos = sGrad.find(L",")) != std::wstring::npos) sGrad.replace(pos, 1, L".");
                    s.grade = _wtof(sGrad.c_str());
                }
                else {
                    s.grade = 0.0;
                }

                subjects.push_back(s);
                RefreshListBox();
                SetWindowTextW(hEditSubj, L"");
                SetWindowTextW(hEditCred, L"");
                SetWindowTextW(hEditGrad, L"");
                SetFocus(hEditSubj);
            }
            break;
        }
        case ID_BTN_DELETE: {
            int sel = (int)SendMessageW(hList, LB_GETCURSEL, 0, 0);
            if (sel != LB_ERR && sel >= 0 && sel < (int)subjects.size()) {
                subjects.erase(subjects.begin() + sel);
                RefreshListBox();
                // Keep a sensible selection near the one that was removed
                int newCount = (int)subjects.size();
                if (newCount > 0) {
                    int newSel = (sel < newCount) ? sel : newCount - 1;
                    SendMessageW(hList, LB_SETCURSEL, newSel, 0);
                }
            }
            break;
        }
        case ID_BTN_CLEAR:
            subjects.clear();
            SendMessageW(hList, LB_RESETCONTENT, 0, 0);
            SetWindowTextW(hLblResult, isEnglish ? L"PAPA: 0.00" : L"PAPA: 0.00");
            break;
        case ID_BTN_CALC: {
            double sumProd = 0;
            double sumCred = 0;
            for (const auto& s : subjects) {
                if (s.hasGrade) {
                    sumProd += (s.credits * s.grade);
                    sumCred += s.credits;
                }
            }

            std::wstringstream ss;
            if (sumCred > 0) {
                double papa = sumProd / sumCred;
                ss << L"PAPA: " << std::fixed << std::setprecision(2) << papa;
            }
            else {
                ss << L"PAPA: 0.00";
            }
            SetWindowTextW(hLblResult, ss.str().c_str());
            break;
        }
        case ID_BTN_SAVE_SET: {
            if (subjects.empty()) {
                MessageBoxW(hWnd, isEnglish ? txtNoSubjectsEn : txtNoSubjectsEs,
                    isEnglish ? txtWarnTitleEn : txtWarnTitleEs, MB_OK | MB_ICONINFORMATION);
                break;
            }
            std::wstring setName;
            if (PromptForSetName(hWnd, setName)) {
                SavedSet set;
                set.name = setName;
                set.subjects = subjects; // snapshot copy, including per-subject grade optionality
                savedSets.push_back(set);
                RefreshSetsListBox();
            }
            break;
        }
        case ID_BTN_LOAD_SET: {
            int sel = (int)SendMessageW(hListSets, LB_GETCURSEL, 0, 0);
            if (sel != LB_ERR && sel >= 0 && sel < (int)savedSets.size()) {
                subjects = savedSets[sel].subjects;
                RefreshListBox();
            }
            break;
        }
        case ID_BTN_DELETE_SET: {
            int sel = (int)SendMessageW(hListSets, LB_GETCURSEL, 0, 0);
            if (sel != LB_ERR && sel >= 0 && sel < (int)savedSets.size()) {
                savedSets.erase(savedSets.begin() + sel);
                RefreshSetsListBox();
            }
            break;
        }
        }
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        // Draw colored frames around the logical sections of the UI
        HPEN hPen = CreatePen(PS_SOLID, 2, COLOR_ACCENT);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

        Rectangle(hdc, Layout::inputFrame.left, Layout::inputFrame.top, Layout::inputFrame.right, Layout::inputFrame.bottom);
        Rectangle(hdc, Layout::listFrame.left, Layout::listFrame.top, Layout::listFrame.right, Layout::listFrame.bottom);
        Rectangle(hdc, Layout::resultFrame.left, Layout::resultFrame.top, Layout::resultFrame.right, Layout::resultFrame.bottom);
        if (showSetsPanel) {
            Rectangle(hdc, Layout::setsPanelFrame.left, Layout::setsPanelFrame.top, Layout::setsPanelFrame.right, Layout::setsPanelFrame.bottom);
        }

        SelectObject(hdc, hOldBrush);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;
        HWND hCtl = (HWND)lParam;

        SetBkColor(hdcStatic, GetSysColor(COLOR_WINDOW));

        if (hCtl == hLblTitle) {
            SetTextColor(hdcStatic, COLOR_TITLE_TEXT);
        }
        else if (hCtl == hLblResult) {
            SetTextColor(hdcStatic, COLOR_RESULT_TEXT);
        }
        else if (hCtl == hLblSetsHeader) {
            SetTextColor(hdcStatic, COLOR_TITLE_TEXT);
        }
        else {
            SetTextColor(hdcStatic, GetSysColor(COLOR_WINDOWTEXT));
        }

        // IMPORTANT: returning a real background brush (instead of
        // NULL_BRUSH) tells Windows to erase the static control's
        // background before repainting it. That is what caused the
        // earlier language-switch bug: when a caption changed length,
        // leftover characters from the previous text used to persist
        // on screen no matter which language you switched to afterward.
        return (INT_PTR)GetSysColorBrush(COLOR_WINDOW);
    }
    case WM_DESTROY:
        DeleteObject(hFont);
        DeleteObject(hFontLarge);
        DeleteObject(hFontTitle);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;
    const wchar_t CLASS_NAME[] = L"PAPA_Calculator_Class";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassW(&wc);

    // Fixed size window - widened to fit the new saved-sets side panel
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT rect = { 0, 0, 760, 560 };
    AdjustWindowRect(&rect, style, FALSE);
    HWND hWnd = CreateWindowExW(
        0, CLASS_NAME, txtTitleEn, style,
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, hInstance, NULL
    );
    if (hWnd == NULL) {
        return 0;
    }
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}