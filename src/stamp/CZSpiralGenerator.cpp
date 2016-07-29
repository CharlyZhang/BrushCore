
///  \file CZSpiralGenerator.h
///  \brief This is the file declare the Class CZSpiralGenerator.
///
///		
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-19
///  \note

#include "CZSpiralGenerator.h"
#include "../basic/CZ2DPoint.h"
#include "../basic/CZAffineTransform.h"
#include "../path/CZBezierNode.h"
#include "../CZUtil.h"
#include "../CZDefine.h"
#include <cmath>

using namespace std;

CZSpiralGenerator::CZSpiralGenerator(CZGLContext *ctx):CZStampGenerator(ctx)
{
	/// 创建属性
	density.title = "Density";
	density.value = 15;		///< ！先假设的
	density.minimumValue = 1;
	density.maximumValue = 20;
	density.conversionFactor = 1;
	//density.delegate = self;
	//(self.rawProperties)[@"density"] = density;
}

/// 拷贝一份当前生成器
CZSpiralGenerator* CZSpiralGenerator::copy()
{
	CZSpiralGenerator *ret = new CZSpiralGenerator(ptrGLContext);
	ret->seed = seed;
	ret->size = size;
	ret->scale = scale;
	ret->density = density;
	return ret;
}


/// 绘制图案
void CZSpiralGenerator::renderStamp(CZRandom* randomizer)
{
	if (!randomizer)
	{
		LOG_ERROR("randomizer is null\n");
		return;
	}

	if(ptrGLContext == NULL)
	{
		LOG_ERROR("ptrGLContext is NULL!\n");
		return;
	}

	vector<string> attributes, uniforms;
	attributes.push_back("inPosition");
	uniforms.push_back("mvpMat");
	uniforms.push_back("intensity");
	CZShader shader("basic","basicColor",attributes,uniforms);
	shader.begin();

	glUniformMatrix4fv(shader.getUniformLocation("mvpMat"),1,GL_FALSE,projMat);

	float dim = baseDimension - 20;

	for (int i = 0; i < density.value; i++)
	{
		CZ2DPoint center(20 + randomizer->nextFloat() * dim, 20 
			+ randomizer->nextFloat() * dim);

		float radius = CZUtil::Min(center.x, baseDimension - center.x);
		radius = CZUtil::Min(radius,CZUtil::Min(center.y, baseDimension - center.y));
		radius -= 2;

		GLfloat c = randomizer->nextFloat();
		glUniform1f(shader.getUniformLocation("intensity"),c);

		drawSpiral(center,radius,randomizer);
	}

	shader.end();
}

/// 绘制螺旋线
void CZSpiralGenerator::drawSpiral(const CZ2DPoint &center_, float radius_,CZRandom* randomizer)
{
	if (!randomizer)
	{
		LOG_ERROR("randomizer is null\n");
		return;
	}

	int         segments = 15;
	float       decay = 80;
	float       b = 1.0f - (decay / 100.f);
	float       a = (float) radius_ / pow(M_E, b * segments * M_PI_4);

	vector<CZBezierNode> nodes;
	CZ2DPoint lastOut(0,0);

	CZAffineTransform transform = CZAffineTransform::makeIdentity();
	transform.translate(center_.x, center_.y);
	transform.rotate(randomizer->nextFloat() * M_PI * 2);

	for (int segment = 0; segment <= segments; segment++)
	{
		float t = segment * M_PI_4;
		float f = a * pow((float)M_E, b * t);
		float x = f * cos(t);
		float y = f * sin(t);

		CZ2DPoint P3(x, y);

		// derivative
		float t0 = t - M_PI_4;
		float deltaT = (t - t0) / 3;

		float xPrime = a*b*pow((float)M_E,b*t)*cos(t) - a*pow((float)M_E,b*t)*sin(t);
		float yPrime = a*pow((float)M_E,b*t)*cos(t) + a*b*pow((float)M_E,b*t)*sin(t);

		CZ2DPoint P2 = P3 - (CZ2DPoint(xPrime, yPrime) * deltaT);
		CZ2DPoint P1 = P3 + (CZ2DPoint(xPrime, yPrime) * deltaT);

		P1 = transform.applyTo2DPoint(P1);
		P2 = transform.applyTo2DPoint(P2);
		P3 = transform.applyTo2DPoint(P3);
		CZBezierNode node(
			CZ3DPoint(P2.x,P2.y,0),
			CZ3DPoint(P3.x,P3.y,0),
			CZ3DPoint(P1.x,P1.y,0));
		nodes.push_back(node);


		lastOut = P1;
	}

	vector<CZ3DPoint> points;
	CZUtil::flattenNodes2Points(nodes,false,points);

	int n = (int)points.size();

	GLfloat w = randomizer->nextFloat()*9 +1;			///< 线大小原来是10以内
	glLineWidth(w);

#ifdef USE_OPENGL
	//glEnable(GL_LINE_SMOOTH);		///< 个人感觉还是不启用抗锯齿来得好
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

	GLuint mVertexBufferObject;
	// 装载顶点
	glGenBuffers(1, &mVertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, n * sizeof(CZ3DPoint), &points[0].x, GL_STREAM_DRAW);


	// 绑定顶点
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,2,GL_FLOAT, GL_FALSE, sizeof(CZ3DPoint),0);

	/// 绘制
	glDrawArrays(GL_LINE_STRIP,0,n);

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/// 消除
	glDeleteBuffers(1, &mVertexBufferObject);

	//glDisable(GL_LINE_SMOOTH);

#elif defined(USE_OPENGL_ES)
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(CZ3DPoint), &points[0].x);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_LINE_STRIP, 0, n);
	glDisableVertexAttribArray(0);
#endif
	CZCheckGLError();

	points.clear();
}