#include "GameScene.h"
#include "KamataEngine.h"
#include <Windows.h>

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// 初期化
	Initialize(L"LE2C_21_ニューマン_ヨシノリ_AL3");

	// DirectXCommonのインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// ゲームシーンのインスタンス生成
	GameScene* gameScene = new GameScene();
	// ゲームシーンの初期化
	gameScene->Initialize();

	// メインループ
	while (true) {
		// エンジンの更新
		if (Update()) {
			break;
		}

		// ゲームシーンの更新
		gameScene->Update();

		// 描画開始
		dxCommon->PreDraw();

		// ゲームシーンの描画
		gameScene->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}

	// 解放処理
	Finalize();

	delete gameScene;

	gameScene = nullptr;

	return 0;
}
