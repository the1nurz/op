#include <windows.h>
#include <cassert>
#include <iostream>
int notices=0;
int TestMessageBox(HWND,LPCWSTR,LPCWSTR,UINT) { ++notices; return IDOK; }
#define MessageBoxW TestMessageBox
#include "../main.cpp"
#undef MessageBoxW
struct Probe : Shape {
    static int destroyed;
    static int drawn;
    Probe():Shape(0,0){}
    ~Probe() override { ++destroyed; }
    void Draw(HDC,bool=false) const override { ++drawn; }
};
int Probe::destroyed=0;
int Probe::drawn=0;
int main() {
    pcshape=new Shape*[N]{};
    WNDCLASSW wc{}; wc.lpfnWndProc=WindowProc;
    wc.hInstance=GetModuleHandleW(nullptr); wc.lpszClassName=L"Lab2Test";
    assert(RegisterClassW(&wc));
    HWND w=CreateWindowW(wc.lpszClassName,L"",WS_OVERLAPPEDWINDOW,0,0,900,600,
        nullptr,CreateEditorMenu(),wc.hInstance,nullptr);
    assert(w);
    assert(GetMenuItemCount(GetMenu(w))==3);
    for(int t=10;t<=13;++t) {
        SendMessageW(w,WM_COMMAND,t,0);
        wchar_t title[100]; GetWindowTextW(w,title,100);
        assert(wcsstr(title,names[t-10]));
        SendMessageW(w,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(200,200));
        SendMessageW(w,WM_MOUSEMOVE,MK_LBUTTON,MAKELPARAM(250,240));
        assert(draft);
        SendMessageW(w,WM_LBUTTONUP,0,MAKELPARAM(250,240));
        assert(count==t-9 && !draft);
    }
    HDC dc=CreateCompatibleDC(nullptr);
    HBITMAP bmp=CreateBitmap(900,600,1,32,nullptr);
    auto old=SelectObject(dc,bmp);
    RECT r{0,0,900,600}; FillRect(dc,&r,static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
    pcshape[2]->Draw(dc);
    assert(GetPixel(dc,200,200)==RGB(255,192,203));
    assert(GetPixel(dc,151,201)==RGB(255,192,203));
    FillRect(dc,&r,static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
    pcshape[3]->Draw(dc);
    assert(GetPixel(dc,225,220)==RGB(255,255,255));
    pcshape[2]->Draw(dc,true);
    assert(GetPixel(dc,200,200)==RGB(255,255,255));
    int black=0,white=0;
    for(int x=160;x<240;++x) {
        auto c=GetPixel(dc,x,160);
        if(c==RGB(0,0,0)) ++black;
        if(c==RGB(255,255,255)) ++white;
    }
    assert(black>0 && white>0);
    SendMessageW(w,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(20,20));
    SendMessageW(w,WM_KEYDOWN,VK_ESCAPE,0);
    assert(!draft && count==4);
    for(int i=count;i<N;++i) {
        SendMessageW(w,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(30,30));
        SendMessageW(w,WM_LBUTTONUP,0,MAKELPARAM(50,50));
    }
    assert(count==N);
    SendMessageW(w,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(30,30));
    assert(count==N && !draft && notices==1);
    // Перевіряємо виклик Draw саме обробником WM_PAINT.
    delete pcshape[N-1];
    pcshape[N-1] = new Probe;
    SendMessageW(w, WM_PAINT, 0, 0);
    assert(count==N && Probe::drawn==1);
    Shape* probe=new Probe; delete probe;
    assert(Probe::destroyed==1);
    SelectObject(dc,old); DeleteObject(bmp); DeleteDC(dc);
    SendMessageW(w,WM_COMMAND,1,0);
    assert(!IsWindow(w) && !pcshape && Probe::destroyed==2);
    std::cout<<"PASS: four shapes, menu/title, mouse messages, rectangle fill/center, ellipse transparency, dotted preview, Esc, 127 limit, WM_PAINT virtual dispatch, virtual destructor, cleanup and exit\n";
}
