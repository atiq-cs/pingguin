// My functions

BOOL MyTaskBarAddIcon(HWND hwnd, UINT uID, UINT CallbackMessageID, HICON hicon, LPTSTR lpszTip);
//BOOL SAShowBalloonTip(HINSTANCE hInst, HWND hWnd, UINT uID, LPCTSTR pszTitle, LPCTSTR pszText, HICON hicon);
BOOL SAShowBalloonTip(HINSTANCE hInst, HWND hWnd, UINT uID, LPCTSTR pszTitle, LPCTSTR pszText, /*HICON hicon*/DWORD dwIcon);
BOOL SATaskBarDelIcon(HWND hwnd, UINT uID);
BOOL IsWin7OrLater();