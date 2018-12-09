#include "pch.h"
#include "Terrain.h"

void Engine::Terrain::Shutdown()
{
		// Release the vertex and index buffer.
	ShutdownBuffers();

		// Release the terrain model.
	ShutdownTerrainModel();

		// Release the height map.
	ShutdownHeightMap();
}

void Engine::Terrain::Render(Matrix world, Matrix view, Matrix proj)
{
		// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(world, view, proj);
}

bool Engine::Terrain::Initialize(Shaders *Shader, const char *HMapFile, const wchar_t *TextureTerrain)
{
	this->Shader.reset(Shader);

	this->GetD3DDevice();
	if (FAILED(result = CreateWICTextureFromFile(Device, TextureTerrain, nullptr, &m_texture)))
		return false;

		// Initialize the terrain height map with the data from the bitmap file.
	if (!LoadBitmapHeightMap(HMapFile))
		return false;

		// Setup the X and Z coordinates for the height map as well as scale the terrain height by the height scale value.
	SetTerrainCoordinates();

		// Now build the 3D model of the terrain.
	if (!BuildTerrainModel())
		return false;

		// Calculate the texture coordinates.
	CalculateTextureCoordinates();

		// Load the rendering buffers with the terrain data.
	if (!InitializeBuffers())
		return false;

	// We can now release the height map since it is no longer needed in memory once the 3D terrain model has been built.
	ShutdownHeightMap();

		// Release the terrain model now that the rendering buffers have been loaded.
	ShutdownTerrainModel();

	return true;
}

bool Engine::Terrain::InitializeBuffers()
{
	int index = 0;
	float positionX = 0.f, positionZ = 0.f;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	D3D11_SAMPLER_DESC samplerDesc;

	GetD3DDevice();

		// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

		// Set the index count to the same as the vertex count.
	m_indexCount = m_vertexCount;

		// Create the vertex array.
	vertices = new Vertex[m_vertexCount];

		// Create the index array.
	auto indices = new ULONG[m_indexCount];

		// Load the vertex array and index array with 3D terrain model data.
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = model[i].Pos;
		vertices[i].texcoord = model[i].texcoord;
		indices[i] = i;
	}

		// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

		// Now create the vertex buffer.
	if (FAILED(result = Device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
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
	if (FAILED(result = Device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
		return false;

	if (FAILED(result = Shader->CompileShaderFromFile(Engine::Terrain::GetResPathW(&wstring(L"VertexShader.hlsl")), &string("ColorVertexShader"), &string("vs_4_0"), &vertexShaderBuffer)))
		return false;

	if (FAILED(result = Shader->CompileShaderFromFile(Engine::Terrain::GetResPathW(&wstring(L"PixelShader.hlsl")), &string("ColorPixelShader"), &string("ps_4_0"), &pixelShaderBuffer)))
		return false;

	if (FAILED(result = Device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader)))
		return false;

	if (FAILED(result = Device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader)))
		return false;

	const D3D11_INPUT_ELEMENT_DESC polygonLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
						 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

		// Get a count of the elements in the layout.
	auto numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

		// Create the vertex input layout.
	if (FAILED(result = Device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout)))
		return false;

		// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	SAFE_RELEASE(vertexShaderBuffer);
	SAFE_RELEASE(pixelShaderBuffer);

		// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		// Create the texture sampler state.
	if (FAILED(result = Device->CreateSamplerState(&samplerDesc, &m_sampleState)))
		return false;

		// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

		// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	if (FAILED(result = Device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer)))
		return false;

		// Release the arrays now that the buffers have been created and loaded.
	SAFE_DELETE(vertices);
	SAFE_DELETE(indices);

	return true;
}

void Engine::Terrain::ShutdownBuffers()
{
		// Release the index buffer.
	SAFE_RELEASE(m_indexBuffer);

		// Release the all buffer.
	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_matrixBuffer);
	SAFE_RELEASE(m_sampleState);
		// Release the layout.
	SAFE_RELEASE(m_layout);

		// Release the pixel shader.
	SAFE_RELEASE(m_pixelShader);

		// Release the vertex shader.
	SAFE_RELEASE(m_vertexShader);
}

void Engine::Terrain::RenderBuffers(Matrix world, Matrix view, Matrix proj)
{
	UINT stride = 0, offset = 0;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer *dataPtr = nullptr;

	GetD3DDeviceCon();

	stride = sizeof(Vertex);

		// Set the vertex buffer to active in the input assembler so it can be rendered.
	DeviceCon->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

		// Set the index buffer to active in the input assembler so it can be rendered.
	DeviceCon->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	DeviceCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DeviceCon->IASetInputLayout(m_layout);

		// Set the vertex and pixel shaders that will be used to render this triangle.
	DeviceCon->VSSetShader(m_vertexShader, NULL, 0);
	DeviceCon->PSSetShader(m_pixelShader, NULL, 0);

		// Set the sampler state in the pixel shader.
	DeviceCon->PSSetSamplers(0, 1, &m_sampleState);

		// Transpose the matrices to prepare them for the shader.
	auto worldMatrix = XMMatrixTranspose(world);
	auto viewMatrix = XMMatrixTranspose(view);
	auto projectionMatrix = XMMatrixTranspose(proj);

		// Lock the constant buffer so it can be written to.
	ThrowIfFailed(DeviceCon->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

		// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBuffer*)mappedResource.pData;

		// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

		// Unlock the constant buffer.
	DeviceCon->Unmap(m_matrixBuffer, 0);

		// Set shader texture resource in the pixel shader.
	DeviceCon->PSSetShaderResources(0, 1, &m_texture);

		// Finanly set the constant buffer in the vertex shader with the updated values.
	DeviceCon->VSSetConstantBuffers(0, 1, &m_matrixBuffer);

		// Render the triangle.
	DeviceCon->DrawIndexed(m_indexCount, 0, 0);

}

bool Engine::Terrain::LoadBitmapHeightMap(const char *TerrainBMPfile)
{
	int imageSize = 0, k = 0, index = 0;
	FILE *filePtr = nullptr;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	UCHAR *bitmapImage = nullptr;
	UCHAR height = 0;

		// Start by creating the array structure to hold the height map data.
	HMap = new HeightMap[m_terrainWidth * m_terrainHeight];

		// Open the bitmap map file in binary.
	if (fopen_s(&filePtr, TerrainBMPfile, "rb") != 0)
		return false;

		// Read in the bitmap file header.
	if (fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr) != 1)
		return false;

		// Read in the bitmap info header.
	if (fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr) != 1)
		return false;

		// Make sure the height map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	if ((bitmapInfoHeader.biHeight != m_terrainHeight) || (bitmapInfoHeader.biWidth != m_terrainWidth))
		return false;

	// Calculate the size of the bitmap image data.  
		// Since we use non-divide by 2 dimensions (eg. 257x257) we need to add an extra byte to each line.
	imageSize = m_terrainHeight * ((m_terrainWidth * 3) + 1);

		// Allocate memory for the bitmap image data.
	bitmapImage = new UCHAR[imageSize];

		// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

		// Read in the bitmap image data.
	if (fread(bitmapImage, 1, imageSize, filePtr) != imageSize)
		return false;

		// Close the file.
	if (fclose(filePtr) != 0)
		return false;

		// Read the image data into the height map array.
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
				// Bitmaps are upside down so load bottom to top into the height map array.
			index = (m_terrainWidth * (m_terrainHeight - 1 - j)) + i;

				// Get the grey scale pixel value from the bitmap image data at this location.
			height = bitmapImage[k];

				// Store the pixel value as the height at this point in the height map array.
			HMap[index].Pos.y = (float)height;

				// Increment the bitmap image data index.
			k += 3;
		}

			// Compensate for the extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
		k++;
	}

		// Release the bitmap image data now that the height map array has been loaded.
	SAFE_DELETE(bitmapImage);
	return true;
}

void Engine::Terrain::ShutdownHeightMap()
{
		// Release the height map array.
	SAFE_DELETE(HMap);
}

void Engine::Terrain::SetTerrainCoordinates()
{
		// Loop through all the elements in the height map array and adjust their coordinates correctly.
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			auto index = (m_terrainWidth * j) + i;

				// Set the X and Z coordinates.
			HMap[index].Pos.x = (float)i;
			HMap[index].Pos.z = -(float)j;

				// Move the terrain depth into the positive range.  For example from (0, -256) to (256, 0).
			HMap[index].Pos.z += (float)(m_terrainHeight - 1);

				// Scale the height.
			HMap[index].Pos.y /= m_heightScale;
		}
	}
}

bool Engine::Terrain::BuildTerrainModel()
{
	int index = 0, index1 = 0, index2 = 0, index3 = 0, index4 = 0;

		// Calculate the number of vertices in the 3D terrain model.
	m_vertexCount = (m_terrainHeight - 1) * (m_terrainWidth - 1) * 6;

		// Create the 3D terrain model array.
	model = new Model[m_vertexCount];

	// Load the 3D terrain model with the height map terrain data.
		// We will be creating 2 triangles for each of the four points in a quad.
	for (int j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (int i = 0; i < (m_terrainWidth - 1); i++)
		{
				// Get the indexes to the four points of the quad.
			index1 = (m_terrainWidth * j) + i;				// Upper left.
			index2 = (m_terrainWidth * j) + (i + 1);		// Upper right.
			index3 = (m_terrainWidth * (j + 1)) + i;		// Bottom left.
			index4 = (m_terrainWidth * (j + 1)) + (i + 1);	// Bottom right.

			// Now create two triangles for that quad.
				// Triangle 1 - Upper left.
			model[index].Pos.x = HMap[index1].Pos.x;
			model[index].Pos.y = HMap[index1].Pos.y;
			model[index].Pos.z = HMap[index1].Pos.z;
			model[index].texcoord = Vector2(0.f, 0.f);
			index++;

				// Triangle 1 - Upper right.
			model[index].Pos.x = HMap[index2].Pos.x;
			model[index].Pos.y = HMap[index2].Pos.y;
			model[index].Pos.z = HMap[index2].Pos.z;
			model[index].texcoord = Vector2(1.f, 0.f);
			index++;

				// Triangle 1 - Bottom left.
			model[index].Pos.x = HMap[index3].Pos.x;
			model[index].Pos.y = HMap[index3].Pos.y;
			model[index].Pos.z = HMap[index3].Pos.z;
			model[index].texcoord = Vector2(0.f, 1.f);
			index++;

				// Triangle 2 - Bottom left.
			model[index].Pos.x = HMap[index3].Pos.x;
			model[index].Pos.y = HMap[index3].Pos.y;
			model[index].Pos.z = HMap[index3].Pos.z;
			model[index].texcoord = Vector2(0.f, 1.f);
			index++;

				// Triangle 2 - Upper right.
			model[index].Pos.x = HMap[index2].Pos.x;
			model[index].Pos.y = HMap[index2].Pos.y;
			model[index].Pos.z = HMap[index2].Pos.z;
			model[index].texcoord = Vector2(1.f, 0.f);
			index++;

				// Triangle 2 - Bottom right.
			model[index].Pos.x = HMap[index4].Pos.x;
			model[index].Pos.y = HMap[index4].Pos.y;
			model[index].Pos.z = HMap[index4].Pos.z;
			model[index].texcoord = Vector2(1.f, 1.f);
			index++;
		}
	}

	return true;
}

void Engine::Terrain::ShutdownTerrainModel()
{
		// Release the terrain model data.
	SAFE_DELETE(model);
}

void Engine::Terrain::CalculateTextureCoordinates()
{
	int incrementCount = 0, tuCount = 0, tvCount = 0;
	float incrementValue = 0.f, tuCoordinate = 0.f, tvCoordinate = 0.f;

		// Calculate how much to increment the texture coordinates by.
	incrementValue = (float)TEXTURE_REPEAT / (float)m_terrainWidth;

		// Calculate how many times to repeat the texture.
	incrementCount = m_terrainWidth / TEXTURE_REPEAT;

		// Initialize the tv coordinate value.
	tvCoordinate = 1.0f;

		// Loop through the entire height map and calculate the tu and tv texture coordinates for each vertex.
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
				// Store the texture coordinate in the height map.
			HMap[(m_terrainHeight * j) + i].texcoord.x = tuCoordinate;
			HMap[(m_terrainHeight * j) + i].texcoord.y = tvCoordinate;

				// Increment the tu texture coordinate by the increment value and increment the index by one.
			tuCoordinate += incrementValue;
			tuCount++;

				// Check if at the far right end of the texture and if so then start at the beginning again.
			if (tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

			// Increment the tv texture coordinate by the increment value and increment the index by one.
		tvCoordinate -= incrementValue;
		tvCount++;

			// Check if at the top of the texture and if so then start at the bottom again.
		if (tvCount == incrementCount)
		{
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}
}
