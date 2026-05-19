#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <cstdio>
#include <cstring>

/* ── Konstantи ──────────────────────────────── */
#define SCREEN_W        1000
#define SCREEN_H         600

#define MAX_TEXT       65536
#define MAX_PATH         512
#define MAX_STATUS       256

#define TOOLBAR_H         42
#define STATUSBAR_H       24
#define PADDING           10
#define FONT_SZ           19
#define FONT_SP            1
#define LINE_GAP           5

/* ── Dijalog mod ────────────────────────────── */
enum DialogMode { DLG_NONE = 0, DLG_OPEN, DLG_SAVE_AS };



static int MinI(int a, int b) { return a < b ? a : b; }
static int MaxI(int a, int b) { return a > b ? a : b; }
static int ClampI(int v, int lo, int hi) { return v < lo ? lo : v > hi ? hi : v; }

/* Korisnicko zadrzuvanje na kopce (repeat) */
static bool KeyHeld(int key)
{
    const double DELAY = 0.30;
    const double RATE  = 0.042;
    static double next[512] = {};
    if (key < 0 || key >= 512) return IsKeyPressed(key);
    double now = GetTime();
    if (IsKeyPressed(key))        { next[key] = now + DELAY; return true; }
    if (IsKeyDown(key) && now >= next[key]) { next[key] = now + RATE;  return true; }
    if (!IsKeyDown(key))          { next[key] = 0; }
    return false;
}

/* ── Pozicija na kursor vo tekst bafer ── */
static int  LineStart(const char *t, int pos)
{
    while (pos > 0 && t[pos-1] != '\n') pos--;
    return pos;
}
static int  LineEnd(const char *t, int len, int pos)
{
    while (pos < len && t[pos] != '\n') pos++;
    return pos;
}
static void CursorRowCol(const char *t, int cur, int *row, int *col)
{
    *row = *col = 0;
    for (int i = 0; i < cur; i++)
        if (t[i] == '\n') { (*row)++; *col = 0; }
        else (*col)++;
}
static int CountLines(const char *t, int len)
{
    int n = 1;
    for (int i = 0; i < len; i++) if (t[i] == '\n') n++;
    return n;
}

/* ── Kretanje na kursor gore/dolu ── */
static void MoveUp(const char *t, int len, int *cur)
{
    (void)len;
    int cs = LineStart(t, *cur);
    int wc = *cur - cs;
    if (cs == 0) return;
    int pe = cs - 1;
    int ps = LineStart(t, pe);
    *cur = ps + MinI(wc, pe - ps);
}
static void MoveDown(const char *t, int len, int *cur)
{
    int cs = LineStart(t, *cur);
    int ce = LineEnd(t, len, *cur);
    int wc = *cur - cs;
    if (ce >= len) return;
    int ns = ce + 1;
    int ne = LineEnd(t, len, ns);
    *cur = ns + MinI(wc, ne - ns);
}

/* ── Insert / brishenje ── */
static void InsertStr(char *t, int *len, int *cur, const char *s)
{
    int sl = (int)strlen(s);
    if (sl <= 0 || *len + sl >= MAX_TEXT) return;
    memmove(t + *cur + sl, t + *cur, (*len - *cur) + 1);
    memcpy(t + *cur, s, sl);
    *cur += sl;
    *len += sl;
}
static void InsertCh(char *t, int *len, int *cur, char c)
{
    char tmp[2] = { c, '\0' };
    InsertStr(t, len, cur, tmp);
}
static void Backspace(char *t, int *len, int *cur)
{
    if (*cur <= 0) return;
    memmove(t + *cur - 1, t + *cur, (*len - *cur) + 1);
    (*cur)--; (*len)--;
}
static void Delete(char *t, int *len, int cur)
{
    if (cur >= *len) return;
    memmove(t + cur, t + cur + 1, (*len - cur));
    (*len)--;
}
static void ClearBuf(char *t, int *len, int *cur)
{
    t[0] = '\0'; *len = 0; *cur = 0;
}

/* ── Save / Open ── */
static bool SaveFile(const char *path, char *t, char *status)
{
    if (!path || !path[0]) { snprintf(status, MAX_STATUS, "Nema ime na fajl!"); return false; }
    if (SaveFileText(path, t)) { snprintf(status, MAX_STATUS, "Snimeno: %s", path); return true; }
    snprintf(status, MAX_STATUS, "Greska pri snimanje: %s", path);
    return false;
}
static bool OpenFile(const char *path, char *t, int *len, int *cur, char *status)
{
    if (!path || !path[0]) { snprintf(status, MAX_STATUS, "Nema pat!"); return false; }
    char *loaded = LoadFileText(path);
    if (!loaded) { snprintf(status, MAX_STATUS, "Greska pri otvoranje: %s", path); return false; }
    strncpy(t, loaded, MAX_TEXT - 1); t[MAX_TEXT-1] = '\0';
    *len = (int)strlen(t); *cur = 0;
    UnloadFileText(loaded);
    snprintf(status, MAX_STATUS, "Otvoreno: %s", path);
    return true;
}

/* ═══════════════════════════════════════════════
   MAIN
   ═══════════════════════════════════════════════ */
int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_W, SCREEN_H, "Mini Notepad  —  raylib + raygui");
    SetExitKey(0);
    SetTargetFPS(60);

    /* ── Stanje na editorot ── */
    char   text[MAX_TEXT]   = {};
    int    textLen          = 0;
    int    cursor           = 0;

    char   curFile[MAX_PATH]   = {};
    char   pathInput[MAX_PATH] = {};
    char   status[MAX_STATUS]  = "Ctrl+N Nov  |  Ctrl+O Otvori  |  Ctrl+S Snimi";

    int        scrollLine = 0;
    DialogMode dlgMode    = DLG_NONE;

    Font font       = GetFontDefault();
    int  lineHeight = FONT_SZ + LINE_GAP;

    /* ════════════════ GLAVNA PETLJA ════════════════ */
    while (!WindowShouldClose())
    {
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();

        Rectangle topBar  = { 0,   0,                          (float)sw, (float)TOOLBAR_H   };
        Rectangle statBar = { 0,   (float)(sh - STATUSBAR_H),  (float)sw, (float)STATUSBAR_H };
        Rectangle edRect  = { (float)PADDING,
                               (float)(TOOLBAR_H + PADDING),
                               (float)(sw - PADDING*2),
                               (float)(sh - TOOLBAR_H - STATUSBAR_H - PADDING*2) };

        int visLines = MaxI(1, (int)((edRect.height - PADDING*2) / lineHeight));

        bool ctrl  = IsKeyDown(KEY_LEFT_CONTROL)  || IsKeyDown(KEY_RIGHT_CONTROL);
        bool shift = IsKeyDown(KEY_LEFT_SHIFT)    || IsKeyDown(KEY_RIGHT_SHIFT);

        /* ── Tastatura (samo koga nema dijalog) ── */
        if (dlgMode == DLG_NONE)
        {
            /* Shortcuts */
            if (ctrl && IsKeyPressed(KEY_N))
            {
                ClearBuf(text, &textLen, &cursor);
                curFile[0] = '\0'; scrollLine = 0;
                snprintf(status, MAX_STATUS, "Nov prazen dokument.");
            }
            if (ctrl && IsKeyPressed(KEY_O))
            {
                pathInput[0] = '\0';
                dlgMode = DLG_OPEN;
            }
            if (ctrl && shift && IsKeyPressed(KEY_S))
            {
                pathInput[0] = '\0';
                dlgMode = DLG_SAVE_AS;
            }
            else if (ctrl && IsKeyPressed(KEY_S))
            {
                if (!curFile[0]) { pathInput[0] = '\0'; dlgMode = DLG_SAVE_AS; }
                else SaveFile(curFile, text, status);
            }

            /* Strelki */
            if (KeyHeld(KEY_LEFT))  cursor = MaxI(0, cursor - 1);
            if (KeyHeld(KEY_RIGHT)) cursor = MinI(textLen, cursor + 1);
            if (KeyHeld(KEY_UP))    MoveUp(text, textLen, &cursor);
            if (KeyHeld(KEY_DOWN))  MoveDown(text, textLen, &cursor);

            if (IsKeyPressed(KEY_HOME)) cursor = LineStart(text, cursor);
            if (IsKeyPressed(KEY_END))  cursor = LineEnd(text, textLen, cursor);

            /* PageUp / PageDown */
            if (IsKeyPressed(KEY_PAGE_UP))
            {
                for (int i = 0; i < visLines; i++) MoveUp(text, textLen, &cursor);
            }
            if (IsKeyPressed(KEY_PAGE_DOWN))
            {
                for (int i = 0; i < visLines; i++) MoveDown(text, textLen, &cursor);
            }

            /* Brishenje */
            if (KeyHeld(KEY_BACKSPACE)) Backspace(text, &textLen, &cursor);
            if (KeyHeld(KEY_DELETE))    Delete(text, &textLen, cursor);

            /* Enter, Tab */
            if (KeyHeld(KEY_ENTER) || KeyHeld(KEY_KP_ENTER))
                InsertCh(text, &textLen, &cursor, '\n');
            if (IsKeyPressed(KEY_TAB))
                InsertStr(text, &textLen, &cursor, "    ");

            /* Oobicni znaci */
            for (int ch = GetCharPressed(); ch > 0; ch = GetCharPressed())
                if (!ctrl && ch >= 32 && ch <= 126)
                    InsertCh(text, &textLen, &cursor, (char)ch);

            /* Trkalo na mish */
            if (CheckCollisionPointRec(GetMousePosition(), edRect))
            {
                int w = -(int)GetMouseWheelMove();
                if (w) scrollLine += w * 3;
            }
        }

        /* ── Kursor vo granici + scroll ── */
        cursor = ClampI(cursor, 0, textLen);

        int curRow, curCol;
        CursorRowCol(text, cursor, &curRow, &curCol);

        if (curRow < scrollLine)              scrollLine = curRow;
        if (curRow >= scrollLine + visLines)  scrollLine = curRow - visLines + 1;

        int totalLines = CountLines(text, textLen);
        scrollLine = ClampI(scrollLine, 0, MaxI(0, totalLines - 1));

        /* ════════════ CRTANJE ════════════ */
        BeginDrawing();
        ClearBackground({ 245, 245, 245, 255 });

        /* ── Toolbar ── */
        DrawRectangleRec(topBar, { 230, 230, 235, 255 });
        DrawLineEx({ 0, (float)TOOLBAR_H }, { (float)sw, (float)TOOLBAR_H }, 1, { 180, 180, 185, 255 });

        if (GuiButton({ 8,   7, 72, 28 }, "Novo"))    { ClearBuf(text,&textLen,&cursor); curFile[0]='\0'; scrollLine=0; snprintf(status,MAX_STATUS,"Nov dokument."); }
        if (GuiButton({ 88,  7, 72, 28 }, "Otvori"))  { pathInput[0]='\0'; dlgMode=DLG_OPEN; }
        if (GuiButton({ 168, 7, 72, 28 }, "Snimi"))
        {
            if (!curFile[0]) { pathInput[0]='\0'; dlgMode=DLG_SAVE_AS; }
            else SaveFile(curFile, text, status);
        }
        if (GuiButton({ 248, 7, 90, 28 }, "Snimi kako")) { pathInput[0]='\0'; dlgMode=DLG_SAVE_AS; }

        /* Ime na fajlot vo toolbar */
        GuiLabel({ 350, 7, (float)(sw - 360), 28 },
                 curFile[0] ? curFile : "[ nov dokument ]");

        /* ── Editor pozadina ── */
        DrawRectangleRec(edRect, RAYWHITE);
        DrawRectangleLinesEx(edRect, 1, LIGHTGRAY);

        BeginScissorMode((int)edRect.x + PADDING, (int)edRect.y + PADDING,
                         (int)edRect.width - PADDING*2, (int)edRect.height - PADDING*2);

        /* Crtaj linii na tekst */
        int drawRow = 0, lineStart = 0;

        for (int i = 0; i <= textLen; i++)
        {
            if (i == textLen || text[i] == '\n')
            {
                if (drawRow >= scrollLine && drawRow < scrollLine + visLines)
                {
                    /* Oznaci tekockata linija */
                    if (drawRow == curRow)
                    {
                        float ly = edRect.y + PADDING + (drawRow - scrollLine)*lineHeight;
                        DrawRectangle((int)(edRect.x + PADDING), (int)ly,
                                      (int)(edRect.width - PADDING*2), lineHeight,
                                      { 232, 242, 255, 255 });
                    }

                    /* Broj na linija (gutter) */
                    char gutter[12];
                    snprintf(gutter, sizeof(gutter), "%d", drawRow + 1);
                    int gw = MeasureText(gutter, FONT_SZ - 3);
                    DrawText(gutter,
                             (int)(edRect.x + PADDING + 38 - gw),
                             (int)(edRect.y + PADDING + (drawRow - scrollLine)*lineHeight + 2),
                             FONT_SZ - 3, LIGHTGRAY);

                    /* Tekst na linijata */
                    int lineLen = i - lineStart;
                    if (lineLen > 0)
                    {
                        char buf[4096] = {};
                        int  copy = MinI(lineLen, (int)sizeof(buf) - 1);
                        memcpy(buf, text + lineStart, copy);

                        Vector2 pos = {
                            edRect.x + PADDING + 48,
                            edRect.y + PADDING + (drawRow - scrollLine)*lineHeight
                        };
                        DrawTextEx(font, buf, pos, FONT_SZ, FONT_SP, BLACK);
                    }
                }
                drawRow++;
                lineStart = i + 1;
            }
        }

        /* Kursor (blinkanje) */
        if (dlgMode == DLG_NONE)
        {
            bool show = ((int)(GetTime() * 2.0) % 2) == 0;
            if (show && curRow >= scrollLine && curRow < scrollLine + visLines)
            {
                int cs = LineStart(text, cursor);
                int pl = cursor - cs;
                char prefix[4096] = {};
                if (pl > 0) { memcpy(prefix, text + cs, MinI(pl, (int)sizeof(prefix)-1)); }

                Vector2 ps = MeasureTextEx(font, prefix, FONT_SZ, FONT_SP);
                float cx = edRect.x + PADDING + 48 + ps.x;
                float cy = edRect.y + PADDING + (curRow - scrollLine)*lineHeight;
                DrawRectangle((int)cx, (int)cy, 2, FONT_SZ, { 30, 100, 220, 255 });
            }
        }

        EndScissorMode();

        /* ── Status bar ── */
        char statusFull[512];
        snprintf(statusFull, sizeof(statusFull),
                 "  Red: %d   Kol: %d   |   Linii: %d   Znaci: %d   |   %s",
                 curRow + 1, curCol + 1, totalLines, textLen, status);
        GuiStatusBar(statBar, statusFull);

        /* ── Dijalog za open / save ── */
        if (dlgMode != DLG_NONE)
        {
            DrawRectangle(0, 0, sw, sh, Fade(RAYWHITE, 0.78f));

            const char *title   = (dlgMode == DLG_OPEN) ? "Otvori fajl" : "Snimi kako";
            const char *message = (dlgMode == DLG_OPEN) ? "Vnesete pat do fajlot (.txt):"
                                                         : "Vnesete pat za snimanje:";

            Rectangle dlgRect = {
                (float)sw/2 - 180,
                (float)sh/2 - 75,
                360, 150
            };

            int result = GuiTextInputBox(dlgRect, title, message, "OK;Otkazi",
                                         pathInput, MAX_PATH - 1, nullptr);

            if (result == 1)       /* OK */
            {
                if (dlgMode == DLG_OPEN)
                {
                    if (OpenFile(pathInput, text, &textLen, &cursor, status))
                    {
                        strncpy(curFile, pathInput, MAX_PATH - 1);
                        curFile[MAX_PATH-1] = '\0';
                        scrollLine = 0;
                    }
                }
                else
                {
                    if (SaveFile(pathInput, text, status))
                    {
                        strncpy(curFile, pathInput, MAX_PATH - 1);
                        curFile[MAX_PATH-1] = '\0';
                    }
                }
                pathInput[0] = '\0';
                dlgMode = DLG_NONE;
            }
            else if (result == 0 || result == 2)  /* X / Otkazi */
            {
                pathInput[0] = '\0';
                dlgMode = DLG_NONE;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}