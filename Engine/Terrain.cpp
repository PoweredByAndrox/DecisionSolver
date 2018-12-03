#include "pch.h"
#include "Terrain.h"

void Engine::Terrain::Shutdown()
{
	// Release the vertex and index buffer.
	ShutdownBuffers();
}

void Engine::Terrain::Render(ID3D11DeviceContext* deviceContext, Matrix world, Matrix view, Matrix proj)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext, world, view, proj);
}

bool Engine::Terrain::InitializeBuffers(Shaders *Shader)
{
	VertexType* vertices;
	unsigned long* indices;
	int index, i, j;
	float positionX, positionZ;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	auto device = DXUTGetD3D11Device();

	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 8;

	// Set the index count to the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
		return false;

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;

	// Initialize the index to the vertex array.
	index = 0;

	// Load the vertex and index arrays with the terrain data.
	for (j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (i = 0; i < (m_terrainWidth - 1); i++)
		{
			// LINE 1
			// Upper left.
			positionX = (float)i;
			positionZ = (float)(j + 1);

			vertices[index].position = Vector3(positionX, 0.0f, positionZ);
			vertices[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[index].position = Vector3(positionX, 0.0f, positionZ);
			vertices[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// LINE 2
			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[index].position = Vector3(positionX, 0.0f, positionZ);
			vertices[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;

			vertices[index].position = Vector3(positionX, 0.0f, positionZ);
			vertices[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// LINE 3
			// Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;

			vertices[index].position = Vector3(positionX, 0.0f, positionZ);
			vertices[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// Bottom left.
			positionX = (float)i;
			positionZ = (float)j;

			vertices[index].position = Vector3(positionX, 0.0f, positionZ);
			vertices[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// LINE 4
			// Bottom left.
			positionX = (float)i;
			positionZ = (float)j;

			vertices[index].position = Vector3(positionX, 0.0f, positionZ);
			vertices[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;

			// Upper left.
			positionX = (float)i;
			positionZ = (float)(j + 1);

			vertices[index].position = Vector3(positionX, 0.0f, positionZ);
			vertices[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[index] = index;
			index++;
		}
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.

	device = DXUTGetD3D11Device();
	if (FAILED(result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
		return false;

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	device = DXUTGetD3D11Device();
	if (FAILED(result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
		return false;

	if (FAILED(result = Shader->CompileShaderFromFile(Engine::Terrain::GetResPathW(&wstring(L"VertexShader.hlsl")), &string("ColorVertexShader"), &string("vs_4_0"), &vertexShaderBuffer)))
		return false;

	if (FAILED(result = Shader->CompileShaderFromFile(Engine::Terrain::GetResPathW(&wstring(L"PixelShader.hlsl")), &string("ColorPixelShader"), &string("ps_4_0"), &pixelShaderBuffer)))
		return false;

	if (FAILED(result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader)))
		return false;

	if (FAILED(result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader)))
		return false;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	if (FAILED(result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout)))
		return false;

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	if (FAILED(result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer)))
		return false;

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void Engine::Terrain::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

}

void Engine::Terrain::RenderBuffers(ID3D11DeviceContext* deviceContext, Matrix world, Matrix view, Matrix proj)
{
	UINT stride = sizeof(VertexType), offset = 0;

	//MatrixBufferType MBT;
	//MBT.world = world;
	//MBT.view = view;
	//MBT.projection = proj;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType *dataPtr = nullptr;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case a line list.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Transpose the matrices to prepare them for the shader.
	auto worldMatrix = XMMatrixTranspose(world);
	auto viewMatrix = XMMatrixTranspose(view);
	auto projectionMatrix = XMMatrixTranspose(proj);

	// Lock the constant buffer so it can be written to.
	if (FAILED(result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return;

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(0, 1, &m_matrixBuffer);

	// Render the triangle.
	deviceContext->DrawIndexed(m_indexCount, 0, 0);

}