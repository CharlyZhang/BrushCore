
///  \file CZPath.cpp
///  \brief This is the file implement the Class CPath.
///
///		(description)
///
///  \version	1.0.0
///	 \author	Charly Zhang<chicboi@hotmail.com>
///  \date		2014-09-11
///  \note

#include "CZPath.h"
#include "../basic/CZAffineTransform.h"
#include "../basic/CZRandom.h"
#include "../brush/CZBrush.h"
#include "../CZUtil.h"
#include "CZBezierSegment.h"
#include <cmath>			//for fabs()...
 
using namespace std;

CZPath::CZPath(vector<CZBezierNode> *nodes_ /* = NULL */)
{
	reset();

	if(nodes_ != NULL)
	{
		size_t n = nodes_->size();
		for(size_t i=0; i<n; i++) nodes.push_back((*nodes_)[i]);
	}
}
CZPath::~CZPath()
{
	nodes.clear();
	points.clear();
	sizes.clear();
	angles.clear();
	alphas.clear();
}

/// 重置轨迹
void CZPath::reset()
{
	nodes.clear();
	points.clear();
	sizes.clear();
	angles.clear();
	alphas.clear();
	
	ptrBrush = NULL;
	scale = 1.0f;
	remainder = 0.0f;
	closed = false;
	limitBrushSize = false;
}

/// 设置所有结点
void CZPath::setNodes(const vector<CZBezierNode> &nodes_)
{
	nodes.clear();

	size_t n = nodes_.size();
	for(size_t i=0; i<n; i++) nodes.push_back(nodes_[i]);
}

/// 获得所有控制点
const vector<CZBezierNode>& CZPath::getNodes() const
{
	return nodes;
}
/// 添加结点
void CZPath::addNode(CZBezierNode &node)
{
	nodes.push_back(node);
}

/// 设置是否封闭
void CZPath::setClosed(bool isClosed)
{
	if (isClosed && nodes.size() < 2)
	{
		LOG_WARN("cannot set closed, control nodes number is less than 2");
		return;
	}

	if (isClosed)
	{
		CZBezierNode firstNode = nodes[0];
		CZBezierNode lastNode = closed ? nodes[0] : nodes.back();
		if (firstNode.anchorPoint == lastNode.anchorPoint)
		{
			CZBezierNode closedNode(lastNode.inPoint,firstNode.anchorPoint,firstNode.outPoint);
			nodes[0] = closedNode;
			nodes.pop_back();
		}
	}

	closed = isClosed;
}
bool CZPath::isClosed() const
{
	return closed;
}

/// 绘制轨迹（返回所有数据的范围）
CZRect CZPath::paint(CZRandom *randomizer_)
{   
	if(randomizer_ == NULL) 
	{
		LOG_ERROR("randomizer is NULL");
		return CZRect();
	}

	points.clear();
	sizes.clear();
	alphas.clear();
	angles.clear();

	if (nodes.size() == 1) 
	{
		paintStamp(randomizer_);
	} 
	else 
	{
		vector<CZ3DPoint> linePoints;		/// <贝塞尔曲线的绘制点
		int numPoints = flattenNodes2Points(nodes,closed,linePoints);

		if(ptrBrush == NULL) 
		{
			LOG_ERROR("ptrBrush is NULL\n");
			return CZRect();
		}

		for (int ix = 0; ix < numPoints - 1; ix++) 
		{
			this->paintBetweenPoints(linePoints[ix],linePoints[ix+1],randomizer_);
		}
	}

	return drawData();

}

/// 获取随机数器（根据该轨迹的笔刷的生成器）
CZRandom *CZPath::getRandomizer()
{
	return ptrBrush->getRandomizer();
}

/// 拷贝构造函数
CZPath& CZPath::operator=(const CZPath& path_)
{
	setNodes(path_.getNodes());
	setClosed(path_.isClosed());
	return *this;
}


/// 定义CZPath最终绘制数据格式
typedef struct 
{
	float     x, y;
	float     s, t;
	float     a;
} vertexData;

/// 绘制数据
/// 
///		以最小粒度的离散点(points_)为中心，形成小矩形，委托render绘制，并将此矩形数据返回。
///
CZRect CZPath::drawData()
{
	size_t iPointSize = points.size();
    CZRect dataBounds = CZRect(0.0f,0.0f,0.0f,0.0f);
    if (iPointSize <= 0) return dataBounds;
    
	vertexData *vertexD = new vertexData[iPointSize * 4 + (iPointSize - 1) * 2];

	int n = 0;
	for (int i = 0; i < iPointSize; i++) 
	{
		CZ2DPoint result = points[i];
		float angle = angles[i];
		float size = sizes[i] / 2;
		float alpha = alphas[i];

		// get the four corners of the dot
		CZRect rect = CZRect(result.x - size, result.y - size, size*2, size*2);
		CZ2DPoint a = CZ2DPoint(rect.getMinX(), rect.getMinY());
		CZ2DPoint b = CZ2DPoint(rect.getMaxX(), rect.getMinY());
		CZ2DPoint c = CZ2DPoint(rect.getMinX(), rect.getMaxY());
		CZ2DPoint d = CZ2DPoint(rect.getMaxX(), rect.getMaxY());

		// rotate the dot
		CZ2DPoint center = rect.getCenter();
		CZAffineTransform t = CZAffineTransform::makeFromTranslation(center.x, center.y);
		t.rotate(angle);
		t.translate(-center.x, -center.y);

		a = t.applyTo2DPoint(a);
		b = t.applyTo2DPoint(b);
		c = t.applyTo2DPoint(c);
		d = t.applyTo2DPoint(d);

		// update the bounds box of data
		CZRect boxBounds = t.applyToRect(rect);
		dataBounds = dataBounds.unionWith(boxBounds.expandToIntergral());

		if (n != 0) {
			// dup 1st vertex
			vertexD[n].x = a.x;
			vertexD[n].y = a.y;
			vertexD[n].s = 0;
			vertexD[n].t = 0;
			vertexD[n].a = alpha;
			n++;
		}

		vertexD[n].x = a.x;
		vertexD[n].y = a.y;
		vertexD[n].s = 0;
		vertexD[n].t = 0;
		vertexD[n].a = alpha;
		n++;

		vertexD[n].x = b.x;
		vertexD[n].y = b.y;
		vertexD[n].s = 1;
		vertexD[n].t = 0;
		vertexD[n].a = alpha;
		n++;

		vertexD[n].x = c.x;
		vertexD[n].y = c.y;
		vertexD[n].s = 0;
		vertexD[n].t = 1;
		vertexD[n].a = alpha;
		n++;

		vertexD[n].x = d.x;
		vertexD[n].y = d.y;
		vertexD[n].s = 1;
		vertexD[n].t = 1;
		vertexD[n].a = alpha;
		n++;

		if (i != (iPointSize - 1)) {
			// dup last vertex
			vertexD[n].x = d.x;
			vertexD[n].y = d.y;
			vertexD[n].s = 1;
			vertexD[n].t = 1;
			vertexD[n].a = alpha;
			n++;
		}
	}
	
#ifdef USE_OPENGL_ES
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertexData), &vertexD[0].x);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(vertexData), &vertexD[0].s);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_TRUE, sizeof(vertexData), &vertexD[0].a);
		glEnableVertexAttribArray(2);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, n);
	#endif

#ifdef USE_OPENGL
		
	/*	// 对于opengl属性数组形式， 顶点位置必须通过以下方式导入
		glEnableClientState (GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT , sizeof(vertexData), &data[0].x);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(vertexData), &data[0].s);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_TRUE, sizeof(vertexData), &data[0].a);
		glEnableVertexAttribArray(2);

		/// 绘制
		glDrawArrays(GL_TRIANGLE_STRIP,0,n);
		*/
		
		GLuint mVertexBufferObject, mTexCoordBufferObject, mAttributeBufferObject;
		// 装载顶点
		glGenBuffers(1, &mVertexBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, n * sizeof(vertexData), &vertexD[0].x, GL_STREAM_DRAW);
		// 装载纹理
		glGenBuffers(1, &mTexCoordBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, mTexCoordBufferObject);
		glBufferData(GL_ARRAY_BUFFER, n * sizeof(vertexData), &vertexD[0].s, GL_STREAM_DRAW);
		// 装载属性
		glGenBuffers(1, &mAttributeBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, mAttributeBufferObject);
		glBufferData(GL_ARRAY_BUFFER, n * sizeof(vertexData), &vertexD[0].a, GL_STREAM_DRAW);

		// 绑定顶点
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,2,GL_FLOAT, GL_FALSE, sizeof(vertexData),0);
		// 绑定纹理
		glBindBuffer(GL_ARRAY_BUFFER, mTexCoordBufferObject);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,2,GL_FLOAT, GL_TRUE, sizeof(vertexData),0);
		// 绑定属性
		glBindBuffer(GL_ARRAY_BUFFER, mAttributeBufferObject);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_TRUE, sizeof(vertexData), NULL);

		/// 绘制
		glDrawArrays(GL_TRIANGLE_STRIP,0,n);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		/// 消除
		glDeleteBuffers(1, &mVertexBufferObject);
		glDeleteBuffers(1, &mTexCoordBufferObject);
		glDeleteBuffers(1, &mAttributeBufferObject);
		
	#endif
		
	CZCheckGLError();

	delete [] vertexD;

	return dataBounds;
}

/// 绘制一个stamp点
void CZPath::paintStamp(CZRandom *randomizer)
{
	if(randomizer == NULL) 
	{
		LOG_ERROR("randomizer is NULL");
		return;
	}

	float weight = scale * (limitBrushSize ? 50 : ptrBrush->weight.value);

	CZ2DPoint start(nodes[0].anchorPoint.x, nodes[0].anchorPoint.y);

	float brushSize = weight;
	float rotationalScatter = randomizer->nextFloat() * ptrBrush->rotationalScatter.value * M_PI * 2;
	float angleOffset = ptrBrush->angle.value * (M_PI / 180.0f);
	float alpha = CZUtil::Max(0.01, ptrBrush->intensity.value);

	points.push_back(start);
	sizes.push_back(brushSize);
	angles.push_back(rotationalScatter+angleOffset);
	alphas.push_back(alpha);
}

/// 绘制两点之间的线.
///
///		将两绘制点(linePoints)之间的线离散成更小粒度的离散点(points)，
///		更新points,sizes,alphas,angles等向量，
///		这里利用到了画刷的参数。
///
///		/param lastLocation - 轨迹最后离散点的位置
///		/param location		- 当前绘制点的位置
///		/param randomizer	- 随机器
///		/note	利用画笔参数生成部分的算法没看懂
void CZPath::paintBetweenPoints(const CZ3DPoint &lastLocation, const CZ3DPoint &location, CZRandom *randomizer)
{
	if(randomizer == NULL) 
	{
		LOG_ERROR("randomizer is NULL");
		return;
	}

	float           pA = lastLocation.z;
	float           pB = location.z;
	float           pDelta = (pB - pA);

	CZ2DPoint		lastLocation2D	= CZ2DPoint(lastLocation.x, lastLocation.y);
	CZ2DPoint		location2D		= CZ2DPoint(location.x, location.y);
	float           distance = lastLocation2D.distanceTo2DPoint(location2D);
	CZ2DPoint       vector = location2D -lastLocation2D;
	CZ2DPoint       unitVector = CZ2DPoint(1.0f, 1.0f);
	float           vectorAngle = atan2(vector.y, vector.x);
	float           pressure = pA;
	float           weight = scale * (limitBrushSize ? 50 : ptrBrush->weight.value);

	if (distance > 0.0f)	unitVector = vector / distance;

	CZ2DPoint start = lastLocation2D + (unitVector*remainder);

	float step = 0.0f, pressureStep = 0.0f;
	float f;
	/// step linearly from last to current, pasting brush image
	for (f = remainder; f <= distance; f += step, pressure += pressureStep) {

		int sign = ptrBrush->weightDynamics.value >= 0 ? 0:1;
		float p = sign ? pressure : (1.0f - pressure);
		float brushSize = CZUtil::Max(1.0f, weight - fabs(ptrBrush->weightDynamics.value) * p * weight);

		float rotationalScatter = randomizer->nextFloat() * ptrBrush->rotationalScatter.value * M_PI * 2;
		float angleOffset = ptrBrush->angle.value * (M_PI / 180.0f);

		float positionalScatter = randomizer->nextFloat(-1.0f,1.0f);
		positionalScatter *= ptrBrush->positionalScatter.value;
		CZ2DPoint orthog;
		orthog.x = unitVector.y;
		orthog.y = -unitVector.x;
		orthog = orthog * weight / 2.0f * positionalScatter;
		CZ2DPoint pos = start + orthog;

		sign = sign = ptrBrush->intensityDynamics.value >= 0 ? 0:1;
		p = sign ? pressure : (1.0f - pressure);
		float alpha = CZUtil::Max(0.01, ptrBrush->intensity.value - fabs(ptrBrush->intensityDynamics.value) * p * ptrBrush->intensity.value);

		points.push_back(pos);
		sizes.push_back(brushSize);
		angles.push_back(vectorAngle * ptrBrush->angleDynamics.value + rotationalScatter + angleOffset);
		alphas.push_back(alpha);

		step = CZUtil::Max(1.0, ptrBrush->spacing.value * brushSize);
		start = start + (unitVector * step);
		pressureStep = pDelta / (distance / step);
	}

	// how much extra spacing should we add when we paint the next time?
	// this keeps the spacing uniform across move events
	remainder = (f - distance);
}

/// 将一连串结点打散，相邻结点用三次贝塞尔曲线连接
/// 
///		两个结点（nodes）形成一根三次贝塞尔曲线，再将曲线打散成若干个绘制点（points）
/// 
///		/param nodes		- 连续的三维结点
///		/param points		- 离散后得到的绘制点容器
///		/return				- 离散后得到的绘制点数目
unsigned int CZPath::flattenNodes2Points(const vector<CZBezierNode> &nodes, bool closed, vector<CZ3DPoint> &points)
{
	size_t numNodes = nodes.size();

	points.clear();

	if (numNodes == 1)
	{
		CZBezierNode lonelyNode = nodes.back();
		points.push_back(lonelyNode.anchorPoint);
		return 1;
	}

	size_t numSegs = closed ? numNodes : numNodes - 1;

	CZBezierSegment   *segment = NULL;
	for (int i = 0; i < numSegs; i++) 
	{
		CZBezierNode a = nodes[i];
		CZBezierNode b = nodes[(i+1) % numNodes];

		segment = CZBezierSegment::segmentBetweenNodes(a,b);
		segment->flattenIntoArray(points);
		delete segment;
	}

	return (unsigned int)points.size();
}

/// 实现coding 接口
void CZPath::update(CZDecoder *decoder_, bool deep /* = false */)
{
	/*
	self.color = [decoder decodeObjectForKey:WDColorKey];
	NSString *brushID = [decoder decodeObjectForKey:WDBrushIDKey];
	self.brush = [[WDActiveState sharedInstance] brushWithID:brushID];
	self.scale = [decoder decodeFloatForKey:WDScaleKey defaultTo:1.f];

	NSString *cnodes = [decoder decodeStringForKey:WDNodesKey];
	if (!cnodes) {
		// handle legacy formats
		NSArray *nodeList = nil;
		nodeList = [decoder decodeArrayForKey:WDNodesKeyV1];
		if (nodeList) {
			self.nodes = [self decodeLegacyNodesA:nodeList v1:YES];
		} else {
			nodeList = [decoder decodeArrayForKey:WDNodesKeyV2];
			if (nodeList) {
				self.nodes = [self decodeLegacyNodesA:nodeList v1:NO];
			} else {
				nodeList = [decoder decodeArrayForKey:WDNodesKeyV3];
				self.nodes = [self decodeLegacyNodesB:nodeList];
			}
		}
	} else {
		NSData *binaryNodes = [NSData dataFromBase64String:cnodes];
		NSMutableArray *nodes = [NSMutableArray arrayWithCapacity:(binaryNodes.length / (9 * sizeof(CFSwappedFloat32)))];
		for (int i = 0; i < binaryNodes.length; i += 9 * sizeof(CFSwappedFloat32)) {
			CFSwappedFloat32 f;
			[binaryNodes getBytes:&f range:NSMakeRange(i + 0 * sizeof(f), sizeof(f))];
			float ix = CFConvertFloat32SwappedToHost(f);
			[binaryNodes getBytes:&f range:NSMakeRange(i + 1 * sizeof(f), sizeof(f))];
			float iy = CFConvertFloat32SwappedToHost(f);
			[binaryNodes getBytes:&f range:NSMakeRange(i + 2 * sizeof(f), sizeof(f))];
			float ip = CFConvertFloat32SwappedToHost(f);
			[binaryNodes getBytes:&f range:NSMakeRange(i + 3 * sizeof(f), sizeof(f))];
			float ax = CFConvertFloat32SwappedToHost(f);
			[binaryNodes getBytes:&f range:NSMakeRange(i + 4 * sizeof(f), sizeof(f))];
			float ay = CFConvertFloat32SwappedToHost(f);
			[binaryNodes getBytes:&f range:NSMakeRange(i + 5 * sizeof(f), sizeof(f))];
			float ap = CFConvertFloat32SwappedToHost(f);
			[binaryNodes getBytes:&f range:NSMakeRange(i + 6 * sizeof(f), sizeof(f))];
			float ox = CFConvertFloat32SwappedToHost(f);
			[binaryNodes getBytes:&f range:NSMakeRange(i + 7 * sizeof(f), sizeof(f))];
			float oy = CFConvertFloat32SwappedToHost(f);
			[binaryNodes getBytes:&f range:NSMakeRange(i + 8 * sizeof(f), sizeof(f))];
			float op = CFConvertFloat32SwappedToHost(f);
			WDBezierNode *node = [WDBezierNode bezierNodeWithInPoint:[WD3DPoint pointWithX:ix y:iy z:ip]
anchorPoint:[WD3DPoint pointWithX:ax y:ay z:ap]
outPoint:[WD3DPoint pointWithX:ox y:oy z:op]];
			[nodes addObject:node];
		}
		self.nodes = nodes;
	}

	boundsDirty_ = YES;
	*/
}
void CZPath::encode(CZCoder *coder_, bool deep /* = false */)
{
	/*
	[coder encodeObject:self.color forKey:WDColorKey deep:deep];
	[coder encodeString:self.brush.uuid forKey:WDBrushIDKey];
	if (self.scale != 1.f) {
		[coder encodeFloat:self.scale forKey:WDScaleKey];
	}

	NSMutableData *binaryNodes = [NSMutableData dataWithCapacity:self.nodes.count * 9 * sizeof(CFSwappedFloat32)];
	for (WDBezierNode *node in self.nodes) {
		CFSwappedFloat32 f;
		f = CFConvertFloat32HostToSwapped(node.inPoint.x);
		[binaryNodes appendBytes:&f length:sizeof(f)];
		f = CFConvertFloat32HostToSwapped(node.inPoint.y);
		[binaryNodes appendBytes:&f length:sizeof(f)];
		f = CFConvertFloat32HostToSwapped(node.inPressure);
		[binaryNodes appendBytes:&f length:sizeof(f)];
		f = CFConvertFloat32HostToSwapped(node.anchorPoint.x);
		[binaryNodes appendBytes:&f length:sizeof(f)];
		f = CFConvertFloat32HostToSwapped(node.anchorPoint.y);
		[binaryNodes appendBytes:&f length:sizeof(f)];
		f = CFConvertFloat32HostToSwapped(node.anchorPressure);
		[binaryNodes appendBytes:&f length:sizeof(f)];
		f = CFConvertFloat32HostToSwapped(node.outPoint.x);
		[binaryNodes appendBytes:&f length:sizeof(f)];
		f = CFConvertFloat32HostToSwapped(node.outPoint.y);
		[binaryNodes appendBytes:&f length:sizeof(f)];
		f = CFConvertFloat32HostToSwapped(node.outPressure);
		[binaryNodes appendBytes:&f length:sizeof(f)];
	}
	[coder encodeString:[[binaryNodes base64EncodedString] stringByReplacingOccurrencesOfString:@"\n" withString:@""]forKey:WDNodesKey];
	*/
}
