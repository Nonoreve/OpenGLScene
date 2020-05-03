#include "geometry/ObjMesh.h"
#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// https://github.com/opengl-tutorials/ogl/blob/master/common/objloader.cpp
bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
) {
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if (file == NULL) {

		printf("Impossible d'ouvrir le fichier (Mauvais repertoire ?)");

		//exit(3);

		//getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("Le fichier n'est pas lisible, soit ce n'est pas un obj soit les options cochee ne sont pas bonnes \n");
				fclose(file);
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}
	fclose(file);


	return true;
}


///www.opengl-tutorial.org/fr/beginners-tutorials/tutorial-7-model-loading/

/*
Lors des test, ne semble marcher qu'1 fois sur 2 avec pourtant le meme code

*/
ObjMesh::ObjMesh(const char * path) : Geometry()
{


	std::vector<glm::vec3> o_vertices;
	std::vector<glm::vec2>  o_uvs;
	std::vector<glm::vec3> o_normals;


	loadOBJ(path, o_vertices, o_uvs, o_normals);


	m_nbVertices = o_vertices.size();

	m_vertices = (float*)malloc(sizeof(float) * 3 * m_nbVertices);
	m_normals = (float*)malloc(sizeof(float) * 3 * 3 * o_normals.size());
	m_uvs = (float*)malloc(sizeof(float) * 2 * 3 * o_uvs.size());


	int j = 0;
	for (int i = 0; i < o_vertices.size(); i++) {

		m_vertices[j] = o_vertices[i].x;
		j++;
		m_vertices[j] = o_vertices[i].y;
		j++;
		m_vertices[j] = o_vertices[i].z;
		j++;
	}


	j = 0;
	for (int i = 0; i < o_normals.size(); i++) {

		m_normals[j] = o_normals[i].x;
		j++;
		m_normals[j] = o_normals[i].y;
		j++;
		m_normals[j] = o_normals[i].z;
		j++;
	}


	j = 0;
	for (int i = 0; i < o_uvs.size(); i++) {

		m_uvs[j] = o_uvs[i].x;
		j++;
		m_uvs[j] = o_uvs[i].y;
		j++;
	}

	/*
	if (std::string(path) == "../../obj/plan.obj") {


	std::cout << "vertices" << std::endl;
	for (int i = 0; i < o_vertices.size(); i++) {
	std::cout << o_vertices[i].x << std::endl;
	std::cout << o_vertices[i].y << std::endl;
	std::cout << o_vertices[i].z << std::endl;
	std::cout << std::endl;
	}

	std::cout << "normals" << std::endl;
	for (int i = 0; i < o_normals.size(); i++) {
	std::cout << o_normals[i].x << std::endl;
	std::cout << o_normals[i].y << std::endl;
	std::cout << o_normals[i].z << std::endl;
	std::cout << std::endl;
	}

	std::cout << "uvs" << std::endl;
	for (int i = 0; i < o_uvs.size(); i++) {
	std::cout << o_uvs[i].x << std::endl;
	std::cout << o_uvs[i].y << std::endl;
	std::cout << std::endl;
	}

	}
	*/

	//printf("\n Ouverture de fichier termine \n");
	//printf("Nous avons : %d nb Vertices", getNbVertices());


}