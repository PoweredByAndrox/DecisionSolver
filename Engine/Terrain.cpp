#include "pch.h"
#include "Terrain.h"

void Engine::Terrain::Shutdown()
{
		// Release the terrain model.
	ShutdownTerrainModel();

		// Release the height map.
	ShutdownHeightMap();

	QTerrain->Shutdown();
}

void Engine::Terrain::Render(Matrix World, Matrix View, Matrix Proj)
{
	QTerrain->Render(World, View, Proj);
}

bool Engine::Terrain::Initialize(Frustum *frustum, const char *HMapFile, const wchar_t *TextureTerrain)
{
	Shader = make_unique<Shaders>();
	QTerrain = make_unique<QuadTerrain>();
	render = make_unique<Render_Buffer>();

	GetD3DDevice();

	ThrowIfFailed(render->CreateTexture(TextureTerrain));

		// Initialize the terrain height map with the data from the bitmap file.
	if (!LoadBitmapHeightMap(HMapFile))
	{
		throw exception("Terrain::Init::LoadBitmapHeightMap == false!!!");
		return false;
	}

		// Setup the X and Z coordinates for the height map as well as scale the terrain height by the height scale value.
	SetTerrainCoordinates();

		// Now build the 3D model of the terrain.
	if (!BuildTerrainModel())
	{
		throw exception("Terrain::Init::BuildTerrainModel == false!!!");
		return false;
	}

		// Calculate the texture coordinates.
	CalculateTextureCoordinates();

		// Load the rendering buffers with the terrain data.
	if (!InitializeBuffers())
	{
		throw exception("Terrain::Init::InitializeBuffers == false!!!");
		return false;
	}

	if (!QTerrain->Initialize(this, frustum))
	{
		throw exception("Terrain::Init::QT_Initialize == false!!!");
		return false;
	}

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

	GetD3DDevice();

		// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

		// Set the index count to the same as the vertex count.
	m_indexCount = m_vertexCount;

		// Create the vertex array.
	vertices = new Vertex[m_vertexCount];

		// Create the index array.
	auto indices = new UINT[m_indexCount];
	vector<UINT> indices_render;

		// Load the vertex array and index array with 3D terrain model data.
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = model[i].Pos;
		vertices[i].texcoord = model[i].texcoord;
		indices[i] = i;
		indices_render.push_back(indices[i]);
	}

	vector<wstring> FileShaders;
	FileShaders.push_back(*Engine::Terrain::GetResPathW(&wstring(L"VertexShader.hlsl")));
	FileShaders.push_back(*Engine::Terrain::GetResPathW(&wstring(L"PixelShader.hlsl")));

	vector<string> Functions, Version;
	Functions.push_back(string("ColorVertexShader"));
	Functions.push_back(string("ColorPixelShader"));

	Version.push_back(string("vs_4_0"));
	Version.push_back(string("ps_4_0"));

	if (!render->isInit())
		render->InitTerrain(sizeof(Vertex) * m_indexCount, vertices, indices_render, &FileShaders, &Functions, &Version);

	SAFE_DELETE(indices);

	return true;
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
			model[index].Pos = HMap[index1].Pos;
			model[index].texcoord = Vector2(0.f, 0.f);
			index++;

				// Triangle 1 - Upper right.
			model[index].Pos = HMap[index2].Pos;
			model[index].texcoord = Vector2(1.f, 0.f);
			index++;

				// Triangle 1 - Bottom left.
			model[index].Pos = HMap[index3].Pos;
			model[index].texcoord = Vector2(0.f, 1.f);
			index++;

				// Triangle 2 - Bottom left.
			model[index].Pos = HMap[index3].Pos;
			model[index].texcoord = Vector2(0.f, 1.f);
			index++;

				// Triangle 2 - Upper right.
			model[index].Pos = HMap[index2].Pos;
			model[index].texcoord = Vector2(1.f, 0.f);
			index++;

				// Triangle 2 - Bottom right.
			model[index].Pos = HMap[index4].Pos;
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

bool Engine::QuadTerrain::Initialize(Terrain *terrain, Frustum *frustum)
{
	int vertexCount = 0;
	float centerX = 0.f, centerZ = 0.f, width = 0.f;

	this->terrain.reset(terrain);
	this->frustum.reset(frustum);

		// Get the number of vertices in the terrain vertex array.
	vertexCount = terrain->GetVertexCount();

	// Store the total triangle count for the vertex list.
	m_triangleCount = vertexCount / 3;

	// Create a vertex array to hold all of the terrain vertices.
	m_vertexList = new Vertex[vertexCount];

		// Copy the terrain vertices into the vertex list.
	terrain->CopyVertexArray((void*)m_vertexList);

		// Calculate the center x,z and the width of the mesh.
	CalculateMeshDimensions(vertexCount, centerX, centerZ, width);

		// Create the parent node for the quad tree.
	m_parentNode = new NT;

		// Recursively build the quad tree based on the vertex list data and mesh dimensions.
	CreateTreeNode(m_parentNode, Vector2(centerX, centerZ), width);

		// Release the vertex list since the quad tree now has the vertices in each node.
	if (m_vertexList)
		SAFE_DELETE(m_vertexList);

	return true;
}

void Engine::QuadTerrain::Shutdown()
{
		// Recursively release the quad tree data.
	if (m_parentNode)
	{
		ReleaseNode(m_parentNode);
		SAFE_DELETE(m_parentNode);
	}

	Release();

	if (render.operator bool())
		SAFE_RELEASE(render);

	if (terrain.operator bool())
		SAFE_RELEASE(terrain);
}

void Engine::QuadTerrain::Render(Matrix World, Matrix View, Matrix Proj)
{
		// Render each node that is visible starting at the parent node and moving down the tree.
	RenderNode(m_parentNode, World, View, Proj);
}

void Engine::QuadTerrain::CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth)
{
	float maxWidth, maxDepth, minWidth, minDepth, width, depth, maxX, maxZ;

		// Initialize the center position of the mesh to zero.
	centerX = 0.0f;
	centerZ = 0.0f;

		// Sum all the vertices in the mesh.
	for (int i = 0; i < vertexCount; i++)
	{
		centerX += m_vertexList[i].position.x;
		centerZ += m_vertexList[i].position.z;
	}

		// And then divide it by the number of vertices to find the mid-point of the mesh.
	centerX = centerX / (float)vertexCount;
	centerZ = centerZ / (float)vertexCount;

		// Initialize the maximum and minimum size of the mesh.
	maxWidth = 0.0f;
	maxDepth = 0.0f;

	minWidth = fabsf(m_vertexList[0].position.x - centerX);
	minDepth = fabsf(m_vertexList[0].position.z - centerZ);

		// Go through all the vertices and find the maximum and minimum width and depth of the mesh.
	for (int i = 0; i < vertexCount; i++)
	{
		width = fabsf(m_vertexList[i].position.x - centerX);
		depth = fabsf(m_vertexList[i].position.z - centerZ);

		if (width > maxWidth) { maxWidth = width; }
		if (depth > maxDepth) { maxDepth = depth; }
		if (width < minWidth) { minWidth = width; }
		if (depth < minDepth) { minDepth = depth; }
	}

		// Find the absolute maximum value between the min and max depth and width.
	maxX = (float)max(fabs(minWidth), fabs(maxWidth));
	maxZ = (float)max(fabs(minDepth), fabs(maxDepth));

		// Calculate the maximum diameter of the mesh.
	meshWidth = max(maxX, maxZ) * 2.0f;
}

void Engine::QuadTerrain::CreateTreeNode(NT *node, Vector2 Pos, float width)
{
	int numTriangles = 0, vertexCount = 0.f, index = 0.f, vertexIndex = 0.f;
	float offsetX = 0.f, offsetZ = 0.f;
	Vertex *vertices = nullptr;
	ULONG *indices = nullptr;

		// Store the node position and size.
	node->texcoord.x = Pos.x;
	node->texcoord.y = Pos.y;
	node->width = width;

		// Initialize the triangle count to zero for the node.
	node->triangleCount = 0;

		// Initialize the vertex and index buffer to null.
	node->vertexBuffer = 0;
	node->indexBuffer = 0;

		// Initialize the children nodes of this node to null.
	node->nodes[0] = 0;
	node->nodes[1] = 0;
	node->nodes[2] = 0;
	node->nodes[3] = 0;

		// Count the number of triangles that are inside this node.
	numTriangles = CountTriangles(Pos, width);

		// Case 1: If there are no triangles in this node then return as it is empty and requires no processing.
	if (numTriangles == 0)
		return;

		// Case 2: If there are too many triangles in this node then split it into four equal sized smaller tree nodes.
	if (numTriangles > MAX_TRIANGLES)
	{
		for (int i = 0; i < 4; i++)
		{
				// Calculate the position offsets for the new child node.
			offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
			offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

				// See if there are any triangles in the new node.
			if (CountTriangles(Vector2((Pos.x + offsetX), (Pos.y + offsetZ)), (width / 2.0f)) > 0)
			{
					// If there are triangles inside where this new node would be then create the child node.
				node->nodes[i] = new NT;

					// Extend the tree starting from this new child node now.
				CreateTreeNode(node->nodes[i], Vector2((Pos.x + offsetX), (Pos.y + offsetZ)), (width / 2.0f));
			}
		}

		return;
	}

	// Case 3: If this node is not empty and the triangle count for it is less than the max then 
		// this node is at the bottom of the tree so create the list of triangles to store in it.
	node->triangleCount = numTriangles;

		// Calculate the number of vertices.
	vertexCount = numTriangles * 3;

		// Create the vertex array.
	vertices = new Vertex[vertexCount];

		// Create the index array.
	indices = new ULONG[vertexCount];

	node->vertexArray.resize(vertexCount);

		// Go through all the triangles in the vertex list.
	for (int i = 0; i < m_triangleCount; i++)
	{
			// If the triangle is inside this node then add it to the vertex array.
		if (IsTriangleContained(i, Pos, width))
		{
				// Calculate the index into the terrain vertex list.
			vertexIndex = i * 3;

			node->vertexArray[index] = m_vertexList[vertexIndex].position;

				// Get the three vertices of this triangle from the vertex list.
			vertices[index].position = m_vertexList[vertexIndex].position;
			vertices[index].texture = m_vertexList[vertexIndex].texture;
			indices[index] = index;
			index++;

			vertexIndex++;

			node->vertexArray[index] = m_vertexList[vertexIndex].position;

			vertices[index].position = m_vertexList[vertexIndex].position;
			vertices[index].texture = m_vertexList[vertexIndex].texture;
			indices[index] = index;
			index++;

			vertexIndex++;

			node->vertexArray[index] = m_vertexList[vertexIndex].position;

			vertices[index].position = m_vertexList[vertexIndex].position;
			vertices[index].texture = m_vertexList[vertexIndex].texture;
			indices[index] = index;
			index++;
		}
	}

		// Set up the description of the static vertex buffer.

	GetD3DDevice();

		// Now create the vertex buffers.
	node->vertexBuffer = terrain->getRenderObj()->CreateVB(int(sizeof(Vertex) * vertexCount), vertices);
	node->indexBuffer = terrain->getRenderObj()->CreateIB(int(sizeof(ULONG) * vertexCount), indices);

		// Release the vertex and index arrays now that the data is stored in the buffers in the node.
	SAFE_DELETE(vertices);
	SAFE_DELETE(indices);
}

int Engine::QuadTerrain::CountTriangles(Vector2 Pos, float width)
{
	int count = 0;

		// Go through all the triangles in the entire mesh and check which ones should be inside this node.
	for (int i = 0; i < m_triangleCount; i++)
			// If the triangle is inside the node then increment the count by one.
		if (IsTriangleContained(i, Pos, width))
			count++;

	return count;
}

bool Engine::QuadTerrain::IsTriangleContained(int index, Vector2 Pos, float width)
{
	float x1 = 0.f, z1 = 0.f, x2 = 0.f, z2 = 0.f, x3 = 0.f, z3 = 0.f,
	minimumX = 0.f, maximumX = 0.f, minimumZ = 0.f, maximumZ = 0.f, radius = 0.f;
	int vertexIndex = 0;

		// Calculate the radius of this node.
	radius = width / 2.0f;

		// Get the index into the vertex list.
	vertexIndex = index * 3;

		// Get the three vertices of this triangle from the vertex list.
	x1 = m_vertexList[vertexIndex].position.x;
	z1 = m_vertexList[vertexIndex].position.z;
	vertexIndex++;

	x2 = m_vertexList[vertexIndex].position.x;
	z2 = m_vertexList[vertexIndex].position.z;
	vertexIndex++;

	x3 = m_vertexList[vertexIndex].position.x;
	z3 = m_vertexList[vertexIndex].position.z;

		// Check to see if the minimum of the x coordinates of the triangle is inside the node.
	minimumX = min(x1, min(x2, x3));
	if (minimumX > (Pos.x + radius))
		return false;

		// Check to see if the maximum of the x coordinates of the triangle is inside the node.
	maximumX = max(x1, max(x2, x3));
	if (maximumX < (Pos.x - radius))
		return false;

		// Check to see if the minimum of the z coordinates of the triangle is inside the node.
	minimumZ = min(z1, min(z2, z3));
	if (minimumZ > (Pos.y + radius))
		return false;

		// Check to see if the maximum of the z coordinates of the triangle is inside the node.
	maximumZ = max(z1, max(z2, z3));
	if (maximumZ < (Pos.y - radius))
		return false;

	return true;
}

void Engine::QuadTerrain::ReleaseNode(NT *node)
{
		// Recursively go down the tree and release the bottom nodes first.
	for (int i = 0; i < 4; i++)
		if (node->nodes[i] != 0)
			ReleaseNode(node->nodes[i]);

		// Release the vertex buffer for this node.
	if (node->vertexBuffer)
		SAFE_RELEASE(node->vertexBuffer);

		// Release the index buffer for this node.
	if (node->indexBuffer)
		SAFE_RELEASE(node->indexBuffer);

		// Release the four child nodes.
	for (int i = 0; i < 4; i++)
		if (node->nodes[i])
			SAFE_DELETE(node->nodes[i]);
}

void Engine::QuadTerrain::RenderNode(NT *node, Matrix World, Matrix View, Matrix Proj)
{
	int count = 0;

	// Check to see if the node can be viewed, height doesn't matter in a quad tree.
		// If it can't be seen then none of its children can either so don't continue down the tree, this is where the speed is gained.
//	if (!frustum->CheckCube(node->texcoord.x, 0.0f, node->texcoord.y, (node->width / 2.0f)))
//		return;

		// If it can be seen then check all four child nodes to see if they can also be seen.
	for (int i = 0; i < 4; i++)
	{
		if (node->nodes[i] != 0)
		{
			count++;
			RenderNode(node->nodes[i], World, View, Proj);
		}
	}

		// If there were any children nodes then there is no need to continue as parent nodes won't contain any triangles to render.
	if (count != 0)
		return;

	GetD3DDeviceCon();

	vector<ID3D11Buffer *> Render = { node->vertexBuffer, node->indexBuffer };

	terrain->getRenderObj()->RenderTerrain(World, View, Proj, node->triangleCount * 3, Render, sizeof(Vertex));
}

bool Engine::QuadTerrain::GetHeightAtPosition(float positionX, float positionZ, float &height)
{
	float meshMinX = 0.f, meshMaxX = 0.f, meshMinZ = 0.f, meshMaxZ = 0.f;

	meshMinX = m_parentNode->texcoord.x - (m_parentNode->width / 2.0f);
	meshMaxX = m_parentNode->texcoord.x + (m_parentNode->width / 2.0f);

	meshMinZ = m_parentNode->texcoord.y - (m_parentNode->width / 2.0f);
	meshMaxZ = m_parentNode->texcoord.y + (m_parentNode->width / 2.0f);

		// Make sure the coordinates are actually over a polygon.
	if ((positionX < meshMinX) || (positionX > meshMaxX) || (positionZ < meshMinZ) || (positionZ > meshMaxZ))
		return false;

		// Find the node which contains the polygon for this position.
	FindNode(m_parentNode, positionX, positionZ, height);

	return true;
}

void Engine::QuadTerrain::FindNode(NT *node, float x, float z, float &height)
{
	float xMin = 0.f, xMax = 0.f, zMin = 0.f, zMax = 0.f,
	vertex1[3] = {0.f, 0.f, 0.f}, vertex2[3] = { 0.f, 0.f, 0.f }, vertex3[3] = { 0.f, 0.f, 0.f };
	int count = 0, index = 0;

		// Calculate the dimensions of this node.
	xMin = node->texcoord.x - (node->width / 2.0f);
	xMax = node->texcoord.x + (node->width / 2.0f);

	zMin = node->texcoord.y - (node->width / 2.0f);
	zMax = node->texcoord.y + (node->width / 2.0f);

		// See if the x and z coordinate are in this node, if not then stop traversing this part of the tree.
	if ((x < xMin) || (x > xMax) || (z < zMin) || (z > zMax))
		return;

	for (int i = 0; i < 4; i++)
	{
		if (node->nodes[i] != 0)
		{
			count++;
			FindNode(node->nodes[i], x, z, height);
		}
	}

		// If there were children nodes then return since the polygon will be in one of the children.
	if (count > 0)
		return;

	// If there were no children then the polygon must be in this node. Check all the polygons in this node to find 
		// the height of which one the polygon we are looking for.
	for (int i = 0; i < node->triangleCount; i++)
	{
		index = i * 3;
		vertex1[0] = node->vertexArray[index].x;
		vertex1[1] = node->vertexArray[index].y;
		vertex1[2] = node->vertexArray[index].z;

		index++;
		vertex2[0] = node->vertexArray[index].x;
		vertex2[1] = node->vertexArray[index].y;
		vertex2[2] = node->vertexArray[index].z;

		index++;
		vertex3[0] = node->vertexArray[index].x;
		vertex3[1] = node->vertexArray[index].y;
		vertex3[2] = node->vertexArray[index].z;

			// Check to see if this is the polygon we are looking for.
		if (CheckHeightOfTriangle(x, z, height, vertex1, vertex2, vertex3))
			// If this was the triangle then quit the function and the height will be returned to the calling function.
			return;
	}
}

bool Engine::QuadTerrain::CheckHeightOfTriangle(float x, float z, float &height, float v0[3], float v1[3], float v2[3])
{
	float startVector[3], directionVector[3], edge1[3], edge2[3], normal[3],
	Q[3], e1[3], e2[3], e3[3], edgeNormal[3], temp[3],
	magnitude, D, denominator, numerator, t, determinant;

		// Starting position of the ray that is being cast.
	startVector[0] = x;
	startVector[1] = 0.0f;
	startVector[2] = z;

		// The direction the ray is being cast.
	directionVector[0] = 0.0f;
	directionVector[1] = -1.0f;
	directionVector[2] = 0.0f;

		// Calculate the two edges from the three points given.
	edge1[0] = v1[0] - v0[0];
	edge1[1] = v1[1] - v0[1];
	edge1[2] = v1[2] - v0[2];

	edge2[0] = v2[0] - v0[0];
	edge2[1] = v2[1] - v0[1];
	edge2[2] = v2[2] - v0[2];

		// Calculate the normal of the triangle from the two edges.
	normal[0] = (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]);
	normal[1] = (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]);
	normal[2] = (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]);

	magnitude = (float)sqrt((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]));
	normal[0] = normal[0] / magnitude;
	normal[1] = normal[1] / magnitude;
	normal[2] = normal[2] / magnitude;

		// Find the distance from the origin to the plane.
	D = ((-normal[0] * v0[0]) + (-normal[1] * v0[1]) + (-normal[2] * v0[2]));

		// Get the denominator of the equation.
	denominator = ((normal[0] * directionVector[0]) + (normal[1] * directionVector[1]) + (normal[2] * directionVector[2]));

		// Make sure the result doesn't get too close to zero to prevent divide by zero.
	if (fabs(denominator) < 0.0001f)
		return false;

		// Get the numerator of the equation.
	numerator = -1.0f * (((normal[0] * startVector[0]) + (normal[1] * startVector[1]) + (normal[2] * startVector[2])) + D);

		// Calculate where we intersect the triangle.
	t = numerator / denominator;

		// Find the intersection vector.
	Q[0] = startVector[0] + (directionVector[0] * t);
	Q[1] = startVector[1] + (directionVector[1] * t);
	Q[2] = startVector[2] + (directionVector[2] * t);

		// Find the three edges of the triangle.
	e1[0] = v1[0] - v0[0];
	e1[1] = v1[1] - v0[1];
	e1[2] = v1[2] - v0[2];

	e2[0] = v2[0] - v1[0];
	e2[1] = v2[1] - v1[1];
	e2[2] = v2[2] - v1[2];

	e3[0] = v0[0] - v2[0];
	e3[1] = v0[1] - v2[1];
	e3[2] = v0[2] - v2[2];

		// Calculate the normal for the first edge.
	edgeNormal[0] = (e1[1] * normal[2]) - (e1[2] * normal[1]);
	edgeNormal[1] = (e1[2] * normal[0]) - (e1[0] * normal[2]);
	edgeNormal[2] = (e1[0] * normal[1]) - (e1[1] * normal[0]);

		// Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
	temp[0] = Q[0] - v0[0];
	temp[1] = Q[1] - v0[1];
	temp[2] = Q[2] - v0[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

		// Check if it is outside.
	if (determinant > 0.001f)
		return false;

		// Calculate the normal for the second edge.
	edgeNormal[0] = (e2[1] * normal[2]) - (e2[2] * normal[1]);
	edgeNormal[1] = (e2[2] * normal[0]) - (e2[0] * normal[2]);
	edgeNormal[2] = (e2[0] * normal[1]) - (e2[1] * normal[0]);

		// Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
	temp[0] = Q[0] - v1[0];
	temp[1] = Q[1] - v1[1];
	temp[2] = Q[2] - v1[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

		// Check if it is outside.
	if (determinant > 0.001f)
		return false;

		// Calculate the normal for the third edge.
	edgeNormal[0] = (e3[1] * normal[2]) - (e3[2] * normal[1]);
	edgeNormal[1] = (e3[2] * normal[0]) - (e3[0] * normal[2]);
	edgeNormal[2] = (e3[0] * normal[1]) - (e3[1] * normal[0]);

		// Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
	temp[0] = Q[0] - v2[0];
	temp[1] = Q[1] - v2[1];
	temp[2] = Q[2] - v2[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

		// Check if it is outside.
	if (determinant > 0.001f)
		return false;

		// Now we have our height.
	height = Q[1];

	return true;
}