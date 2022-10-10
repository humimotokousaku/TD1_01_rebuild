#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

const char kWindowTitle[] = "LC1B_26_フミモトコウサク";

const int kWindowWidth = 1280;
const int kWindowHeight = 720;

// 構造体
typedef struct Vector2 {
	float x;
	float y;
};
typedef struct Player {
	Vector2 pos;
	Vector2 speed;
	float radius;
	int unsigned color;
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };
	// 宣言
	// player
	Player player = {
		{kWindowWidth / 2,kWindowHeight / 2},
		{6.0f,6.0f},
		16.0f,
		WHITE
	};
	// チャージ攻撃するときのplayerの座標と速度
	Player blinkPlayer = {
		{player.pos.x,player.pos.y},
		{5.0f,5.0f}
	};

	// チャージ攻撃の方向指定
	float theta = 0.0f;
	Vector2 length = { 0.0f,0.0f };
	// チャージ攻撃している時間
	int frame = 60;
	// チャージ時間
	int blinkChageTime = 60;

	// フラグ
	// 1秒チャージしたらtrueになって攻撃
	int isBlinkChageMax = false;
	// SPACE押してる間方向指定できる
	int isDirection = false;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///
		// チャージ攻撃の処理ここから↓
		// SPACE押してる間チャージ
		if (keys[DIK_SPACE]) {
			isDirection = true;
			// チャージ攻撃のチャージ時間1秒
			blinkChageTime--;
			if (blinkChageTime <= 0) {
				blinkChageTime = 0;
			}
		}
		// SPACE離す
		// SPACEを1秒以上押した後チャージ攻撃ができる
		if (!keys[DIK_SPACE] && preKeys[DIK_SPACE]) {
			if (blinkChageTime <= 0) {
				isBlinkChageMax = true;
				isDirection = false;
			}
			// SPACEを1秒未満で離したらチャージ時間を初期化
			blinkChageTime = 60;
		}

		// チャージ攻撃の方向指定ここから↓
		// SPACE押してる間は方向指定できる
		if (isDirection) {
			blinkPlayer.pos.x = player.pos.x;
			blinkPlayer.pos.y = player.pos.y;
			if (keys[DIK_UP]) {
				theta += 0.1f;
			}
			else if (keys[DIK_DOWN]) {
				theta -= 0.1f;
			}
			else if (keys[DIK_LEFT]) {
				theta += 0.1f;
			}
			else if (keys[DIK_RIGHT]) {
				theta -= 0.1f;
			}
		}
		// 
		if (theta >= 6.0f) {
			theta = 0.0f;
		}
		else if (theta < 0.0f) {
			theta = 5.9f;
		}
		// チャージ攻撃の方向指定ここまで↑

		// 1秒間チャージ
		if (isBlinkChageMax) {
			// 1秒間指定した方向にまっすぐ進む
			frame--;
			if (theta != -1.0f) {
				player.pos.x += (cosf(theta) * blinkPlayer.speed.x);
				player.pos.y += -(sinf(theta) * blinkPlayer.speed.y);
			}
		}
		// 初期化
		if (frame <= 0) {
			isBlinkChageMax = false;
			blinkChageTime = 60;
			frame = 60;
		}
		// チャージ攻撃の処理ここまで↑
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		// player
		Novice::DrawEllipse(player.pos.x, player.pos.y, player.radius, player.radius, 0.0f, player.color, kFillModeSolid);
		// 文字表示
		Novice::ScreenPrintf(0, 10, "Theta:%f", theta);
		Novice::ScreenPrintf(0, 30, "PlayerPosX,Y:%1.f:%1.f", player.pos.x, player.pos.y);
		Novice::ScreenPrintf(0, 50, "ChageTime:%d", blinkChageTime);
		Novice::ScreenPrintf(0, 70, "AttackTime:%d", frame);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
