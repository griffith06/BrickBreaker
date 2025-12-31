#pragma once
#include "Common/CVectorVer2.h"
#include "CoreMinimal.h"
#include "BreakerCoreUnreal.h"



void CopyVector3fFromFVector(Vector3f tar, FVector src);
void CopyFVectorFromVector3f(FVector& tar, Vector3f src);
void FileLog(int id, const char* fmt, ...);

bool ReadAllTrackFromAction(TArray<FTrackAction>& tr_action, FVector& pos, FVector& start, FVector& end);

bool ReadFileLogFromAction(FBlocks* blocks, FVector& pos, FVector& start, FVector& end);
bool WriteFileLogFromAction(FBlocks* blocks, FVector& start, FVector& end);
bool WriteFileLogFromAction(FBlocks* blocks, Vector3f start, Vector3f end);
