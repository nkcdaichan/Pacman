//
// Main.cpp
//

#include "pch.h"
#include "Game.h"


using namespace DirectX;


namespace
{
  std::unique_ptr<Game> g_game;
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//�n�C�u���b�h�O���t�B�b�N�X�V�X�e���Ƀf�B�X�N���[�g�p�[�g��D�悷��悤�w��
extern "C"
{
  __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
  __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

//�G���g���[�|�C���g
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  //CPU�T�|�[�g�̊m�F
  if (!XMVerifyCPUSupport())
    return 1;
  //COM�̏�����
  HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
  if (FAILED(hr))
    return 1;
  //�Q�[���I�u�W�F�N�g�̍쐬
  g_game = std::make_unique<Game>();

  //�N���X�̓o�^�ƃE�B���h�E�̍쐬
  {
	//�N���X�̓o�^
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, L"IDI_ICON");
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"PacmanWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, L"IDI_ICON");
    if (!RegisterClassEx(&wcex))
      return 1;

	//�E�B���h�E�̍쐬
    uint16_t w, h;
    g_game->GetDefaultSize(w, h);

    RECT rc = { 0, 0, static_cast<LONG>(w), static_cast<LONG>(h) };

    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hwnd = CreateWindowEx(0, L"PacmanWindowClass", L"Pacman", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
      nullptr);

    if (!hwnd)
      return 1;

    ShowWindow(hwnd, nCmdShow);
	//�f�t�H���g���t���X�N���[���ɂ���ɂ�nCmdShow��SW_SHOWMAXIMIZED �ɕύX����

    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(g_game.get()));

    GetClientRect(hwnd, &rc);

    g_game->Initialize(hwnd, static_cast<uint16_t>(rc.right - rc.left), static_cast<uint16_t>(rc.bottom - rc.top));
  }

  //���C�����b�Z�[�W���[�v
  MSG msg = {};
  while (WM_QUIT != msg.message)
  {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else
    {
      g_game->Tick();
    }
  }

  g_game.reset();

  CoUninitialize();

  return static_cast<int>(msg.wParam);
}

//Windows�v���V�[�W��
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  PAINTSTRUCT ps;
  HDC hdc;

  static bool s_in_sizemove = false;
  static bool s_in_suspend = false;
  static bool s_minimized = false;
  static bool s_fullscreen = false;

  auto game = reinterpret_cast<Game*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

  switch (message)
  {
  case WM_PAINT:
    if (s_in_sizemove && game)
    {
      game->Tick();
    }
    else
    {
      hdc = BeginPaint(hWnd, &ps);
      EndPaint(hWnd, &ps);
    }
    break;

  case WM_SIZE:
    if (wParam == SIZE_MINIMIZED)
    {
      if (!s_minimized)
      {
        s_minimized = true;
        if (!s_in_suspend && game)
          game->OnSuspending();
        s_in_suspend = true;
      }
    }
    else if (s_minimized)
    {
      s_minimized = false;
      if (s_in_suspend && game)
        game->OnResuming();
      s_in_suspend = false;
    }
    else if (!s_in_sizemove && game)
    {
      game->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
    }
    break;

  case WM_ENTERSIZEMOVE:
	//�T�C�Y�ύX���J�n���ꂽ���Ƃ��L�^
    s_in_sizemove = true;
    break;

  case WM_EXITSIZEMOVE:
	//�T�C�Y�ύX���I���������Ƃ��L�^��,�Q�[���ɃE�B���h�E�T�C�Y�̕ύX��ʒm
    s_in_sizemove = false;
    if (game)
    {
      RECT rc;
      GetClientRect(hWnd, &rc);

      game->OnWindowSizeChanged(static_cast<uint16_t>(rc.right - rc.left), static_cast<uint16_t>(rc.bottom - rc.top));
    }
    break;

  case WM_GETMINMAXINFO:
   //�E�B���h�E�̍ŏ��g���b�N�T�C�Y��ݒ�
  {
    auto info = reinterpret_cast<MINMAXINFO*>(lParam);
    info->ptMinTrackSize.x = 320;
    info->ptMinTrackSize.y = 200;
  }
  break;

  case WM_ACTIVATEAPP:
	//�A�v���P�[�V�����̃A�N�e�B�u��Ԃ��ύX���ꂽ�Ƃ��̏���
    if (game)
    {
      if (wParam)
      {
        game->OnActivated();
      }
      else
      {
        game->OnDeactivated();
      }
    }
    Keyboard::ProcessMessage(message, wParam, lParam);
    break;

  case WM_KEYDOWN:
  case WM_KEYUP:
  case WM_SYSKEYUP:
	//�L�[�{�[�h���͂̏���
    Keyboard::ProcessMessage(message, wParam, lParam);
    break;

  case WM_POWERBROADCAST:
	//�d���̏�ԕύX�Ɋւ��鏈��
    switch (wParam)
    {
    case PBT_APMQUERYSUSPEND:
	  //�V�X�y���h��Ԃւ̈ڍs�O�ɍs������
      if (!s_in_suspend && game)
        game->OnSuspending();
      s_in_suspend = true;
      return TRUE;

    case PBT_APMRESUMESUSPEND:
      if (!s_minimized)
      {
        if (s_in_suspend && game)
          game->OnResuming();
        s_in_suspend = false;
      }
      return TRUE;
    }
    break;

  case WM_DESTROY:
	//�E�B���h�E���j�����ꂽ�Ƃ��̏���
    PostQuitMessage(0);
    break;

  case WM_SYSKEYDOWN:
	//�V�X�e���L�[�������ꂽ�Ƃ��̏���
    if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
    {
      //ALT+ENTER�t���X�N���[���؂�ւ��̎���
      if (s_fullscreen)
      {
		//�E�B���h�E���[�h�ɐ؂�ւ�
        SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
        SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

        uint16_t width = 800;
        uint16_t height = 600;

        if (game)
          game->GetDefaultSize(width, height);

        ShowWindow(hWnd, SW_SHOWNORMAL);

        SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
      }
      else
      {
        SetWindowLongPtr(hWnd, GWL_STYLE, 0);
        SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

        SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

        ShowWindow(hWnd, SW_SHOWMAXIMIZED);
      }

      s_fullscreen = !s_fullscreen;
    }
    Keyboard::ProcessMessage(message, wParam, lParam);
    break;

  case WM_MENUCHAR:
	  //���j���[���A�N�e�B�u��,���[�U�[�����j�b�N��A�N�Z�����[�^�L�[�ɑΉ����Ȃ��L�[���������ꍇ��,
	  //�G���[�r�[�v�𔭐������Ȃ��悤�ɖ�������B
    return MAKELRESULT(0, MNC_CLOSE);
  }
  //�f�t�H���g�̃E�B���h�E�v���V�[�W���Ƀ��b�Z�[�W��n��
  return DefWindowProc(hWnd, message, wParam, lParam);
}

//�I���⏕�֐�
void ExitGame()
{
  //���b�Z�[�W�L���[�ɏI�����b�Z�[�W�𑗐M
  PostQuitMessage(0);
}
