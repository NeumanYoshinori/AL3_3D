#include "worldTransform.h"
#include "Math.h"

Math* matrix = new Math;

void WorldUpdate::WorldTransformUpdate(WorldTransform& worldTransform) {
	// スケール、回転、平行移動を合成して行列を計算する
	worldTransform.matWorld_ = matrix->MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
	// 定数バッファへの書き込み
	worldTransform.TransferMatrix();
}
