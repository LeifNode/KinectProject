#include "LineRenderer.h"
#include "LineSegment.h"
#include "d3dApp.h"
#include "D3DRenderer.h"

Shader* LineRenderer::pLineShader = NULL;
ID3D11Buffer* LineRenderer::pPerLineBuffer = NULL;

LineRenderer::LineRenderer()
	:mpVertexBuffer(NULL),
	mpIndexBuffer(NULL)
{

}

LineRenderer::~LineRenderer()
{
	ReleaseCOM(mpVertexBuffer);
	ReleaseCOM(mpIndexBuffer);
}

void LineRenderer::reloadPoints()
{
	if (Points.List.size() > 0)
	{
		ReleaseCOM(mpVertexBuffer);
		ReleaseCOM(mpIndexBuffer);

		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(XMFLOAT3) * (UINT)Points.List.size();
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &Points.List[0];

		HR(gpApplication->getRenderer()->device()->CreateBuffer(&bd, &initData, &mpVertexBuffer));

		UINT* indexArr = new UINT[(UINT)Points.List.size() * 2];
	
		//Line List indexing
		if (Points.List.size() > 1)
		{
			for (int i = 0; i < Points.List.size() - 1; i += 2)
			{
				if (i > 0)
					indexArr[i * 2] = i - 2; //Change to 2 for smoothing
				else
					indexArr[i * 2] = i;

				indexArr[i * 2 + 1] = i;
				indexArr[i * 2 + 2] = i + 1;

				if (i < Points.List.size() - 2)
					indexArr[i * 2 + 3] = i + 3; //Change to 3 for smoothing
				else
					indexArr[i * 2 + 3] = i + 1;
			}
		}

		//Line strip indexing
		/*if (Points.List.size() > 1)
		{
			for (int i = 0; i < Points.List.size() - 1; i++)
			{
				if (i > 0)
					indexArr[i * 4] = i - 1;
				else
					indexArr[i * 4] = i;

				indexArr[i * 4 + 1] = i;
				indexArr[i * 4 + 2] = i + 1;

				if (i < Points.List.size() - 2)
					indexArr[i * 4 + 3] = i + 2;
				else
					indexArr[i * 4 + 3] = i + 1;
			}
		}*/

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(UINT) * (UINT)Points.List.size() * 2;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA iinitData;

		iinitData.pSysMem = &indexArr[0];

		HR(gpApplication->getRenderer()->device()->CreateBuffer(&ibd, &iinitData, &mpIndexBuffer));

		delete [] indexArr;
	}
}

void LineRenderer::Initialize()
{
	D3DRenderer* renderer = gpApplication->getRenderer();

	ShaderInfo shaderInfo[] = {
		{ SHADER_TYPE_VERTEX, "VS" },
		{ SHADER_TYPE_GEOMETRY, "GS" },
		{ SHADER_TYPE_PIXEL, "PS" },
		{ SHADER_TYPE_NONE, NULL }
	};

	D3D11_INPUT_ELEMENT_DESC vertexDescription[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	pLineShader = renderer->loadShader(L"Shaders/line.fx", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ, vertexDescription, ARRAYSIZE(vertexDescription));

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CBPerLine);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	renderer->device()->CreateBuffer(&bd, NULL, &pPerLineBuffer);
}

void LineRenderer::DeInit()
{
	ReleaseCOM(pPerLineBuffer);
}

void LineRenderer::Render(D3DRenderer* renderer)
{
	if (mpVertexBuffer != NULL)
	{
		renderer->setShader(pLineShader);

		CBPerFrame perFrame = *renderer->getPerFrameBuffer();
		renderer->setPerFrameBuffer(perFrame);

		CBPerObject perObject;
		perObject.World = XMMatrixIdentity();
		perObject.WorldInvTranspose = XMMatrixInverse(NULL, XMMatrixTranspose(perObject.World));
		perObject.WorldViewProj = perObject.World * renderer->getPerFrameBuffer()->ViewProj;

		renderer->setPerObjectBuffer(perObject);

		CBPerLine perLine;
		perLine.Color = XMFLOAT4(2.0f, 0.0f, 0.0f, 1.0f);
		perLine.Thickness = 0.01f;
		perLine.PointCount = Points.List.size();

		renderer->context()->UpdateSubresource(pPerLineBuffer, 0, NULL, &perLine, 0, 0);
		renderer->setConstantBuffer(2, pPerLineBuffer);
		//renderer->setBlendState(true);
		//renderer->setDepthStencilState(D3DRenderer::Depth_Stencil_State_Particle);

		UINT stride = sizeof(XMFLOAT3);
		UINT offset = 0;
		renderer->context()->IASetVertexBuffers(0, 1, &mpVertexBuffer, &stride, &offset); 
		renderer->context()->IASetIndexBuffer(mpIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		
		renderer->context()->DrawIndexed((UINT)Points.List.size() * 2, 0, 0);

		renderer->setBlendState(false);
		renderer->setDepthStencilState(D3DRenderer::Depth_Stencil_State_Default);
	}
}