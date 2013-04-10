#pragma once

#include <windows.h>
#include <vector>
#include <map>
#include <string>

#include "shape.h"
#include "system.h"
using namespace mj;

// ��״��
// ���������ͬwindow class name�� HWND������ͬһ message callback��
// �����ڴ���ǰ������ע������class name��
// ��Ϣ�������� CB(hwnd, msg, wparam, lparam);
// ��Ϣѭ����message dispatch loop
// 
//
// ���Ŀ��:
// 1, ����Win32 GUI��̵ĳ�ʼ����
// 2, �򻯶�η�񣬸�Ϊϵ��set����
// 3, ������ע�����أ�
// 4, ֧���Ӵ���
// 5, ���ڷ�װ
// 6, ������ͼ��װ �� GDI ������2Dͼ�λ����㷨
//  
// ʵ�� ��
// application: ����window class ע�ᣬ��Ϣѭ�����̣߳�������Ϣ������
// window : ��һ��HWND��������˽����Ϣ�����������ģʽ��������
// �Ӵ��� �� Ҳ��һ��window�����и��������ã�
// �ؼ����Ӵ���ʵ��
// ������Ϣ������������window���ã�����ַ���Ϣ������window
// let's all

// ���⣺ 
// ���д��ھ�����ͬwindow class����̫����
// ����ע�ᣬ�Ƿ�ɲ���app�����׶Σ��Ƿ������Ϣ�ж�̬ע��\
// ���ڵ�ICON��BRUSH��COLOR���������û�ָ���󣬺�ʱ���ã�WNDCLASS��WM_CREATE��
// ���� create����ʾǰ�ģ�����HWND SelectObject����
// 

// ������ʵ�֣����콫bitmap��window�������
class application
{
	// ��Ϊ���д��ڶ�������ͬ����Ϣ������ڣ�����WNDCLASS��ע�ᣬֻ��Ϊ������
	// �����ڵĹ���Ĭ�ϣ�
	WNDCLASS _default_wndclass;
	std::vector<WNDCLASS> _wndclasses;
public:
	static application &instance()
	{
		static application the_only_one;
		return the_only_one;
	}

	std::wstring get_default_class_name() { return _default_wndclass.lpszClassName; }
	int run()
	{
		for (std::vector<WNDCLASS>::iterator it = _wndclasses.begin(); it != _wndclasses.end(); it++) {
			RegisterClass(&(*it));
		}

		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return 0;
	}
private:
	static LRESULT CALLBACK app_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	application()
	{
		// init default wndclass: a empty wndclass, default icon, white brush, border frame,no anything
		_default_wndclass.style = CS_DBLCLKS;
		_default_wndclass.lpfnWndProc = app_proc;
		_default_wndclass.cbClsExtra = 0;
		_default_wndclass.cbWndExtra = 0;
		_default_wndclass.hInstance = GetModuleHandle(NULL);
		_default_wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		_default_wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		_default_wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		_default_wndclass.lpszMenuName = NULL;
		_default_wndclass.lpszClassName = TEXT("empty class");
		if (!RegisterClass(&_default_wndclass))
			MessageBox(NULL, strsyserror().c_str(), TEXT("ERROR"), 0);
	}
};

class base_window
{
	std::wstring _cls_name;
	std::wstring _wnd_name;
	unsigned long _ex_style;
	unsigned long _style;
	int _x;
	int _y;
	int _width;
	int _height;
	HMENU _menu;
	HINSTANCE _instance;
	HWND _hparent;
	HWND _hwnd;

	// attribute of window class
	const static int _ldblclk_timer_id = 3101;
	const static int _rdblclk_timer_id = 3103;
	//static  int _dblclk_elapse_ms = GetDoubleClickTime();
	UINT _buttondown;
	WPARAM _buttondown_wparam;
	LPARAM _buttondown_lparam;
	bool _dblclk_filtered;
public:
	base_window(HWND parent = 0);
	void set_position(const mj::point &p) { _x = p.x; _y = p.y; }
	void set_width(int width) { _width = width; } 
	void set_height(int height) { _height = height; }
	//void set_class_name(const std::wstring &cn) { _cls_name = cn; }
	void set_window_name(const std::wstring &wn) { _wnd_name = wn; }
	void set_extra_style(unsigned long style) {_ex_style = style; }
	void set_style(unsigned long style) { _style = style; }
	void set_parent(HWND parent) { _hparent = parent; }

	HWND get_hwnd() const { return _hwnd; }
	int get_width() {return _width;};
	int get_height() { return _height; }
	HINSTANCE get_instance() const { return _instance;}
	mj::point get_position() const { return mj::point(_x, _y); }
	
	int create()
	{
		_hwnd = CreateWindowEx(_ex_style,_cls_name.c_str(), _wnd_name.c_str(), _style, 
						_x, _y, _width, _height , _hparent, _menu, _instance, this);
		return _hwnd ? 0 : -1;
	}
	int show(int cmd = SW_SHOW) { ShowWindow(_hwnd, cmd);  return   UpdateWindow(_hwnd);}
	bool common_wnd_proc(UINT msg, WPARAM wparam, LPARAM lparam)
	{
		bool left = false;
		switch(msg) {
		case WM_LBUTTONDOWN:
			left = true;
		case WM_RBUTTONDOWN:
			_buttondown = msg;
			_buttondown_wparam = wparam;
			_buttondown_lparam = lparam;
			if (!_dblclk_filtered) {
				_dblclk_filtered = true;
				if (left)
					SetTimer(get_hwnd(), _ldblclk_timer_id, GetDoubleClickTime(), NULL);
				else 
					SetTimer(get_hwnd(), _rdblclk_timer_id, GetDoubleClickTime(), NULL);
				return true;
			} else {
				_dblclk_filtered = true;
			}
			break;
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
			_dblclk_filtered = false;
			break;
		case WM_TIMER:
			switch(wparam) {
			case _ldblclk_timer_id:
				if (_dblclk_filtered) {
					PostMessage(get_hwnd(), WM_LBUTTONDOWN, _buttondown_wparam, _buttondown_lparam);
				}
				KillTimer(get_hwnd(), wparam);
				return true;
			case _rdblclk_timer_id:
				if (_dblclk_filtered) {
					PostMessage(get_hwnd(), WM_RBUTTONDOWN, _buttondown_lparam, _buttondown_lparam);
				}
				KillTimer(get_hwnd(), wparam);
				return true;
			}
		}
		if (wnd_proc(msg, wparam, lparam))
			return true;
		switch (msg) {
		case WM_DESTROY:
			//MessageBox(get_hwnd(), TEXT("destroy window"), TEXT("..."), 0);
			if (_hparent == 0)
				PostQuitMessage(0);
			return true;
		default:
			return false;
		}
	}
	virtual bool wnd_proc(UINT msg, WPARAM wparam, LPARAM lparam) = 0;
};

class window : public base_window
{
	window *_parent;
	std::map<HWND, window *> _children;
	window(const window &);
	window &operator=(const window&);
public:
	window(window *parent = 0) : _parent(parent)
	{
		if (_parent) {
			_parent->add_child_window(*this);
			this->set_parent(parent->get_hwnd());
			this->set_style(WS_CHILD);
		}
	}
	void add_child_window(window &child) 
	{
		if (_children.find(child.get_hwnd()) == _children.end()) {
			_children[child.get_hwnd()] = &child;
		}
	}
	void remove_child(const window &child)
	{
		std::map<HWND, window*>::iterator it = _children.find(child.get_hwnd());
		if (it != _children.end())
				_children.erase(it);
	}
	void update_children()
	{
		for (std::map<HWND, window*>::iterator it = _children.begin(); it != _children.end(); it++) {
			InvalidateRect(it->second->get_hwnd(), NULL, true);
		}
	}
	void destroy()
	{
		if (_parent)
			_parent->remove_child(*this);
		for (std::map<HWND, window *>::iterator it = _children.begin();
			it != _children.end(); it++)
				DestroyWindow(it->first);
		DestroyWindow(get_hwnd());
	}
	virtual bool wnd_proc(UINT, WPARAM, LPARAM) = 0;
};

class button : public window
{
	HBITMAP _hbitmap;
	rectangle _rect;
	//color _background;
	//color _border;
public:
	bool wnd_proc(UINT, WPARAM, LPARAM);
	void set_bitmap();
};