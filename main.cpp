#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

const char kWindowTitle[] = "LC1B_26_フミモトコウサク";

const int kWindowWidth = 800;
const int kWindowHeight = 800;

// 構造体
// x,y座標
typedef struct Vector2 {
	float x;
	float y;
};
// 自機
typedef struct Player {
	Vector2 pos;
	Vector2 speed;
	float radius;
	int unsigned color;
	int isAlive;
};

/***********変更点↓**********/
// 1の位
typedef struct Units {
	int digit;			// 桁
	Vector2 pos;		// 表示場所
	unsigned int color;
};
// 10の位
typedef struct Tens {
	int digit;			// 桁
	Vector2 pos;		// 表示場所
	unsigned int color;
};
/***********変更点↑**********/

// シーン切り替え
enum Scene {
	TITLE,
	GAME,
	RESULT
};
int scene = GAME;
// tileの番号
enum MapNumber {
	NONE,		   // 何もなし
	ENEMYRESPAWN   //　敵のスポーン地点
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	/***********変更点↓**********/
	// 画像読み込み
	int number[10] = {
	Novice::LoadTexture("./number0.png"),
	Novice::LoadTexture("./number1.png"),
	Novice::LoadTexture("./number2.png"),
	Novice::LoadTexture("./number3.png"),
	Novice::LoadTexture("./number4.png"),
	Novice::LoadTexture("./number5.png"),
	Novice::LoadTexture("./number6.png"),
	Novice::LoadTexture("./number7.png"),
	Novice::LoadTexture("./number8.png"),
	Novice::LoadTexture("./number9.png"),
	};
	int blockSize = 32;
	// 制限時間
	int timeOverCount = 3600;
	// 一の位
	Units units{};
	units.digit = 0;
	units.pos.x = static_cast<float>(kWindowWidth) / 2 + blockSize / static_cast<float>(2);
	units.pos.y = blockSize / static_cast<float>(2);
	// 十の位
	Tens tens{};
	tens.digit = 1;
	tens.pos.x = static_cast<float>(kWindowWidth) / 2 - blockSize / static_cast<float>(2);
	tens.pos.y = blockSize / static_cast<float>(2);
	/***********変更点↑**********/

	// 宣言
	// player
	Player player = {
		{kWindowWidth / 2,kWindowHeight / 2},
		{6.0f,6.0f},
		16.0f,
		WHITE,
		true
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

	// スコア
	int score = 0;
	int eachScore[6] = {};
	int boxScore = 0;

	// フラグ
	// 1秒チャージしたらtrueになって攻撃
	int isBlinkChageMax = false;
	// SPACE押してる間方向指定できる
	int isDirection = false;
	// バリア
	int isBarrier = false;

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
		switch (scene) {
		case TITLE:
			break;
		case GAME:
			// Rでenemyとplayerをリスポーン
			if (!keys[DIK_R] && preKeys[DIK_R]) {
				player.isAlive = true;
			}

			// チャージ攻撃の処理ここから↓
			// SPACE押してる間チャージ
			if (player.isAlive) {
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
						theta += 0.05f;
					}
					else if (keys[DIK_S]) {
						theta -= 0.05f;
					}
					else if (keys[DIK_A]) {
						theta += 0.05f;
					}
					else if (keys[DIK_D]) {
						theta -= 0.05f;
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

				
			}

			/***********変更点↓**********/
			// 制限時間の数字の処理
			timeOverCount--;
			if (timeOverCount % 60 == 0) {
				units.digit--;
				// unitsDigit(一の位の変数)が0以下
				// unitsDigitに9を代入
				if (units.digit <= -1) {
					units.digit = 9;
					// 十の位を1減らす
					tens.digit--;
				}
			}
			// 制限時間が0以下になったらリザルトに切り替え
			// 0秒になって一秒経過してからゲーム終了
			if (timeOverCount <= 0) {
				if (units.digit <= 9 && tens.digit <= -1) {
					// 制限時間の初期化
					scene = RESULT;
					timeOverCount = 3600;
					units.digit = 0;
					tens.digit = 6;
				}
			}
			/***********変更点↑**********/

			break;
		case RESULT:
			/*********デバッグ用**********/
			if (keys[DIK_SPACE] == 0 && preKeys[DIK_SPACE]) {
				boxScore += 1;
				score += 1;
			}
			// スコアの計算
			eachScore[5] = score / 1;
			boxScore = boxScore % 1;

			eachScore[4] = score / 10;
			boxScore = boxScore % 10;

			eachScore[3] = score / 100;
			boxScore = boxScore % 100;

			eachScore[2] = score / 1000;
			boxScore = boxScore % 1000;

			eachScore[1] = score / 10000;
			boxScore = boxScore % 10000;

			eachScore[0] = score / 100000;
			boxScore = boxScore % 100000;

			for (int num = 0; num < 6; num++) {
				eachScore[num] %= 10;
			}
			break;
		}
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		switch (scene) {
		case TITLE:
			break;
		case GAME:
			// player
			if (player.isAlive) {
				Novice::DrawEllipse(player.pos.x, player.pos.y, player.radius, player.radius, 0.0f, player.color, kFillModeSolid);
			}

			// バリア
			if (isBarrier) {
				Novice::DrawEllipse(barrier.pos.x, barrier.pos.y, barrier.radius, barrier.radius, 0.0f, barrier.color, kFillModeWireFrame);
			}
			/***********変更点↓**********/
			// 制限時間
			// 1の位
			for (int digitNumber = 0; digitNumber < 10; digitNumber++) {
				if (units.digit == digitNumber) {
					Novice::DrawSprite(units.pos.x, units.pos.y, number[digitNumber], 1.0f, 1.0f, 0.0f, WHITE);
				}
			}
			// 10の位
			for (int digitNumber = 0; digitNumber < 10; digitNumber++) {
				if (tens.digit == digitNumber) {
					Novice::DrawSprite(tens.pos.x, tens.pos.y, number[digitNumber], 1.0f, 1.0f, 0.0f, WHITE);
				}
			}
			/***********変更点↑**********/

			break;
		case RESULT:
			// score描画
			for (int num = 0; num < 10; num++) {
				// 10万の位
				if (eachScore[0] == num) {
					Novice::DrawSprite(100, 100, number[num], 1, 1, 0.0f, WHITE);
				}
				// 1万の位
				if (eachScore[1] == num) {
					Novice::DrawSprite(120, 100, number[num], 1, 1, 0.0f, WHITE);
				}
				// 1000の位
				if (eachScore[2] == num) {
					Novice::DrawSprite(140, 100, number[num], 1, 1, 0.0f, WHITE);
				}
				// 100の位
				if (eachScore[3] == num) {
					Novice::DrawSprite(160, 100, number[num], 1, 1, 0.0f, WHITE);
				}
				// 10の位
				if (eachScore[4] == num) {
					Novice::DrawSprite(180, 100, number[num], 1, 1, 0.0f, WHITE);
				}
				// 1の位
				if (eachScore[5] == num) {
					Novice::DrawSprite(200, 100, number[num], 1, 1, 0.0f, WHITE);
				}
			}
			break;
		}
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
