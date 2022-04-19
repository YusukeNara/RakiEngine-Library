#include "Raki_Input.h"
#include "Raki_WinAPI.h"

#include <stdio.h>

BYTE Input::keys[256]    = {};
BYTE Input::oldkeys[256] = {};
DIMOUSESTATE Input::mouseState;
DIMOUSESTATE Input::oldMouseState;
POINT        Input::pos;
XINPUT_STATE Input::xInputState;
XINPUT_STATE Input::oldxInputState;
//デッドゾーン定数（初期値はXINPUTライブラリ提供の定数を使用）
SHORT Input::XPAD_RS_DEADZONE = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
SHORT Input::XPAD_LS_DEADZONE = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

IDirectInput8       *Input::dinput      = nullptr;
IDirectInputDevice8 *Input::devkeyBoard = nullptr;
IDirectInputDevice8 *Input::devMouse	= nullptr;

bool Input::Init(WNDCLASSEX w, HWND hwnd)
{
	HRESULT result;
	//初期化
	result = DirectInput8Create(w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&dinput, nullptr);
	if (result != S_OK) { return false; }

	//キーボードデバイス生成
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyBoard, NULL);
	if (result != S_OK) { return false; }
	//マウスデバイス生成
	result = dinput->CreateDevice(GUID_SysMouse, &devMouse, NULL);
	if (result != S_OK) { return false; }

	//入力データ形式セット
	result = devkeyBoard->SetDataFormat(&c_dfDIKeyboard);//標準
	if (result != S_OK) { return false; }
	result = devMouse->SetDataFormat(&c_dfDIMouse);
	if (result != S_OK) { return false; }

	//排他制御レベルセット
	result = devkeyBoard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (result != S_OK) { return false; }
	result = devMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (result != S_OK) { return false; }

	//xinputの入力状態のクリア
	ZeroMemory(&xInputState, sizeof(xInputState));
	//xinputの入力状態のテスト取得
	auto xresult = XInputGetState(0, &xInputState);
	//取得失敗時、メッセージ出力
	if (xresult != ERROR_SUCCESS) {
		printf("FAILED : xInput : Failed to get input from xInput.\n");
	}
	else {
		printf("SUCCESSED : xInput : Get input state.\n");
	}

	return true;
}

void Input::StartGetInputState()
{
	//前フレームの入力情報を取得
	for (int i = 0; i < 256; i++) {
		oldkeys[i] = keys[i];
	}
	//キー入力状態取得開始
	devkeyBoard->Acquire();
	//全キーの入力状態を取得
	devkeyBoard->GetDeviceState(sizeof(keys), keys);

	//マウスの前フレームの入力状態を取得
	oldMouseState = mouseState;
	//マウス入力状態取得開始
	devMouse->Acquire();
	//マウスの入力状態を取得
	devMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);

	//前フレームの入力状態を保存
	oldxInputState = xInputState;
	//xinputの入力状態のクリア
	ZeroMemory(&xInputState, sizeof(xInputState));
	//xinputの入力状態の取得開始
	auto result = XInputGetState(0, &xInputState);
	//デッドゾーン範囲にまとめる
	XpadStickTiltRoundOffToDeadzone();
}

bool Input::isKey(int key)
{
	return keys[key];
}

bool Input::isKeyTrigger(int key)
{
	return keys[key] && !oldkeys[key];
}

bool Input::isKeyReleased(int key)
{
	return oldkeys[key] && !keys[key];
}

bool Input::isMouseClicking(int keyCode)
{
	//マウスが押されている
	return mouseState.rgbButtons[keyCode] & (0x80);
}

bool Input::isMouseClickTrigger(int keyCode)
{
	BYTE onValue = 0x80;
	//前フレームは押されておらず、現フレームで押されている
	return (mouseState.rgbButtons[keyCode] == onValue) & (oldMouseState.rgbButtons[keyCode] != onValue);
}

bool Input::isMouseClicked(int keyCode)
{
	BYTE onValue = 0x80;
	//前フレームで押されており、現フレームで押されていない
	return (mouseState.rgbButtons[keyCode] != onValue) & (oldMouseState.rgbButtons[keyCode] == onValue);
}

XMFLOAT2 Input::getMousePos()
{
	//デスクトップ座標を取得
	GetCursorPos(&pos);
	//クライアント領域座標変換
	ScreenToClient(Raki_WinAPI::GetHWND(), &pos);
	//戻り値に格納
	XMFLOAT2 result((float)pos.x, (float)pos.y);
	//返却
	return result;
}

XMFLOAT2 Input::getMouseVelocity()
{
	XMFLOAT2 result;
	result.x = (float)mouseState.lX;
	result.y = (float)mouseState.lY;

	return result;
}

bool Input::isXpadButtonPushing(XPAD_INPUT_CODE code)
{
	switch (code)
	{
	case XPAD_BUTTON_A:
		return xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A;
		break;
	case XPAD_BUTTON_B:
		return xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_B;
		break;
	case XPAD_BUTTON_X:
		return xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_X;
		break;
	case XPAD_BUTTON_Y:
		return xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_Y;
		break;
	case XPAD_BUTTON_CROSS_UP:
		return xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
		break;
	case XPAD_BUTTON_CROSS_DOWN:
		return xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
		break;
	case XPAD_BUTTON_CROSS_LEFT:
		return xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
		break;
	case XPAD_BUTTON_CROSS_RIGHT:
		return xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
		break;
	case XPAD_TRIGGER_LB:
		return xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
		break;
	case XPAD_TRIGGER_LT:
		return xInputState.Gamepad.bLeftTrigger != 0;
		break;
	case XPAD_TRIGGER_RB:
		return xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
		break;
	case XPAD_TRIGGER_RT:
		return xInputState.Gamepad.bRightTrigger != 0;
		break;
	case XPAD_BUTTON_LSTICK:
		return xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
		break;
	case XPAD_BUTTON_RSTICK:
		return xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
		break;
	case XPAD_BUTTON_OPTION_R:
		return xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK;
		break;
	case XPAD_BUTTON_OPTION_L:
		return xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_START;
		break;
	default:
		break;
	}
}

bool Input::isXpadButtonPushTrigger(XPAD_INPUT_CODE code)
{

	return isXpadButtonPushing(code) == true && isOldXpadPushing(code) == false;
}

bool Input::isXpadButtonPushed(XPAD_INPUT_CODE code)
{
	return isXpadButtonPushing(code) == false && isOldXpadPushing(code) == true;
}

int Input::GetXpadRTStrength()
{
	return static_cast<int>(xInputState.Gamepad.bRightTrigger);
}

int Input::GetXpadLTStrength()
{
	return static_cast<int>(xInputState.Gamepad.bLeftTrigger);
}

bool Input::isXpadStickTilt(XPAD_STICK_DIRECTION_CODE dircode)
{
	return false;
}

Input *Input::Get()
{
	static Input instance;
	return &instance;
}

bool Input::isOldXpadPushing(XPAD_INPUT_CODE code)
{
	switch (code)
	{
	case XPAD_BUTTON_A:
		return oldxInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A;
		break;
	case XPAD_BUTTON_B:
		return oldxInputState.Gamepad.wButtons & XINPUT_GAMEPAD_B;
		break;
	case XPAD_BUTTON_X:
		return oldxInputState.Gamepad.wButtons & XINPUT_GAMEPAD_X;
		break;
	case XPAD_BUTTON_Y:
		return oldxInputState.Gamepad.wButtons & XINPUT_GAMEPAD_Y;
		break;
	case XPAD_BUTTON_CROSS_UP:
		return oldxInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
		break;
	case XPAD_BUTTON_CROSS_DOWN:
		return oldxInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
		break;
	case XPAD_BUTTON_CROSS_LEFT:
		return oldxInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
		break;
	case XPAD_BUTTON_CROSS_RIGHT:
		return oldxInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
		break;
	case XPAD_TRIGGER_LB:
		return oldxInputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
		break;
	case XPAD_TRIGGER_LT:
		return oldxInputState.Gamepad.bLeftTrigger != 0;
		break;
	case XPAD_TRIGGER_RB:
		return oldxInputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
		break;
	case XPAD_TRIGGER_RT:
		return oldxInputState.Gamepad.bRightTrigger != 0;
		break;
	case XPAD_BUTTON_LSTICK:
		return oldxInputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
		break;
	case XPAD_BUTTON_RSTICK:
		return oldxInputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
		break;
	case XPAD_BUTTON_OPTION_R:
		return oldxInputState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK;
		break;
	case XPAD_BUTTON_OPTION_L:
		return oldxInputState.Gamepad.wButtons & XINPUT_GAMEPAD_START;
		break;
	default:
		return false;
		break;
	}

	return false;
}

void Input::XpadStickTiltRoundOffToDeadzone()
{
	//傾き方向を判別する関数で、どっちの方向にも傾いていない場合
	//スティックの傾きはデッドゾーンの範囲内と判断できるので、その場合は傾きを0にする

	if (!isRSTiltUp() && !isRSTiltDown()) { xInputState.Gamepad.sThumbRY = (SHORT)0; }
	if (!isRSTiltRight() && !isRSTiltLeft()) { xInputState.Gamepad.sThumbRX = (SHORT)0; }
	if (!isLSTiltUp() && !isLSTiltDown()) { xInputState.Gamepad.sThumbLY = (SHORT)0; }
	if (!isLSTiltRight() && !isLSTiltLeft()) { xInputState.Gamepad.sThumbLX = (SHORT)0; }
}

bool Input::isRSTiltRight() { return xInputState.Gamepad.sThumbRX > XPAD_RS_DEADZONE; }

bool Input::isRSTiltLeft() { return xInputState.Gamepad.sThumbRX < -XPAD_RS_DEADZONE; }

bool Input::isRSTiltUp() { return xInputState.Gamepad.sThumbRY > XPAD_RS_DEADZONE; }

bool Input::isRSTiltDown() { return xInputState.Gamepad.sThumbRY < -XPAD_RS_DEADZONE; }

bool Input::isLSTiltRight() { return xInputState.Gamepad.sThumbLX > XPAD_LS_DEADZONE; }

bool Input::isLSTiltLeft() { return xInputState.Gamepad.sThumbLX < -XPAD_LS_DEADZONE; }

bool Input::isLSTiltUp() { return xInputState.Gamepad.sThumbLY > XPAD_LS_DEADZONE; }

bool Input::isLSTiltDown() { return xInputState.Gamepad.sThumbLY < -XPAD_LS_DEADZONE; }

Input::StickTiltParam Input::GetXpadRStickTilt()
{
	StickTiltParam result;
	result.x = (int)xInputState.Gamepad.sThumbRX;
	result.y = (int)xInputState.Gamepad.sThumbRY;

	return result;
}

Input::StickTiltParam Input::GetXpadLStickTilt()
{
	StickTiltParam result;
	result.x = (int)xInputState.Gamepad.sThumbLX;
	result.y = (int)xInputState.Gamepad.sThumbLY;

	return result;
}
