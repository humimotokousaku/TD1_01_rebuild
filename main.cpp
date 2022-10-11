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
	// enemy
	Player enemy = {
		{200.0f,200.0f},
		{4.0f,4.0f},
		16.0f,
		BLACK
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

	// バリア
	Player barrier = {
		{player.pos.x,player.pos.y},
		{6.0f,6.0f},
		32.0f,
		RED
	};
	int barrierTime = 120;
	int barrierHp = 10;

	// 当たり判定
	float p2eR = player.radius + enemy.radius;
	float p2eX;
	float p2eY;

	// フラグ
	// 1秒チャージしたらtrueになって攻撃
	int isBlinkChageMax = false;
	// SPACE押してる間方向指定できる
	int isDirection = false;
	// バリア
	int isBarrier = false;
	// player
	int isPlayerAlive = true;
	// enemy
	int isEnemyAlive = true;

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
		// Rでenemyとplayerをリスポーン
		if (!keys[DIK_R] && preKeys[DIK_R]) {
			isPlayerAlive = true;
			isEnemyAlive = true;
		}
		
		// チャージ攻撃の処理ここから↓
		// SPACE押してる間チャージ
		if (isPlayerAlive) {
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
				}
				// バリア展開
				else if (blinkChageTime >= 50) {
					if (!isBlinkChageMax) {
						isBarrier = true;
					}
				}
				// SPACEを1秒未満で離したらチャージ時間を初期化
				blinkChageTime = 60;
				isDirection = false;
			}

			// チャージ攻撃の方向指定ここから↓
			// SPACE押してる間は方向指定できる
			if (isDirection) {
				blinkPlayer.pos.x = player.pos.x;
				blinkPlayer.pos.y = player.pos.y;
				if (keys[DIK_W]) {
					theta += 0.1f;
				}
				else if (keys[DIK_S]) {
					theta -= 0.1f;
				}
				else if (keys[DIK_A]) {
					theta += 0.1f;
				}
				else if (keys[DIK_D]) {
					theta -= 0.1f;
				}
			}
			// SPACE押してないときはplayerの移動
			if (isBarrier || !isDirection) {
				if (keys[DIK_W]) {
					player.pos.y -= player.speed.y;
				}
				else if (keys[DIK_S]) {
					player.pos.y += player.speed.y;
				}
				else if (keys[DIK_A]) {
					player.pos.x -= player.speed.x;
				}
				else if (keys[DIK_D]) {
					player.pos.x += player.speed.x;
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

			// バリアの処理
			if (!isBlinkChageMax) {
				// バリアが出てるときはチャージ攻撃できない
				if (isBarrier) {
					barrier.pos.x = player.pos.x;
					barrier.pos.y = player.pos.y;
					barrierTime--;
					blinkChageTime = 60;
				}
				// バリアは2秒で消える
				if (barrierTime <= 0) {
					isBarrier = false;
					barrierTime = 120;
				}
			}

			// 当たり判定
			p2eX = player.pos.x - enemy.pos.x;
			p2eY = player.pos.y - enemy.pos.y;
			if (p2eR * p2eR >= p2eX * p2eX + p2eY * p2eY) {
				// 通常時
				isPlayerAlive = false;
				// バリアの判定
				if (isBarrier) {
					isPlayerAlive = true;
				}
				// チャージ攻撃
				if (isBlinkChageMax) {
					isPlayerAlive = true;
					isEnemyAlive = false;
				}
			}
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		// player
		if (isPlayerAlive) {
			Novice::DrawEllipse(player.pos.x, player.pos.y, player.radius, player.radius, 0.0f, player.color, kFillModeSolid);
		}
		// enemy
		if (isEnemyAlive) {
			Novice::DrawEllipse(enemy.pos.x, enemy.pos.y, enemy.radius, enemy.radius, 0.0f, enemy.color, kFillModeSolid);
		}
		
		// バリア
		if (isBarrier) {
			Novice::DrawEllipse(barrier.pos.x, barrier.pos.y, barrier.radius, barrier.radius, 0.0f, barrier.color, kFillModeWireFrame);
		}
		// 文字表示
		Novice::ScreenPrintf(0, 10, "Theta:%f", theta);
		Novice::ScreenPrintf(0, 30, "PlayerPosX,Y:%1.f:%1.f", player.pos.x, player.pos.y);
		Novice::ScreenPrintf(0, 50, "ChageTime:%d", blinkChageTime);
		Novice::ScreenPrintf(0, 70, "AttackTime:%d", frame);
		Novice::ScreenPrintf(0, 90, "isBarrier:%d", isBarrier);
		Novice::ScreenPrintf(0, 110, "isDirection:%d", isDirection);

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
