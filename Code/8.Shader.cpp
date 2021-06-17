#include "DXUT.h"
#include "Draw.h"
#include <string>

namespace Shader
{
	LPD3DXEFFECT effect;
	D3DXHANDLE technique;
	D3DXHANDLE mWVP;

	LPD3DXMESH mesh;
	LPD3DXBUFFER subsetBuffer;
	LPD3DXMATERIAL subset;
	LPDIRECT3DTEXTURE9 texture;
	std::wstring filepath = L"Source/";
	std::wstring filename = L"map.x";

	HRESULT Initialize(LPDIRECT3DDEVICE9 device)
	{
		WCHAR finalpath[MAX_PATH];

		::lstrcpy(finalpath, filepath.c_str());
		::lstrcat(finalpath, filename.c_str());

		if (FAILED(D3DXLoadMeshFromX(finalpath, D3DXMESH_MANAGED, device, nullptr, &subsetBuffer, nullptr, nullptr, &mesh)))
			return E_FAIL;

		subset = static_cast<LPD3DXMATERIAL>(subsetBuffer->GetBufferPointer());

		WCHAR _filename[MAX_PATH] = L"";
		::lstrcpy(finalpath, filepath.c_str());
		MultiByteToWideChar(CP_ACP, 0, subset[0].pTextureFilename, strlen(subset[0].pTextureFilename), _filename, MAX_PATH);
		::lstrcat(finalpath, _filename);

		if (FAILED(D3DXCreateTextureFromFile(device, finalpath, &texture)))
			return E_FAIL;


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
		
		if(error != nullptr) error->Release();

		return S_OK;
	}
	
	// 윈도우 크기가 바뀌었을 때 등 디바이스가 리셋되었을 때, OnLostDevice를 호출
	// 리셋으로부터 복원되었을 때, OnResetDevice를 호출

	void Update(LPDIRECT3DDEVICE9 device)
	{
		D3DXMATRIX mWorld, mView, mProj;
		D3DXMatrixIdentity(&mWorld);
		D3DXVECTOR3 eye{ 0.f,5.f,-8.f };
		D3DXVECTOR3 lookat{ 0.f,-2.f,0.f };
		D3DXVECTOR3 up{ 0.f,1.f,0.f };
		D3DXMatrixLookAtLH(&mView, &eye, &lookat, &up);
		D3DXMatrixPerspectiveFovLH(&mProj,
			D3DX_PI / 4, // 시야각
			1280 / 720,  // 화면 비율
			1.f,		 // 전방 클립면
			100.f);		 // 후방 클립면


		effect->SetTechnique(technique);
		
		effect->Begin(
			NULL, // 셰이더 파일에서 최대 pass 수를 읽는다
			0); // 옵션
		// 첫번째 인자
		// 패스 수를 이펙트 파일로부터 읽어들이기 위해서 쓰기 가능한 변수를 지정한다.
		// 같은 메시에 대해서 렌더링하는 셰이더를 바꾸어 렌더링하는 멀티패스 렌더링을 할 경우
		// pPasses에 패스 수를 넣어, 그 횟수만 렌더링할 때 사용한다.
		// 패스 수를 직접 입력해 제어할 때는 패스 수를 조사하는 것이 불필요하다.

		// 두번째 인자
		// 이펙트에 의해 변경된 설정을 이펙트 종료 후 반환할지를 결정하는 플래그
		// 값이 0으로 아무것도 지정되지 않았을 때는 ID3DXEffect::Begin() 후에 
		// 이펙트에 의해 변경된 상태를 ID3DXEffect::End()가 끝나면 복원한다.
		// 복원하지 않을 경우 다음 값 중 하나 또는 양쪽 모두를 지정 가능하다.
		// D3DXFX_DONOTSAVESTATE = 디바이스 상태를 보존, 복원하지 않는다.
		// D3DXFX_DONOTSAVESHADERSTATE = 셰이더 디바이스 상태, 상수를 보존, 복원하지 않는다.
		
		effect->BeginPass(0);

		D3DXMATRIX m = mWorld * mView * mProj;
		effect->SetMatrix(mWVP, &m);
		// ID3DXEffect::SetMatrix()로 행렬을 설정할 경우에는
		// 셰이더 어셈블러와 달리, 행렬을 전치 행렬로 만들 필요가 없다.
		// HLSL 프로그램 안에서 D3DXMATRIX 그대로의 형태로 사용 가능하다.
		// HLSL 에서는 행렬에 벡터를 작용시킬 때 벡터를 왼쪽으로부터 작용시킨다.

		effect->SetTexture((D3DXHANDLE)"Tex", texture);
		// 문자열을 직접 지정해도 괜찮지만 그 순간마다 문자 해석 처리가 들어가므로
		// ID3DXEffect::GetParameterByName()을 사용하는것이 실행 속도가 빠르다

		mesh->DrawSubset(0);

		effect->EndPass();
		effect->End();
	}

	void Close()
	{
		effect->Release();
		texture->Release();
		mesh->Release();
	}
}