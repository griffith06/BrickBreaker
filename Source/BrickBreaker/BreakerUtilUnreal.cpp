#include "BreakerUtilUnreal.h"
#include "BreakerCoreUnreal.h"

#pragma warning(disable : 4996)

void CopyVector3fFromFVector(Vector3f tar, FVector src)
{
	tar[0] = src.X;
	tar[1] = src.Y;
	tar[2] = src.Z;
}
void CopyFVectorFromVector3f(FVector& tar, Vector3f src)
{
	tar.X = src[0];
	tar.Y = src[1];
	tar.Z = src[2];
}
/*
void FileLog(int id, const TCHAR* fmt, ...)
{
	FString ProjectDir = FPaths::ProjectDir();
	FString LogPath = ProjectDir + TEXT("breaker_log.txt");

	if (id == 0) {
		// 파일 삭제
		if (FPaths::FileExists(LogPath)) {
			IFileManager::Get().Delete(*LogPath);
		}
	}

	// 가변 인수 처리
	va_list args;
	va_start(args, fmt);
	TCHAR Buffer[8192]; // 충분한 크기 확보
	FString ret = FString::Printf(Buffer, fmt, args); // va_list를 FString::Printf로 처리
	va_end(args);

	FString FormattedLog = ret + TEXT("\n");
	FFileHelper::SaveStringToFile(FormattedLog, *LogPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);

}
*/

FILE* g_fp = nullptr;
void FileLog(int id, const char* fmt, ...)
{
	char buf[8192];
	va_list args, args_copy;

	va_start(args, fmt);
	va_copy(args_copy, args);

	int count = vprintf(fmt, args);
	vsnprintf(buf, sizeof(buf), fmt, args_copy); // 안전하게 변경

	va_end(args);
	va_end(args_copy);

	FString ProjectDir = FPaths::ProjectDir();
//	UE_LOG(LogTemp, Log, TEXT("ProjectDir: %s"), *ProjectDir);

	if (id == 0)
	{
		char log_path[1256];
		snprintf(log_path, sizeof(log_path), "%s/breaker_log.txt", TCHAR_TO_ANSI(*ProjectDir)); // 안전하게 변경
		//파일삭제.
		if (FPaths::FileExists(log_path))
		{
			IFileManager::Get().Delete(*FString(log_path));
		}
		g_fp = fopen(log_path, "a+");
	}

	if (g_fp)
	{
		fprintf(g_fp, "%s\n", buf);
	}
	if (id == 2 && g_fp)
	{
		fclose(g_fp);
		g_fp = nullptr;
	}
}
void GetFVectorFromFp(FVector& pos, FILE* fp)
{
	char hole[1256];
	fscanf(fp, "%s", hole);
	pos.X = atof(hole);
	fscanf(fp, "%s", hole);
	pos.Y = atof(hole);
	fscanf(fp, "%s", hole);
	pos.Z = atof(hole);
}
void GetVector3fFromFp(Vector3f pos, FILE* fp)
{
	char hole[1256];
	fscanf(fp, "%s", hole);
	pos[0] = atof(hole);
	fscanf(fp, "%s", hole);
	pos[1] = atof(hole);
	fscanf(fp, "%s", hole);
	pos[2] = atof(hole);
}

bool ReadAllTrackFromAction(TArray<FTrackAction>& tr_action, FVector& pos, FVector& start, FVector& end)
{
	FString ProjectDir = FPaths::ProjectDir();
	UE_LOG(LogTemp, Log, TEXT("ProjectDir: %s"), *ProjectDir);
	char log_path[1256];
	sprintf(log_path, "%s/breaker_action.txt", TCHAR_TO_ANSI(*ProjectDir));

	FILE* fp = fopen(log_path, "rt");
	if (!fp)
		return false;
	char hole[1256];
	fscanf(fp, "%s", hole); //start

	if (strcmp(hole, "start"))
	{
		fclose(fp);
		return false;
	}
	GetFVectorFromFp(start, fp);

	fscanf(fp, "%s", hole); //end
	GetFVectorFromFp(end, fp);

	int t_idx = -1;
	FTrackAction tr;
	for (;;)
	{
		if (fscanf(fp, "%s", hole) == EOF)
		{
			break;
		}
		if (!strcmp(hole, "pos"))
		{
			t_idx++;
			tr_action.Add(tr);
			tr_action[t_idx].Blocks.Clear();
			GetVector3fFromFp(tr_action[t_idx].Start, fp);
			GetVector3fFromFp(tr_action[t_idx].End, fp);
		}
		if (!strcmp(hole, "stage"))
		{
			fscanf(fp, "%s", hole);
			tr_action[t_idx].Blocks.Stage = atoi(hole);
		}
		if (!strcmp(hole, "block"))
		{
			fscanf(fp, "%s", hole);
			int i = atoi(hole);
			fscanf(fp, "%s", hole);
			int j = atoi(hole);
			fscanf(fp, "%s", hole);
			tr_action[t_idx].Blocks.Block[i][j].Hp = atoi(hole);
			tr_action[t_idx].Blocks.Block[i][j].State = en_block_normal;
		}
	}
	t_idx++;
	if (t_idx > 0)
	{
		CopyFVectorFromVector3f(pos, tr_action[0].Start);
	}
	return true;
}

bool ReadFileLogFromAction(FBlocks* blocks, FVector& pos, FVector& start, FVector& end)
{
	FString ProjectDir = FPaths::ProjectDir();
	UE_LOG(LogTemp, Log, TEXT("ProjectDir: %s"), *ProjectDir);
	char log_path[1256];
	sprintf(log_path, "%s/breaker_action.txt", TCHAR_TO_ANSI(*ProjectDir));

	FILE* fp = fopen(log_path, "rt");
	if (!fp)
		return false;
	char hole[1256];
	fscanf(fp, "%s", hole); //start
	if (strcmp(hole, "start"))
	{
		fclose(fp);
		return false;
	}
	GetFVectorFromFp(start, fp);

	fscanf(fp, "%s", hole); //end
	GetFVectorFromFp(end, fp);

	fscanf(fp, "%s", hole); //pos
	GetFVectorFromFp(pos, fp);

	fscanf(fp, "%s", hole);
	fscanf(fp, "%s", hole);
	fscanf(fp, "%s", hole);

	fscanf(fp, "%s", hole); //stage
	fscanf(fp, "%s", hole); //
	int stage = atoi(hole);
	blocks->Clear();
	blocks->Stage = stage;
	while (1)
	{
		fscanf(fp, "%s", hole);
		if (!strcmp(hole, "pos"))
			break;
		if (!strcmp(hole, "block"))
		{
			fscanf(fp, "%s", hole);
			int i = atoi(hole);
			fscanf(fp, "%s", hole);
			int j = atoi(hole);
			fscanf(fp, "%s", hole);
			blocks->Block[i][j].Hp = atoi(hole);
			blocks->Block[i][j].State = en_block_normal;
		}
	}

	fclose(fp);
	return true;
}

bool WriteFileLogFromAction(FBlocks* blocks, Vector3f start, Vector3f end)
{
	FVector f_start, f_end;
	CopyFVectorFromVector3f(f_start, start);
	CopyFVectorFromVector3f(f_end, end);
	return WriteFileLogFromAction(blocks, f_start, f_end);
}
bool WriteFileLogFromAction(FBlocks *blocks, FVector& start, FVector& end)
{
//	return false;

	FString ProjectDir = FPaths::ProjectDir();
//	UE_LOG(LogTemp, Log, TEXT("ProjectDir: %s"), *ProjectDir);
	char log_path[1256];
	sprintf(log_path, "%s/breaker_log.txt", TCHAR_TO_ANSI(*ProjectDir));

	FILE* fp = fopen(log_path, "a+");

	if (!fp)
		return false;

	FileLog(1, "pos %f %f %f %f %f %f"
		, start.X, start.Y, start.Z
		, end.X, end.Y, end.Z
	);
	FileLog(1, "stage %d", blocks->Stage);
	int i, j;
	for (i = 0; i < _BLOCK_ROWS; i++)
	{
		for (j = 0; j < _BLOCK_COLS; j++)
		{
			FBlock* block = &blocks->Block[i][j];
			if (block->State == en_block_normal)
			{
				FileLog(1, "	block %d %d %d", i, j, block->Hp);
			}
		}
	}

	fclose(fp);
	return true;
}