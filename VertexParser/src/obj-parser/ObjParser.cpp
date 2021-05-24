#include "ObjParser.h"

#include <iostream>
#include <fstream>
#include <sstream>


std::string delimiter = " ";
std::string delimiterF = "//";
std::string delimiterVT = "/";

struct Vec3i {
	int x;
	int y;
	int z;
};

struct Vec3iv {
	Vec3i p1;
	Vec3i p2;
	Vec3i p3;
};

void SplitLine_V_VN_VT_(float* res, std::string& line, std::string& frst_smbl)
{

	size_t pos = 0;
	std::string token;

	line.erase(0, frst_smbl.length() + 1);

	while ((pos = line.find(delimiter)) != std::string::npos) {
		token = line.substr(0, pos);

		*res = std::stof(token);
		line.erase(0, pos + delimiter.length()); // truncates string

		res++;
	}

	*res = std::stof(line);
}

void SplitLine_F_(Vec3i* vec, std::string& line, std::string& frst_smbl)
{
	size_t pos = 0;
	std::string token;

	int* res = (int*) vec;

	line.erase(0, frst_smbl.length() + 1);

	while ((pos = line.find(delimiter)) != std::string::npos) {
		token = line.substr(0, pos);

		*res = std::stoi(token.substr(0, token.find(delimiterF)));

		line.erase(0, pos + delimiter.length()); // truncates string

		res++;
	}

	*res = std::stoi(line.substr(0, line.find(delimiterF)));

}

void SplitLine_F_V_VT_(Vec3iv* vec, std::string& line, std::string& frst_smbl)
{
	size_t pos = 0;
	size_t posInner = 0;
	std::string token;
	std::string tokenInner;

	int* res = (int*)vec;

	line.erase(0, frst_smbl.length() + 1);

	while ((pos = line.find(delimiter)) != std::string::npos) {
		token = line.substr(0, pos);
		
		//std::cout << "token = " << token << std::endl;

		while ((posInner = token.find(delimiterVT)) != std::string::npos) {
			tokenInner = token.substr(0, posInner);

			//std::cout << "tokenInner = " << tokenInner << std::endl;


			*res = std::stof(tokenInner);
			token.erase(0, posInner + delimiterVT.length()); // truncates string

			res++;
		}

		token = token.substr(0, token.find(delimiterVT));

		//std::cout << "tokenInner = " << token << std::endl;

		*res = std::stof(token);
		res++;

		line.erase(0, pos + delimiter.length()); // truncates string from begining

	}

	line = line.substr(0, line.find(delimiter));

	//std::cout << "token = " << line << std::endl;

	while ((posInner = line.find(delimiterVT)) != std::string::npos) {
		tokenInner = line.substr(0, posInner);

		//std::cout << "tokenInner = " << tokenInner << std::endl;


		*res = std::stof(tokenInner);
		line.erase(0, posInner + delimiterVT.length()); // truncates string

		res++;
	}

	line = line.substr(0, line.find(delimiterVT));

	//std::cout << "tokenInner = " << token << std::endl;

	*res = std::stof(line);

	res++;

	//*res = std::stoi(line.substr(0, line.find(delimiterF)));

}

parser::Vertices* parser::obj::ObjParser::Parse_P_N_T(const std::string& filepath)
{
	std::fstream stream(filepath);

	std::string line;
	std::stringstream ss[2];

	std::string v_str("v");
	std::string vn_str("vn");
	std::string vt_str("vt");
	std::string f_str("f");

	int v_count = 0;
	int f_count = 0;
	int t_count = 0;


	// # 1st iteration cycle - find out length of pos and norm arrays

	while (getline(stream, line))
	{

		if (line.length() == 0)
		{
			continue;
		}

		if (line.find(vt_str) == 0)
		{
			t_count++;
		}
		else if (line.find(vn_str) == 0)
		{
			v_count++;
		}
		else if (line.find(f_str) == 0)
		{
			f_count++;
		}
	}

	/*std::cout << "v_count = " << v_count << std::endl;
	std::cout << "f_count = " << f_count << std::endl;
	std::cout << "t_count = " << t_count << std::endl;*/

	// # 2st iteration cycle - allocate memory, fill additional arrays, copy from them to main array


	float* vertices = new float[v_count * 3];
	float* normals  = new float[v_count * 3];
	float* textures = new float[t_count * 2];

	float* vert_p = vertices;
	float* norm_p = normals;
	float* text_p = textures;

	//int ccc = 0;

	Vertices* resVertices = new Vertices(f_count * 3 * (3 + 3 + 2) * sizeof(float), f_count); // triangles_count * triangle,  triangle = 3 verts * (3 pos + 3 norms + 2 uv) * 4 bytes
	float* data = (float*)resVertices->GetData();

	int data_index = 0;

	stream.clear();                 // clear fail and eof bits
	stream.seekg(0, std::ios::beg); // back to the start!


	while (getline(stream, line))
	{
		//	std::cout << "line = " << line << std::endl;

		if (line.length() == 0)
		{
			continue;
		}

		if (line.find(vt_str) == 0)
		{
		//	std::cout << ccc << " line = " << line << std::endl;
			SplitLine_V_VN_VT_(text_p, line, vt_str);
			text_p += 2;

		//	std::cout << *(text_p - 2) << " " << *(text_p - 1) << std::endl;

		//	ccc++;
		}
		else if (line.find(vn_str) == 0)
		{
			SplitLine_V_VN_VT_(norm_p, line, vn_str);
			norm_p += 3;
		}
		else if (line.find(v_str) == 0)
		{
			SplitLine_V_VN_VT_(vert_p, line, v_str);
			vert_p += 3;
		}
		else if (line.find(f_str) == 0)
		{
			Vec3iv indices;
			SplitLine_F_V_VT_(&indices, line, f_str);

		/*	if (ccc < 10)
			{
				std::cout << indices.p1.x << "/" << indices.p1.y << "/" << indices.p1.z << " "
					<< indices.p2.x << "/" << indices.p2.y << "/" << indices.p2.z << " "
					<< indices.p3.x << "/" << indices.p3.y << "/" << indices.p3.z << std::endl;

				ccc++;
			}*/

			

			// vertex 1
			int vertex1 = (indices.p1.x - 1) * 3;
			int textur1 = (indices.p1.y - 1) * 2;
			int normal1 = (indices.p1.z - 1) * 3;


			data[data_index++] = vertices[vertex1 + 0];
			data[data_index++] = vertices[vertex1 + 1];
			data[data_index++] = vertices[vertex1 + 2];

			data[data_index++] = normals[normal1 + 0];
			data[data_index++] = normals[normal1 + 1];
			data[data_index++] = normals[normal1 + 2];

			data[data_index++] = textures[textur1 + 0];
			data[data_index++] = textures[textur1 + 1];

			// vertex 2
			vertex1 = (indices.p2.x - 1) * 3;
			textur1 = (indices.p2.y - 1) * 2;
			normal1 = (indices.p2.z - 1) * 3;


			data[data_index++] = vertices[vertex1 + 0];
			data[data_index++] = vertices[vertex1 + 1];
			data[data_index++] = vertices[vertex1 + 2];

			data[data_index++] = normals[normal1 + 0];
			data[data_index++] = normals[normal1 + 1];
			data[data_index++] = normals[normal1 + 2];

			data[data_index++] = textures[textur1 + 0];
			data[data_index++] = textures[textur1 + 1];

			// vertex 3
			
			vertex1 = (indices.p3.x - 1) * 3;
			textur1 = (indices.p3.y - 1) * 2;
			normal1 = (indices.p3.z - 1) * 3;


			data[data_index++] = vertices[vertex1 + 0];
			data[data_index++] = vertices[vertex1 + 1];
			data[data_index++] = vertices[vertex1 + 2];

			data[data_index++] = normals[normal1 + 0];
			data[data_index++] = normals[normal1 + 1];
			data[data_index++] = normals[normal1 + 2];

			data[data_index++] = textures[textur1 + 0];
			data[data_index++] = textures[textur1 + 1];

		}
	}

	/*for (int i = 0; i < v_count * 3; i += 3)
	{
		std::cout << "Vertex " << i / 3 << " "
			<< " x_pos = " << vertices[i + 0]
			<< " y_pos = " << vertices[i + 1]
			<< " z_pos = " << vertices[i + 2]
			<< std::endl;
	}*/

	/*for (int i = t_count * 2 - 20; i < t_count * 2; i += 2)
	{
		std::cout << "Textures " << i / 2 << " "
			<< " u_pos = " << textures[i + 0]
			<< " v_pos = " << textures[i + 1]
			<< std::endl;
	}*/
	// 17767 - 8857
	/*for (int i = 0; i < v_count * 3; i += 3)
	{
		std::cout << "Normal " << i / 3 << " "
			<< " x_pos = " << normals[i + 0]
			<< " y_pos = " << normals[i + 1]
			<< " z_pos = " << normals[i + 2]
			<< std::endl;
	}*/


	delete[] vertices;
	delete[] normals;
	delete[] textures;

	return resVertices;
}

parser::Vertices* parser::obj::ObjParser::Parse(const std::string& filepath)
{
	std::fstream stream(filepath);

	std::string line;
	std::stringstream ss[2];

	std::string v_str("v");
	std::string vn_str("vn");
	std::string f_str("f");

	int v_count = 0;
	int f_count = 0;

	// # 1st iteration cycle - find out length of pos and norm arrays

	while (getline(stream, line))
	{

		if (line.length() == 0)
		{
			continue;
		}

		if (line.find(vn_str) == 0)
		{
			v_count++;
		}
		else if (line.find(f_str) == 0)
		{
			f_count++;
		}
	}

	//std::cout << "v_count = " << v_count << std::endl;
	//std::cout << "f_count = " << f_count << std::endl;

	// # 2st iteration cycle - allocate memory, fill additional arrays, copy from them to main array

	float* vertices = new float[v_count * 3];
	float* normals = new float[v_count * 3];

	float* vert_p = vertices;
	float* norm_p = normals;

	Vertices* resVertices = new Vertices(f_count * 3 * (3 + 3) * sizeof(float), f_count); // triangles_count * triangle,  triangle = 3 verts * (3 pos + 3 norms) * 4 bytes
	float* data = (float*) resVertices->GetData();

	int data_index = 0;

	stream.clear();                 // clear fail and eof bits
	stream.seekg(0, std::ios::beg); // back to the start!

	while (getline(stream, line))
	{
	//	std::cout << "line = " << line << std::endl;

		if (line.length() == 0)
		{
			continue;
		}
		
		if (line.find(vn_str) == 0)
		{
			SplitLine_V_VN_VT_(norm_p, line, vn_str);
			norm_p += 3;
		}
		else if (line.find(v_str) == 0)
		{
			SplitLine_V_VN_VT_(vert_p, line, v_str);
			vert_p += 3;
		}
		else if (line.find(f_str) == 0)
		{
			Vec3i indices;
			SplitLine_F_(&indices, line, f_str);

			// vertex 1
			int vertex1 = (indices.x - 1) * 3;
			int normal1 = (indices.x - 1) * 3;



			data[data_index++] = vertices[vertex1 + 0];
			data[data_index++] = vertices[vertex1 + 1];
			data[data_index++] = vertices[vertex1 + 2];

			data[data_index++] = normals[normal1 + 0];
			data[data_index++] = normals[normal1 + 1];
			data[data_index++] = normals[normal1 + 2];


			// vertex 2
			vertex1 = (indices.y - 1) * 3;
			normal1 = (indices.y - 1) * 3;

			data[data_index++] = vertices[vertex1 + 0];
			data[data_index++] = vertices[vertex1 + 1];
			data[data_index++] = vertices[vertex1 + 2];

			data[data_index++] = normals[normal1 + 0];
			data[data_index++] = normals[normal1 + 1];
			data[data_index++] = normals[normal1 + 2];

			// vertex 3
			vertex1 = (indices.z - 1) * 3;
			normal1 = (indices.z - 1) * 3;

			data[data_index++] = vertices[vertex1 + 0];
			data[data_index++] = vertices[vertex1 + 1];
			data[data_index++] = vertices[vertex1 + 2];

			data[data_index++] = normals[normal1 + 0];
			data[data_index++] = normals[normal1 + 1];
			data[data_index++] = normals[normal1 + 2];


		}
	}

//	std::cout << "out of loop = " << std::endl;

	/*for (int i = 0; i < v_count * 3; i += 3)
	{
		std::cout << "Vertex " << i / 3 << " "
			<< " x_pos = " << vertices[i + 0]
			<< " y_pos = " << vertices[i + 1]
			<< " z_pos = " << vertices[i + 2]
			<< std::endl;
	}

	for (int i = 0; i < v_count * 3; i += 3)
	{
		std::cout << "Normal " << i / 3 << " "
			<< " x_pos = " << normals[i + 0]
			<< " y_pos = " << normals[i + 1]
			<< " z_pos = " << normals[i + 2]
			<< std::endl;
	}*/

	delete[] vertices;
	delete[] normals;

	//std::cout << "delete = " << std::endl;


	return resVertices;
}
