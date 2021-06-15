#include "DXUT.h"
#include "Draw.h"

namespace Shader
{
	LPD3DXEFFECT effect;
	D3DXHANDLE technique;
	D3DXHANDLE mWVP;

	HRESULT Initialize(LPDIRECT3DDEVICE9 device)
	{
		LPD3DXBUFFER error;
		if (FAILED(D3DXCreateEffectFromFile(
			device, // 디바이스
			L"Shader/Shader.fx", // 이펙트 파일 명 포인터
			NULL, // 전처리기 포인터
			NULL, // 옵션 인터페이스 포인터
			0, // D3DXSHADER 식별 컴파일 옵션
			NULL, // 공유 인수로 사용하는 ID3DXEffectPool 오브젝트 포인터
			&effect, // 컴파일된 이펙트 파일이 저장될 버퍼
			&error))) // 컴파일 에러가 저장될 버퍼
		{
			MessageBox(NULL, (LPCTSTR)error->GetBufferPointer(), L"ERROR", MB_OK);
		}
		else
		{
			technique = effect->GetTechniqueByName("TShader");
			mWVP = effect->GetParameterByName(NULL, "mWVP");
		}
		error->Release();

		return S_OK;
	}
	
	// 윈도우 크기가 바뀌었을 때 등 디바이스가 리셋되었을 때, OnLostDevice를 호출
	// 리셋으로부터 복원되었을 때, OnResetDevice를 호출


	void Update(LPDIRECT3DDEVICE9 device)
	{
		effect->SetTechnique(technique);
		effect->Begin(NULL, 0);
		effect->BeginPass(0);

		

		effect->EndPass();
		effect->End();
	}

	void Close()
	{
		effect->Release();
	}
}